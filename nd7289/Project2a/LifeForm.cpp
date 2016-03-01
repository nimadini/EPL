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

// TODO:: can speed/course be negative?

void LifeForm::set_course(double course) {
	// if there is a border_cross_event:
	if (this->border_cross_event) {
		this->border_cross_event->cancel();	// cancel the existing event
		update_position();					// update the position
	}

	this->course = course;

	this->compute_next_move();
}

void LifeForm::set_speed(double speed) {
	// if there is a border_cross_event:
	if (this->border_cross_event) {
		this->border_cross_event->cancel();	// cancel the existing event
		// TODO: shall we set boarder_cross_event to nullptr here???
		update_position();					// update the position
	}

	// if the new speed value is 0.0
	if (speed <= std::numeric_limits<double>::epsilon()) {
		this->speed = 0.0;
		this->border_cross_event = nullptr; // TODO: shall we?
		return;
	}

	// update the speed
	this->speed = speed;

	this->compute_next_move();
}

void LifeForm::compute_next_move(void) {
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
	this->border_cross_event->cancel();
	this->update_position();
	this->compute_next_move();
}

void LifeForm::update_position(void) {
	// TODO: we may need to check if alive,
	// cause if dead, space.update_position may
	// throw an exception...
	double timeElapsed = Event::now() - this->update_time;

	const double time_tolerance = 1.0e-3; // TODO: find the actual const!!!

	// TODO? shall we check for speed == 0?
	if (timeElapsed < time_tolerance || 
		speed <= std::numeric_limits<double>::epsilon()) {
		return;
	}

	// charge for energy consumed! 
	energy -= movement_cost(timeElapsed, this->speed);

	// if energy < min_energy: die bitch!
	if (energy < min_energy) {
		this->die();
	}

	this->update_time = Event::now();

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

void LifeForm::reproduce(SmartPointer<LifeForm> lf) {

}

ObjList LifeForm::perceive(double) {
	ObjList* obj = new ObjList();
	return *obj;
}




