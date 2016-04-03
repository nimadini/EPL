/* main test simulator */
#include <iostream>
#include "CraigUtils.h"
#include "Window.h"
#include "tokens.h"
#include "ObjInfo.h"
#include "QuadTree.h" 
#include "Params.h"
#include "LifeForm.h"
#include "Event.h"

using namespace std;

template <typename T>
void bound(T& x, const T& min, const T& max) {
	assert(min < max);
	if (x > max) { x = max; }
	if (x < min) { x = min; }
}

ObjInfo LifeForm::info_about_them(SmartPointer<LifeForm> neighbor) {
	ObjInfo info;

	info.species = neighbor->species_name();
	info.health = neighbor->health();
	info.distance = pos.distance(neighbor->position());
	info.bearing = pos.bearing(neighbor->position());
	info.their_speed = neighbor->speed;
	info.their_course = neighbor->course;
	return info;
}

// Note: course can be negative!
void LifeForm::set_course(double course) {
	if (!this->is_alive) { return; }

	// note that border_cross_event may be nullptr
	// but in C++ you can legally invoke a method
	// on nullptr and in fact in cancel body we
	// are checking if (this) {...}
	this->border_cross_event->cancel();

	// update your position
	this->update_position();

	// check if still alive
	if (!this->is_alive) { return; }

	// set course
	this->course = course;

	// calculate next move
	this->compute_next_move();
}

// Note: Speed cannot be negative
void LifeForm::set_speed(double speed) {
	if (!this->is_alive || speed < 0) { return; }

	this->border_cross_event->cancel();

	this->update_position();

	if (!this->is_alive) { return; }

	// bound the speed with ::max_speed
	this->speed = std::min(speed, ::max_speed);

	this->compute_next_move();
}

// update the border_cross event
void LifeForm::compute_next_move(void) {
	if (!this->is_alive) { return; }

	// if speed is ~ 0.0: you will never reach another border!
	if (speed <= std::numeric_limits<double>::epsilon()) {
		this->border_cross_event->cancel();
		return;
	}

	// based on the current speed, compute the distance/time to the boarder (asking QTree)
	double distance_to_border = LifeForm::space.distance_to_edge(this->pos, this->course);
	double time_to_reach_border = distance_to_border / speed;

	// add the threshold to avoid the floating point error [to ensure you will cross the
	// border and not getting closer over-and-over again in a semi-infinite loop!]
	time_to_reach_border += Point::tolerance;

	// create a new border_cross event
	SmartPointer<LifeForm> self = SmartPointer<LifeForm>(this);
	this->border_cross_event = 
		new Event(time_to_reach_border, [self](void) { self->border_cross(); });
}

void LifeForm::region_resize(void) {
	this->border_cross_event->cancel();
	this->update_position();
	this->compute_next_move();
}

void LifeForm::gain_energy(double e) {
	if (!this->is_alive) { return; }

	this->energy += e;
}

// this method implementation assumes eat 
// has been successful when you enter the body 
void LifeForm::eat(SmartPointer<LifeForm> victim) {
	// if either side is dead, no eating happens
	if (!this->is_alive || !victim->is_alive) { return; }

	// energy that the eater will earn after digestion_time
	double energy_to_earn = victim->energy * ::eat_efficiency;

	// victim shoud die regardless of the eater's energy
	victim->die();

	// the energy that eater pays for eating...
	this->energy -= eat_cost_function(this->energy, victim->energy);

	// eater should die if it does not have enough energy
	if (this->energy < ::min_energy) {
		this->die();
		return;
	}

	SmartPointer<LifeForm> self = SmartPointer<LifeForm>(this);

	// set up the gain_energy event
	new Event(::digestion_time, [self, energy_to_earn](void) 
								{ self->gain_energy(energy_to_earn); });
}

// returns if eater is willing to eat victim
bool LifeForm::eat_trial(SmartPointer<LifeForm> eater, 
		SmartPointer<LifeForm> victim) {

	double rnd = drand48();
	double esc = eat_success_chance(eater->energy, victim->energy);

	return rnd < esc;
}

void LifeForm::resolve_encounter(SmartPointer<LifeForm> alien) {
	Action my_act = this->encounter(this->info_about_them(alien));

	// my action (EAT or IGNORE)
	SmartPointer<LifeForm> self = SmartPointer<LifeForm>(this);

	// alien's action
	Action alien_act = alien->encounter(alien->info_about_them(self));

	// if both sides are willing to eat each other
	if (my_act == Action::LIFEFORM_EAT && alien_act == Action::LIFEFORM_EAT) {
		bool me_succeed = LifeForm::eat_trial(self, alien);
		bool alien_succeed = LifeForm::eat_trial(alien, self);

		// break the tie based on the strategy
		if (me_succeed && alien_succeed) {
			if (::encounter_strategy == EncounterResolver::EVEN_MONEY) {
				drand48() > 0.5 ? this->eat(alien) : alien->eat(self);

			} else if (::encounter_strategy == EncounterResolver::BIG_GUY_WINS) {
				this->energy > alien->energy ? this->eat(alien) : alien->eat(self);

			} else if (::encounter_strategy == EncounterResolver::UNDERDOG_IS_HERE) {
				this->energy < alien->energy ? this->eat(alien) : alien->eat(self);

			} else if (::encounter_strategy == EncounterResolver::FASTER_GUY_WINS) {
				this->speed > alien->speed ? this->eat(alien) : alien->eat(self);

			} else if (::encounter_strategy == EncounterResolver::SLOWER_GUY_WINS) {
				this->speed < alien->speed ? this->eat(alien) : alien->eat(self);
			}

		} else if (me_succeed) {
			this->eat(alien);

		} else if (alien_succeed) {
			alien->eat(self);
		}

	} else if (my_act == Action::LIFEFORM_EAT && alien_act == Action::LIFEFORM_IGNORE) {
		if (LifeForm::eat_trial(self, alien)) {
			this->eat(alien);
		}
		
	} else if (my_act == Action::LIFEFORM_IGNORE && alien_act == Action::LIFEFORM_EAT) {
		if (LifeForm::eat_trial(alien, self)) {
			alien->eat(self);
		}
	}
}

void LifeForm::check_encounter(void) {
	if (!this->is_alive) { return; }

	// we assume there should be a closest point in the simulation.
	// i.e., there are at least one other creature present.
	SmartPointer<LifeForm> closest = this->space.closest(this->pos);
	
	// update the closest point's position
	closest->update_position();

	if (!this->is_alive) { return; }

	// if still in encounter_distance
	if (this->pos.distance(closest->pos) > ::encounter_distance) {
		return;
	}

	this->energy -= encounter_penalty;
	closest->energy -= encounter_penalty;

	bool both_alive = true;

	if (this->energy < ::min_energy) {
		both_alive = false;
		this->die();
	}

	if (closest->energy < ::min_energy) {
		both_alive = false;
		closest->die();
	}

	if (both_alive) {
		this->resolve_encounter(closest);
	}
}

void LifeForm::border_cross(void) {
	// no need to cancel the border_cross_event here,
	// since it has already been occured and therefore canceled

	this->update_position();

	// we need to check for encounters here
	this->check_encounter();

	this->compute_next_move();
}

void LifeForm::update_position(void) {
	if (!this->is_alive) { return; }

	double time_elapsed = Event::now() - this->update_time;

	// if the time passed since we updated position
	// is negligible, no need to update again, just return!
	if (time_elapsed < ::min_delta_time) { 
		return; 
	}

	// charge for energy consumed! 
	energy -= movement_cost(time_elapsed, this->speed);

	// if energy < min_energy: die bitch!
	if (energy < ::min_energy) {
		this->die();
		return;
	}

	// calculate the dela point
	double distance = time_elapsed * this->speed;
	double deltaX = distance * cos(this->course);
	double deltaY = distance * sin(this->course);
	Point delta = Point{ deltaX, deltaY };

	// if the new calculated point is outside of QTree boundaries
	if (this->space.is_out_of_bounds(this->pos + delta)) {
		this->die();
		return;
	}

	// update the last updated time
	// TODO: updating the place of this line to the end causes 
	// the program to crash! (in QTree Line 620)
	this->update_time = Event::now();

	// update the position in the QTree
	if (this->pos != this->pos + delta) {
		LifeForm::space.update_position(this->pos, this->pos + delta);
	}

	// update the position in the LifeForm obj
	this->pos += delta;
}

// Note: the initial aging event has been
// created in the LifeForm::create_life 
void LifeForm::age(void) {
	this->energy -= ::age_penalty;

	if (energy < ::min_energy) {
		this->die();
		return;
	}

	SmartPointer<LifeForm> self = SmartPointer<LifeForm>(this);

	// creating the new aging event: the existing one, will be removed
	// right after this method execution terminates.
	new Event(::age_frequency, [self](void) { self->age(); });
}

// generates a random number in range
double my_rand(double min, double max) {
    double f = (double)rand() / RAND_MAX;
    return min + f * (max - min);
}

void LifeForm::reproduce(SmartPointer<LifeForm> creature) {
	if (!this->is_alive) { return; }

	this->update_position();

	if (!this->is_alive) { return; }

	double time_elapsed = Event::now() - this->reproduce_time;

	if (time_elapsed < ::min_reproduce_time) {
		return;
	}

	// the new energy for both parent and child
	double new_energy = (1.0 - ::reproduce_cost) * this->energy / 2.0;

	if (new_energy < ::min_energy) {
		this->die();
		return;
	}

	if (::encounter_distance >= ::reproduce_dist) {
		return;
	}

	bool safe_pos_found = false;

	for (uint32_t i=0; i < 5; i++) {
		double child_radius = my_rand(::encounter_distance, ::reproduce_dist);
		double child_angle = drand48() * 2.0 * M_PI;
		double deltaX = child_radius * cos(child_angle);
		double deltaY = child_radius * sin(child_angle);

		Point child_pos = this->pos + Point(deltaX, deltaY);

		// if child pos is out of bounds or is already occupied
		if (LifeForm::space.is_out_of_bounds(child_pos) 
			|| LifeForm::space.is_occupied(child_pos)) {
			continue;
		}

		SmartPointer<LifeForm> closest_to_child = space.closest(child_pos);

		// if child pos is in encounter distance with someone else
		if (child_pos.distance(closest_to_child->position()) < ::encounter_distance) {
			continue;
		}

		safe_pos_found = true;

		creature->pos = child_pos;

		break;
	}

	if (!safe_pos_found) {
		return;
	}

    creature->start_point = creature->pos;

    // inser the child into QTree (with region_resize() callback function)
    space.insert(creature, creature->pos,
        [creature](void) { creature->region_resize(); });

    // create an age event
    (void) new Event(age_frequency, [creature]() { creature->age(); });

    // mark creature as alive
    creature->is_alive = true;

    creature->energy = new_energy;
    
    this->energy = new_energy;

    this->reproduce_time = Event::now();
}

ObjList LifeForm::perceive(double radius) {
	// only alive objects can perceive 
	if (!this->is_alive) { return ObjList{}; }

	bound(radius, ::min_perceive_range, ::max_perceive_range);

	this->energy -= perceive_cost(radius);

	if (this->energy < ::min_energy) {
		this->die();
		return ObjList{};
	}

	// list of obj infos in the given radius in QTree
	ObjList items_perceived{};

	// list of nearby LifeForms
	std::vector<SmartPointer<LifeForm>> nearby_objs = 
			this->space.nearby(this->pos, radius);

	for (vector<SmartPointer<LifeForm>>::iterator itr = 
			nearby_objs.begin(); itr < nearby_objs.end(); itr++) {

		items_perceived.push_back(this->info_about_them(*itr));
	}

	return items_perceived;
}
