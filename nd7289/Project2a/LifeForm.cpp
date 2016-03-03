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
	if (!this->is_alive) {
		return;
	}

	// if there is a border_cross_event:
	if (this->border_cross_event && this->border_cross_event->is_active()) {
		this->border_cross_event->cancel();	// cancel the existing event
		update_position();					// update the position
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

	// if spees is ~ 0.0
	if (speed <= std::numeric_limits<double>::epsilon()) {
		this->border_cross_event = nullptr;
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
	this->border_cross_event->cancel();
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

	this->energy -= eat_cost_function(); // TODO: what are the 2 double arguments?!

	// eating unsuccessful (TODO: is this the correct behavior?)
	if (this->energy < ::min_energy) {
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

	Action alien_act = alien->encounter(
		alien->info_about_them(SmartPointer<LifeForm>(this)));

	if (my_act == Action::LIFEFORM_EAT && alien_act == Action::LIFEFORM_EAT) {
		bool me_succeed = LifeForm::eat_trial(SmartPointer<LifeForm>(this), alien);
		bool alien_succeed = LifeForm::eat_trial(alien, SmartPointer<LifeForm>(this));

		// break the tie based on the strategy
		if (me_succeed && alien_succeed) {
			if (::encounter_strategy == EncounterResolver::EVEN_MONEY) {
				drand48() > 0.5 ? this->eat(alien) : alien->eat(SmartPointer<LifeForm>(this));

			} else if (::encounter_strategy == EncounterResolver::BIG_GUY_WINS) {
				this->energy > alien->energy ? this->eat(alien) : 
									alien->eat(SmartPointer<LifeForm>(this));

			} else if (::encounter_strategy == EncounterResolver::UNDERDOG_IS_HERE) {
				this->energy < alien->energy ? this->eat(alien) : 
									alien->eat(SmartPointer<LifeForm>(this));

			} else if (::encounter_strategy == EncounterResolver::FASTER_GUY_WINS) {
				this->speed > alien->speed ? this->eat(alien) : 
									alien->eat(SmartPointer<LifeForm>(this));

			} else if (::encounter_strategy == EncounterResolver::SLOWER_GUY_WINS) {
				this->speed < alien->speed ? this->eat(alien) : 
									alien->eat(SmartPointer<LifeForm>(this));
			}

		} else if (me_succeed) {
			this->eat(alien);

		} else if (alien_succeed) {
			alien->eat(SmartPointer<LifeForm>(this));
		}

	} else if (my_act == Action::LIFEFORM_EAT && alien_act == Action::LIFEFORM_IGNORE) {
		if (LifeForm::eat_trial(SmartPointer<LifeForm>(this), alien)) {
			this->eat(alien);
		}
		
	} else if (my_act == Action::LIFEFORM_IGNORE && alien_act == Action::LIFEFORM_EAT) {
		if (LifeForm::eat_trial(alien, SmartPointer<LifeForm>(this))) {
			alien->eat(SmartPointer<LifeForm>(this));
		}

	}
	// else do nothing special (just return)
}

void LifeForm::check_encounter(void) {
	if (!this->is_alive) {
		return;
	}

	SmartPointer<LifeForm> closest = this->space.closest(this->pos, encounter_distance);

	if (closest) {
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
}

void LifeForm::border_cross(void) {
	// no need to cancel the border_cross_event here,
	// since it has already been occured and therefore canceled

	this->update_position();
	this->compute_next_move();

	// we need to check for encounters here
	this->check_encounter();
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

void LifeForm::reproduce(SmartPointer<LifeForm> creature) {
	if (!this->is_alive) {
		return;
	}

	double time_elapsed = Event::now() - this->reproduce_time;

	if (time_elapsed < ::min_reproduce_time) {
		return;
	}

	double new_energy = (1.0 - ::reproduce_cost) * 100/2.0 * this->energy;

	if (new_energy < ::min_energy) {
		return;
	}

	// TODO: Insertion logic now commented out (broken!)

	/*std::vector<SmartPointer<LifeForm>> nearby_spots = 
			this->space.nearby(this->pos, ::reproduce_dist);

	SmartPointer<LifeForm> child_spot = nullptr;

	for(vector<string>::iterator itr = nearby_spots.begin(); 
		itr < nearby_spots.end(); itr++) {

		if (!this->space.is_occupied(*itr)) {
			child_spot = *itr;
			break;
		}
	}

	if (!child_spot) {
		return;
	} */

	creature->energy = new_energy;
	this->energy = new_energy;
}

// TODO: Where exactly do we need to update our own position? [GENERAL]

ObjList LifeForm::perceive(double radius) {
	// only alive objects can perceive 
	// {TODO: shall this be checked? (is return val correct?)}
	if (!this->is_alive) {
		return ObjList{};
	}

	this->energy -= perceive_cost(radius);

	if (this->energy < ::min_energy) {
		this->die();
		return;
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
