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

void LifeForm::set_course(double course) {
	update_position();

	this->course = course;
}

void LifeForm::set_speed(double speed) {
	// if there is a border_cross_event:
	if (this->border_cross_event) {
		this->border_cross_event->cancel();	// cancel the existing event
		update_position();					// update the position
	}

	// update the speed
	this->speed = speed;

	// based on the updated speed, compute the distance/time to the boarder (asking for QTree)
	double distanceToBorder = LifeForm::space.distance_to_edge(this->pos, this->course);
	double timeToReachBorder = distanceToBorder / speed;

	// add the threshold to avoid the floating point error
	timeToReachBorder+=Point::tolerance;

	// create a new border cross event
	SmartPointer<LifeForm> self = SmartPointer<LifeForm>(this);
	this->border_cross_event = new Event(timeToReachBorder, [self](void) { self->border_cross(); });
}

void LifeForm::region_resize(void) {

}

void LifeForm::border_cross(void) {

}

void LifeForm::update_position(void) {
	double timeElapsed = Event::now() - this->update_time;

	const double time_tolerance = 1.0e-3; // TODO: find the actual const!!!

	if (timeElapsed < time_tolerance) {
		return;
	}

	// charge for energy consumed! 
	energy -= movement_cost(timeElapsed, this->speed);

	// TODO: resizing?? (called by QuadTree??)

	// if energy < min_energy: die bitch!
	if (energy < min_energy) {
		this->die();
	}

	this->update_time = Event::now();

	// TODO: do we need to check if speed == 0??

	double distance = timeElapsed * this->speed;
	double deltaX = distance * cos(this->course);
	double deltaY = distance * sin(this->course);
	Point delta = Point{ deltaX, deltaY };

	// update the position in the QTree
	this->space.update_position(this->pos, this->pos + delta);

	// update the position in the LifeForm obj
	this->pos += delta;
}

void LifeForm::age(void) {
	energy -= age_penalty;

	if (energy < min_energy) {
		this->die();
	}
}

void LifeForm::reproduce(SmartPointer<LifeForm>) {}

ObjList LifeForm::perceive(double) {
	ObjList* obj = new ObjList();
	return *obj;
}




