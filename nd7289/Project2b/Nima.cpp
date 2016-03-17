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

String Nima::species_name(void) const
{
    return "Nima";
}

Action Nima::encounter(const ObjInfo& info)
{
    if (info.species == species_name()) {
        /* don't be cannibalistic */
        set_course(info.bearing + M_PI);
        return LIFEFORM_IGNORE;
    }
    else {
        hunt_event->cancel();
        SmartPointer<Nima> self = SmartPointer<Nima>(this);
        hunt_event = new Event(0.0, [self](void) { self->hunt(); });
        return LIFEFORM_EAT;
    }
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

Nima::~Nima() {}

void Nima::startup(void) {
    set_course(drand48() * 2.0 * M_PI);
    set_speed(2 + 5.0 * drand48());
    SmartPointer<Nima> self = SmartPointer<Nima>(this);
    hunt_event = new Event(0, [self](void) { self->hunt(); });
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


void Nima::hunt(void) {
    const String fav_food = "Algae";

    hunt_event = nullptr;
    if (health() == 0.0) { return; } // we died

    ObjList prey = perceive(20.0);

    double best_d = HUGE;
    for (ObjList::iterator i = prey.begin(); i != prey.end(); ++i) {
        if ((*i).species == fav_food) {
            if (best_d > (*i).distance) {
                set_course((*i).bearing);
                best_d = (*i).distance;
            }
        }
    }

    SmartPointer<Nima> self = SmartPointer<Nima>(this);
    hunt_event = new Event(10.0, [self](void) { self->hunt(); });

    if (health() >= 4.0) spawn();
}
