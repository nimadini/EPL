#pragma once

#if !(_jre794_h)
#define _jre794_h 1

#include <memory>
#include "LifeForm.h"
#include "Init.h"

class jre794 : public LifeForm {
private:
	//Constants
	const int perceive_min = 5;
	const int speed_min = 1;
	const int hunt_speed = 10;
	const std::string food = "Algae";

	const double magic_mult = 1e8;
	const int magic_mod = 53;
	const int mod_result = 2;

	double edges_for_circle = 8;
	int num_turns; // How much I've turned in a spiral
	double speed_rand;
	double perceive_rand;
	double circle_rand;
	std::string current_name;

	bool is_food(const ObjInfo & info);
	bool is_friend(const ObjInfo & info);
	double fix_number(double);
	bool test_number(double);
protected:
	static void initialize(void);
	void startup(void);
	Event* look_event;
	void look(void);
	void patrol(void);
	void spawn(void);
	void spawn(double speed, double perceive, double circle);

	void set_speed(double);
	void set_course(double);
	ObjList perceive(double);
public:
	jre794(void);
	jre794(double, double, double);
	~jre794(void);
	Color my_color(void) const;   // defines LifeForm::my_color
	static SmartPointer<LifeForm> create(void);
	virtual std::string player_name(void) const override;
	virtual std::string species_name(void) const override;
	virtual Action encounter(const ObjInfo&);
	friend class Initializer<jre794>;
};
#endif /* !(_jre794_h) */
