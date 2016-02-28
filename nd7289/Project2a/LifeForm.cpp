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

 /*void LifeForm::add_creator(IstreamCreator, const std::string&) {};
 void LifeForm::create_life() {};
/* draw the lifeform on 'win' where x,y is upper left corner */
 /*void LifeForm::draw(int, int) const {};

void LifeForm::display(void) const {};
 void LifeForm::redisplay_all(void) {};
 void LifeForm::clear_screen(void) {};

void LifeForm::set_course(double) {};

std::string LifeForm::player_name(void) const { return "NIMA"; };*/

void LifeForm::set_course(double) {};

void LifeForm::region_resize(void) {};

void LifeForm::update_position(void) {
	double timeElapsed = Event::now() - this->update_time;

	// charge for energy consumed! 
	energy -= movement_cost(timeElapsed, this->speed);

	// TODO: resizing?? (called by QuadTree??)

	// if energy < min_energy: Die bitch!
	if (energy < min_energy) {
		this->die();
	}

	const double time_tolerance = 1.0e-3; // TODO: find the actual const!!!

	if (timeElapsed < time_tolerance) {
		return;
	}

	this->update_time = Event::now();

	double distance = timeElapsed * this->speed;
	double deltaX = distance * cos(this->course);
	double deltaY = distance * sin(this->course);
	Point delta = Point{ deltaX, deltaY };
	this->pos += delta;
}

void LifeForm::age(void) {
	energy -= age_penalty;

	if (energy < min_energy) {
		this->die();
	}
};

void LifeForm::reproduce(SmartPointer<LifeForm>) {};

void LifeForm::set_speed(double) {};

ObjList LifeForm::perceive(double) {
	ObjList* obj = new ObjList();
	return *obj;
};




