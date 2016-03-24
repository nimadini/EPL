#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

#include "Nima.h"
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

Initializer<Nima> __Nima_initializer;

String Nima::species_name(void) const {
    return "Nima";
}

String Nima::player_name(void) const {
    return "nd7289";
}

bool Nima::is_algae(const ObjInfo& info) {
    return 
        info.species == "Algae" && 
        info.their_speed == 0.0 && 
        info.their_course == 0.0;
}

bool Nima::is_worth_eating(const ObjInfo& info) {
    // always go for algaes
    if (Nima::is_algae(info)) {
        return true;
    }

    // if same species and speed certification valid ignore
    if (info.species == species_name() 
        && is_friend(info.their_speed)) {

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

Action Nima::encounter(const ObjInfo& info) {
    if (is_worth_eating(info)) {
        hunt_event->cancel();
        SmartPointer<Nima> self = SmartPointer<Nima>(this);
        hunt_event = new Event(0.0, [self](void) { self->hunt(); });
        return LIFEFORM_EAT;
    }

    if (this->get_speed() > 4) {
        this->set_speed(this->get_speed()-2);
    }
    
    return LIFEFORM_IGNORE;
}

void Nima::initialize(void) {
    LifeForm::add_creator(Nima::create, "Nima");
}

/*
 * REMEMBER: do not call set_course, set_speed, perceive, or reproduce
 * from inside the constructor!!!!
 * you must wait until the object is actually alive
 */
Nima::Nima() {
    SmartPointer<Nima> self = SmartPointer<Nima>(this);
    new Event(0, [self](void) { self->startup(); });
}

bool Nima::is_friend(double speed) {
    double fraction = speed - ((long)speed);

    if ((long)(fraction * 10000) == 1015) {
        return true;
    }

    return false;
}

void Nima::set_speed(double speed) {
    LifeForm::set_speed(std::ceil(speed) + 0.1015);
}

void Nima::set_course(double course) {
    LifeForm::set_course(course);

    double new_course = this->get_course() + M_PI / 2.0;

    SmartPointer<Nima> self = SmartPointer<Nima>(this);
    new Event(interval, [self, new_course](void) { self->set_course(new_course); });
    interval += 10;
}

Nima::~Nima() {}

void Nima::slow_down(void) {
    set_speed(get_speed() / 2.0);
}

void Nima::startup(void) {
    set_course(drand48() * 2.0 * M_PI);

    double ceil (double x);

    set_speed(3.0 + 3.0 * drand48());

    SmartPointer<Nima> self = SmartPointer<Nima>(this);
    hunt_event = new Event(0, [self](void) { self->hunt(); });

    new Event(50, [self](void) { self->slow_down(); });
}

void Nima::spawn(void) {
    SmartPointer<Nima> child = new Nima;
    reproduce(child);
}

Color Nima::my_color(void) const {
    return BLUE;
}

SmartPointer<LifeForm> Nima::create(void) {
    return new Nima;
}

// TODO: you can override all the protected methods inside LifeForm
// lying is a good idea in this assignment!

// TODO: need a mechanism to distinguish your own species!
void Nima::hunt(void) {
    const String fav_food = "Algae";

    hunt_event = nullptr;
    if (health() == 0.0) { return; } // we died

    double perceive_radius = 18.0 + 2.0 * drand48();

    // if perceiving kills you, just skip it and create a new hunt event
    if (this->health() * ::start_energy - ::perceive_cost(perceive_radius) < ::min_energy) {
        SmartPointer<Nima> self = SmartPointer<Nima>(this);
        hunt_event = new Event(9.0 + 1.0 * drand48(), [self](void) { self->hunt(); });
        return;
    }

    ObjList prey = perceive(perceive_radius);

    double closest_algae = HUGE;
    double closest_creature = HUGE;

    double bearing = 0.0;

    bool algae_found = false;

    for (ObjList::iterator i = prey.begin(); i != prey.end(); ++i) {
        if (Nima::is_algae(*i)) {
            algae_found = true;
            if (closest_algae > (*i).distance) {
                closest_algae = (*i).distance;
                bearing = (*i).bearing;
            }
        }

        else if (!algae_found && (*i).species != "Nima") {
            if (closest_creature > (*i).distance) {
                bearing = (*i).bearing;
                closest_creature = (*i).distance;
            }
        }
    }

    if (closest_algae != HUGE || closest_creature != HUGE) {
        set_course(bearing);
    }

    SmartPointer<Nima> self = SmartPointer<Nima>(this);
    hunt_event = new Event(9.0 + 1.0 * drand48(), [self](void) { self->hunt(); });
    
    // broken!

    /* double new_enegy = (1.0 - ::reproduce_cost) * this->health() * ::start_energy / 2.0;

    std::cout << "NEW_ENERGY: " << new_enegy << "\n";

    if (new_enegy > ::min_energy) {
        spawn();
    } */

    if (health() >= 3.0 + drand48()) spawn();
}
