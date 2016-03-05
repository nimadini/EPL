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

//TODO [GENERAL]: when shall we set the 1st border_cross_event?
// Should we do it in reproduce for child?

// Note: course can be negative!
void LifeForm::set_course(double course) {
	if (!this->is_alive) {
		return;
	}

	// if there is a border_cross_event:
	if (this->border_cross_event && this->border_cross_event->is_active()) {
		this->border_cross_event->cancel();	// cancel the existing event
		update_position();					// update the position

		if (!this->is_alive) {
			return;
		}
	}

	this->course = course;

	this->compute_next_move();
}

// Note: Speed is non-negative
void LifeForm::set_speed(double speed) {
	if (!this->is_alive) {
		return;
	}

	// if there is a border_cross_event:
	if (this->border_cross_event && this->border_cross_event->is_active()) {
		this->border_cross_event->cancel();	// cancel the existing event
		update_position();					// update the position
		if (!this->is_alive) {
			return;
		}
	}

	// update the speed
	this->speed = std::min(speed, ::max_speed);

	this->compute_next_move();
}

// TODO: In general, check if we are working based on the updated position!!
void LifeForm::compute_next_move(void) {
	if (!this->is_alive) {
		return;
	}

	// if speed is ~ 0.0
	if (speed <= std::numeric_limits<double>::epsilon()) {
		if (this->border_cross_event && this->border_cross_event->is_active()) {
			this->border_cross_event->cancel();
		}
		return;
	}

	// based on the current speed, compute the distance/time to the boarder (asking for QTree)
	double distanceToBorder = LifeForm::space.distance_to_edge(this->pos, this->course);
	double time_to_reach_border = distanceToBorder / speed;

	// add the threshold to avoid the floating point error
	time_to_reach_border += Point::tolerance;

	// create a new border cross event
	SmartPointer<LifeForm> self = SmartPointer<LifeForm>(this);
	this->border_cross_event = 
		new Event(time_to_reach_border, [self](void) { self->border_cross(); });
}

// What you will want to do is have region_resize cancel 
// any pending border crossing events, update your position, 
// and schedule the next movement event

void LifeForm::region_resize(void) {
	std::cout << "stuck!!!\n";
	if (this->border_cross_event && this->border_cross_event->is_active()) {
		this->border_cross_event->cancel();
	}
	this->update_position();
	this->compute_next_move();
}

void LifeForm::gain_energy(double e) {
	if (!this->is_alive) {
		return;
	}

	this->energy += e;
}

void LifeForm::eat(SmartPointer<LifeForm> victim) {
	// if either side is dead, no eating happens
	if (!this->is_alive || !victim->is_alive) {
		return;
	}

	this->energy -= eat_cost_function(this->energy, victim->energy);

	if (this->energy < ::min_energy) {
		this->die();
		victim->die();
		return;
	}

	double energy_earned = victim->energy * eat_efficiency;

	SmartPointer<LifeForm> self = SmartPointer<LifeForm>(this);

	new Event(::digestion_time, [self, energy_earned](void) 
								{ self->gain_energy(energy_earned); });

	victim->die();
}

bool LifeForm::eat_trial(SmartPointer<LifeForm> eater, 
		SmartPointer<LifeForm> victim) {

	double rnd = drand48();
	double esc = eat_success_chance(eater->energy, victim->energy);

	return rnd < esc;
}

void LifeForm::resolve_encounter(SmartPointer<LifeForm> alien) {
	Action my_act = this->encounter(this->info_about_them(alien));

	SmartPointer<LifeForm> self = SmartPointer<LifeForm>(this);

	Action alien_act = alien->encounter(
		alien->info_about_them(self));

	if (my_act == Action::LIFEFORM_EAT && alien_act == Action::LIFEFORM_EAT) {
		bool me_succeed = LifeForm::eat_trial(self, alien);
		bool alien_succeed = LifeForm::eat_trial(alien, self);

		// break the tie based on the strategy
		if (me_succeed && alien_succeed) {
			if (::encounter_strategy == EncounterResolver::EVEN_MONEY) {
				drand48() > 0.5 ? this->eat(alien) : alien->eat(self);

			} else if (::encounter_strategy == EncounterResolver::BIG_GUY_WINS) {
				this->energy > alien->energy ? this->eat(alien) : 
									alien->eat(self);

			} else if (::encounter_strategy == EncounterResolver::UNDERDOG_IS_HERE) {
				this->energy < alien->energy ? this->eat(alien) : 
									alien->eat(self);

			} else if (::encounter_strategy == EncounterResolver::FASTER_GUY_WINS) {
				this->speed > alien->speed ? this->eat(alien) : 
									alien->eat(self);

			} else if (::encounter_strategy == EncounterResolver::SLOWER_GUY_WINS) {
				this->speed < alien->speed ? this->eat(alien) : 
									alien->eat(self);
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

	std::cout << "ENCOUNTERING DONE!!!!\n";
	// else do nothing special (just return)
}

void LifeForm::check_encounter(void) {
	if (!this->is_alive) {
		return;
	}

	// we are assuming there should be a closest!

	SmartPointer<LifeForm> closest = this->space.closest(this->pos);
	
	closest->update_position();

	if (!this->is_alive) {
		return;
	}

	// TODO: check for the condition
	if (this->pos.distance(closest->pos) > ::encounter_distance) {
		return;
	}

	// DIFFERENT WITH CHENGUANG
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
	if (!this->is_alive) {
		return;
	}

	double time_elapsed = Event::now() - this->update_time;

	const double time_tolerance = 1.0e-3; // TODO: find the actual const!!!

	// TODO? shall we check for speed == 0?
	if (time_elapsed < time_tolerance || 
		speed <= std::numeric_limits<double>::epsilon()) {
		return;
	}

	// charge for energy consumed! 
	energy -= movement_cost(time_elapsed, this->speed);

	// if energy < min_energy: die bitch!
	if (energy < ::min_energy) {
		this->die();
		return;
	}

	this->update_time = Event::now();

	double distance = time_elapsed * this->speed;
	double deltaX = distance * cos(this->course);
	double deltaY = distance * sin(this->course);
	Point delta = Point{ deltaX, deltaY };

	// if the new calculated point is outside of QTree boundaries
	if (this->space.is_out_of_bounds(this->pos + delta)) {
		this->die();
		return;
	}

	// update the position in the QTree
	this->space.update_position(this->pos, this->pos + delta);

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

double my_rand(double min, double max) {
    double f = (double)rand() / RAND_MAX;
    return min + f * (max - min);
}

// DOES ALGAE REPRODUCE?
void LifeForm::reproduce(SmartPointer<LifeForm> creature) {
	if (!this->is_alive) {
		return;
	}

	this->update_position();

	if (!this->is_alive) {
		return;
	}

	double time_elapsed = Event::now() - this->reproduce_time;

	if (time_elapsed < ::min_reproduce_time) {
		return;
	}

	double new_energy = (1.0 - ::reproduce_cost) * this->energy / 2.0;

	// maybe redundant (TODO)
	if (new_energy < ::min_energy) {
		return;
	}

	if (::encounter_distance >= ::reproduce_dist) {
		return;
	}

	bool safe_pos_found = false;

	for (int i=0; i < 5; i++) {
		double child_radius = my_rand(::encounter_distance, ::reproduce_dist);
		double child_angle = drand48() * 2.0 * M_PI;
		double deltaX = child_radius * cos(child_angle);
		double deltaY = child_radius * sin(child_angle);

		Point child_pos = this->pos + Point(deltaX, deltaY);

		if (LifeForm::space.is_out_of_bounds(child_pos) 
			|| LifeForm::space.is_occupied(child_pos)) {
			continue;
		}

		SmartPointer<LifeForm> closest_to_child = space.closest(child_pos);

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

    std::cout<<"*******************HERE1\n";

    creature->start_point = creature->pos;

    space.insert(creature, creature->pos,
        [creature](void) { creature->region_resize(); });

    creature->energy = new_energy;
    (void) new Event(age_frequency, [creature]() { creature->age(); });
    creature->is_alive = true;

    this->energy = new_energy;

    this->reproduce_time = Event::now();

    std::cout<<"*******************HERE2\n";
	// assert(!creature->space.is_out_of_bounds(*child_pos));
}

// find the closest nearby object. update its position. if after update the distance is still
// in the collision distance, we have a collision. If not, no collision, skip.

// perceive -> don't need to update_position

// TODO: Where exactly do we need to update our own position? [GENERAL]
// TODO: Where the heck is perceive being invoked? (may need to check if alive there...)
ObjList LifeForm::perceive(double radius) {
	// only alive objects can perceive 
	// {TODO: shall this be checked? (is return val correct?)}
	if (!this->is_alive) {
		return ObjList{};
	}

	if (radius > ::max_perceive_range) {
		radius = ::max_perceive_range;

	} else if (radius < ::min_perceive_range) {
		radius = ::min_perceive_range;
	}

	this->energy -= perceive_cost(radius);

	if (this->energy < ::min_energy) {
		this->die();
		return ObjList{};
	}

	// TODO: do we need to update our own position?
	// this->update_position();

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
