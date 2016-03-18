#if !(_Nima_h)
#define _Nima_h 1

#include <memory>
#include "LifeForm.h"
#include "Init.h"

class Nima : public LifeForm {
protected:
  static void initialize(void);
  void spawn(void);
  void hunt(void);
  void startup(void);
  Action proceed_to_eat();
  Event* hunt_event;
public:
  Nima(void);
  ~Nima(void);
  Color my_color(void) const;   // defines LifeForm::my_color
  static SmartPointer<LifeForm> create(void);
  virtual std::string species_name(void) const;
  virtual Action encounter(const ObjInfo&);
  friend class Initializer<Nima>;
};


#endif /* !(_Nima_h) */
