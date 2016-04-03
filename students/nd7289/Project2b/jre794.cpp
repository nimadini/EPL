#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

#include "jre794.h"
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

Initializer<jre794> __jre794_initializer;

// Where I tell the truth
String jre794::player_name(void) const
{
	return "jre794";
}

/* 
This is where I lie
current_name will be changed in various phases to whatever
*/
String jre794::species_name(void) const
{
	return current_name;
}

bool jre794::is_food(const ObjInfo& info)
{
	bool result = false;
	if (info.species == this->food && info.their_course == 0.0 && info.their_speed == 0.0)
	{
		result = true;
	}

	return result;
}

bool jre794::is_friend(const ObjInfo& info)
{
	bool result = false;
	if (test_number(info.their_course) && test_number(info.their_speed))
	{
		result = true;
		//cout << "Name is: " << info.species << endl;
	}

	return result;
}

Action jre794::encounter(const ObjInfo& info)
{
	this->num_turns = 0;

	bool isFriend = is_friend(info);
	if (isFriend) {
		/* don't be cannibalistic */
		set_course(info.bearing + M_PI);
		return LIFEFORM_IGNORE;
	}
	else {
		//Stop whatever we're doing
		//Maybe reproduce if we're low enough health
		//Look around to decide what to do next

		look_event->cancel();
		SmartPointer<jre794> self = SmartPointer<jre794>(this);
		look_event = new Event(0.0, [self](void) { self->look(); });
		return LIFEFORM_EAT;
	}
}

void jre794::initialize(void) {
	LifeForm::add_creator(jre794::create, "jre794");
}

/*
* REMEMBER: do not call set_course, set_speed, perceive, or reproduce
* from inside the constructor!!!!
* you must wait until the object is actually alive
*/
jre794::jre794() {

	//Setting state
	this->current_name = "";
	this->num_turns = 0;
	this->speed_rand = drand48() - 0.5;
	this->perceive_rand = drand48() - 0.5;
	this->circle_rand = drand48() - 0.5;

	SmartPointer<jre794> self = SmartPointer<jre794>(this);
	new Event(0, [self](void) { self->startup(); });
}

jre794::jre794(double speed, double perceive, double circle)
{
	//Setting state
	this->current_name = "";
	this->num_turns = 0;
	this->speed_rand = speed + drand48() - 0.5;
	this->perceive_rand = perceive + drand48() - 0.5;
	this->circle_rand = circle + drand48() - 0.5;

	SmartPointer<jre794> self = SmartPointer<jre794>(this);
	new Event(0, [self](void) { self->startup(); });
}

jre794::~jre794() {}

void jre794::startup(void) {
	//I'm going to try to make everyone go in a circle...
	set_course(drand48() * 2 * M_PI);
	set_speed(this->speed_min + this->speed_rand);
	this->edges_for_circle += circle_rand;
	// Look around
	look();
}

void jre794::look(void)
{
	//Might as well reproduce while I'm at it?
	if (health() > 5)
	{
		spawn(this->speed_rand, this->perceive_rand, this->circle_rand);
	}

	ObjList x = perceive(this->num_turns * 2 + perceive_min);
	SmartPointer<jre794> self = SmartPointer<jre794>(this);

	double closest_dist = HUGE;
	ObjInfo closest;
	for (ObjList::iterator i = x.begin(); i != x.end(); ++i) {
		if ((*i).distance < closest_dist)
		{
			closest = *i;
			closest_dist = (*i).distance;
		}
	}
	//See Algae = pursue it
	if (is_food(closest))
	{
		set_course(closest.bearing);
		set_speed(this->hunt_speed);
		double time = closest.distance / this->get_speed();
		//Just in case we don't run into them
		look_event = new Event(time, [self](void) { self->look(); });

		//Need to reset this because we're going off the spiral
		this->num_turns = 0;
	}

	//See really big guy = run the other way
	else if (closest.health > 2 * this->health())
	{
		set_course(closest.bearing + M_PI);
		set_speed(closest.their_speed + 5);

		//Look around once we're safe
		look_event = new Event(5, [self](void) { self->look(); });

		//Need to reset this because we're going off the spiral
		this->num_turns = 0;
	}

	// Else just patrol I guess
	else
	{
		patrol();
	}
}

void jre794::patrol(void)
{
	//Want to continue my spiral then look around
	set_speed(speed_min);
	set_course(get_course() + (2 * M_PI) / edges_for_circle);

	this->num_turns += 1;

	double time = 2 * this->num_turns + 10;
	SmartPointer<jre794> self = SmartPointer<jre794>(this);
	look_event = new Event(time, [self](void) { self->look(); });
}

void jre794::spawn(void) {
	SmartPointer<jre794> child = new jre794;
	reproduce(child);
}

void jre794::spawn(double speed, double perceive, double circle)
{
	SmartPointer<jre794> child = new jre794{speed, perceive, circle};
	reproduce(child);
}

Color jre794::my_color(void) const {
	return (Color)(int)(drand48() * 8); // Rainbow Lifeform
	//return MAGENTA;
}

SmartPointer<LifeForm> jre794::create(void) {
	return new jre794;
}

ObjList jre794::perceive(double radius)
{
	ObjList x = LifeForm::perceive(radius);

	//Iterate through the Info and lie about something
	double closest_distance = HUGE;
	for (ObjList::iterator i = x.begin(); i != x.end(); ++i) {
		if (!is_food((*i)) && closest_distance > (*i).distance) {
			this->current_name = (*i).species;
			closest_distance = (*i).distance;
		}
	}

	return x;
}

void jre794::set_speed(double speed)
{
	LifeForm::set_speed(fix_number(speed));
}

void jre794::set_course(double course)
{
	LifeForm::set_course(fix_number(course));
}

double jre794::fix_number(double input)
{
	double my_num = input * magic_mult;
	int num = (int)my_num % magic_mod;
	my_num += (mod_result - num);
	my_num /= magic_mult;
	return my_num;
}

bool jre794::test_number(double input)
{
	double my_num = input * magic_mult;
	if ((int)my_num % magic_mod == mod_result)
	{
		return true;
	}
	return false;
}