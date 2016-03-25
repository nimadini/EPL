#if !(_Nima_h)
#define _Nima_h 1

#include <memory>
#include <inttypes.h>
#include "LifeForm.h"
#include "Init.h"

class Nima : public LifeForm {
protected:
  static void initialize(void);
  void spawn(void);
  void hunt(void);
  void startup(void);
  bool is_worth_eating(const ObjInfo& info);
  static bool is_algae(const ObjInfo& info);
  void slow_down(void);
  long interval = 10;
  void set_course(double);
  void set_speed(double);
  bool is_friend(double);
  std::string sp_name = "Nima";
  Event* hunt_event;
public:
  Nima(void);
  ~Nima(void);
  Color my_color(void) const;   // defines LifeForm::my_color
  static SmartPointer<LifeForm> create(void);
  virtual std::string species_name(void) const;
  virtual std::string player_name(void) const;
  virtual Action encounter(const ObjInfo&);
  friend class Initializer<Nima>;
};


#endif /* !(_Nima_h) */
