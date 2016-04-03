#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

#include "nd7289.h"
#include "CraigUtils.h"
#include "Event.h"
#include "ObjInfo.h"
#include "Params.h"
#include "Random.h"
#include "Window.h"

#ifdef _MSC_VER
using namespace epl;
#endif
using namespace std;
using String = std::string;

Initializer<nd7289> __nd7289_initializer;

String nd7289::species_name(void) const {
    const std::string sp_name = this->sp_name;
    return sp_name;
    //return this->species_name;
}

String nd7289::player_name(void) const {
    return "nd7289";
}

bool nd7289::is_algae(const ObjInfo& info) {
    return 
        info.species == "Algae" && 
        info.their_speed == 0.0 && 
        info.their_course == 0.0;
}

bool nd7289::is_worth_eating(const ObjInfo& info) {
    // always go for algaes
    /*if (nd7289::is_algae(info)) {
        return true;
    }*/

    // if same species and speed certification valid ignore
    if (is_friend(info.their_speed)) {

        /* don't be cannibalistic */
        set_course(info.bearing + M_PI);
        return false;
    }

    /* commented out: I'll take my chances of eating a larger beast! d=

    if (this->health() < info.health) {
        return false;
    } */

    return true;
}

Action nd7289::encounter(const ObjInfo& info) {
    if (is_worth_eating(info)) {
        hunt_event->cancel();
        SmartPointer<nd7289> self = SmartPointer<nd7289>(this);
        hunt_event = new Event(0.0, [self](void) { self->hunt(); });
        return LIFEFORM_EAT;
    }

    if (this->get_speed() > 4 && drand48() < 0.5) {
        this->set_speed(this->get_speed()-2);

    } else if (drand48() < 0.5) {
        this->set_speed(this->get_speed()+1);
    }
    
    
    return LIFEFORM_IGNORE;
}

void nd7289::initialize(void) {
    LifeForm::add_creator(nd7289::create, "nd7289");
}

/*
 * REMEMBER: do not call set_course, set_speed, perceive, or reproduce
 * from inside the constructor!!!!
 * you must wait until the object is actually alive
 */
nd7289::nd7289() {
    SmartPointer<nd7289> self = SmartPointer<nd7289>(this);
    new Event(0, [self](void) { self->startup(); });
}

bool nd7289::is_friend(double speed) {
    double fraction = speed - ((long)speed);

    if ((long)(fraction * 10000) == 1015) {
        return true;
    }

    return false;
}

void nd7289::set_speed(double speed) {
    LifeForm::set_speed(std::ceil(speed) + 0.1015);
}

void nd7289::set_course(double course) {
    LifeForm::set_course(course);

    double new_course = this->get_course() + M_PI / 2.0;

    SmartPointer<nd7289> self = SmartPointer<nd7289>(this);
    new Event(interval, [self, new_course](void) { self->set_course(new_course); });

    int threshold = ::grid_max / 3;
    interval = (interval + 10) % threshold + 10;
}

nd7289::~nd7289() {}

void nd7289::slow_down(void) {
    set_speed(get_speed() / 2.0);
}

void nd7289::startup(void) {
    set_course(drand48() * 2.0 * M_PI);

    set_speed(2.0 + 3.0 * drand48());

    SmartPointer<nd7289> self = SmartPointer<nd7289>(this);
    hunt_event = new Event(0, [self](void) { self->hunt(); });

    // trigger the slow_down event after 50 time units
    new Event(50, [self](void) { self->slow_down(); });
}

void nd7289::spawn(void) {
    SmartPointer<nd7289> child = new nd7289;
    reproduce(child);
}

Color nd7289::my_color(void) const {
    return BLUE;
}

SmartPointer<LifeForm> nd7289::create(void) {
    return new nd7289;
}

void nd7289::hunt(void) {
    const String fav_food = "Algae";

    hunt_event = nullptr;
    if (health() == 0.0) { return; } // we died

    double perceive_radius = 18.0 + 4.0 * drand48();

    // if perceiving kills you, just skip it and create a new hunt event
    if (this->health() * ::start_energy - ::perceive_cost(perceive_radius) < ::min_energy) {
        SmartPointer<nd7289> self = SmartPointer<nd7289>(this);
        hunt_event = new Event(8.0 + 1.0 * drand48(), [self](void) { self->hunt(); });
        return;
    }

    ObjList prey = perceive(perceive_radius);

    double closest_algae = HUGE;
    double closest_creature = HUGE;
    std::string nearest_enemy_species = "";

    double bearing = 0.0;

    bool algae_found = false;

    for (ObjList::iterator i = prey.begin(); i != prey.end(); ++i) {
        if (nd7289::is_algae(*i)) {
            algae_found = true;
            if (closest_algae > (*i).distance) {
                closest_algae = (*i).distance;
                bearing = (*i).bearing;
            }
        }

        else if (!algae_found && !is_friend((*i).their_speed)) {
            if (closest_creature > (*i).distance) {
                bearing = (*i).bearing;
                closest_creature = (*i).distance;
                nearest_enemy_species = (*i).species;
            }
        }
    }

    if (closest_algae != HUGE || closest_creature != HUGE) {
        set_course(bearing);
    }

    if (closest_creature != HUGE) {
        this->sp_name = nearest_enemy_species;
        // std::cout << "*************: " << this->sp_name << "\n";
    }

    SmartPointer<nd7289> self = SmartPointer<nd7289>(this);
    hunt_event = new Event(8.0 + 1.0 * drand48(), [self](void) { self->hunt(); });
    
    // broken!

    /* double new_enegy = (1.0 - ::reproduce_cost) * this->health() * ::start_energy / 2.0;

    std::cout << "NEW_ENERGY: " << new_enegy << "\n";

    if (new_enegy > ::min_energy) {
        spawn();
    } */

    if (health() >= 3.0 + drand48()) spawn();
}
