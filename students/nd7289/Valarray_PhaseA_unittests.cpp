#include <cstdint>
#include <stdexcept>
#include <future>
#include <chrono>
#include "gtest/gtest.h"
#include "Valarray.h"

//#define PHASE_A

#ifdef _MSC_VER
#define noexcept
#endif

using epl::valarray;

/*****************************************************************************************/
// Class Instrumentation
/*****************************************************************************************/
namespace{
  //Class Instrumentation
  class Foo {
  public:
    bool alive;
    
    static uint64_t constructions;
    static uint64_t destructions;
    static uint64_t copies;
    static uint64_t moves;
    static void reset(){ moves=copies=destructions=constructions=0; }

    Foo(void) { alive = true; ++constructions; }
    ~Foo(void) { destructions += alive;}
    Foo(const Foo&) noexcept { alive = true; ++copies; }
    Foo(Foo&& that) noexcept { that.alive = false; this->alive = true; ++moves;}
  };

  uint64_t Foo::constructions = 0;
  uint64_t Foo::destructions = 0;
  uint64_t Foo::copies = 0;
  uint64_t Foo::moves = 0;
} //namespace

/*****************************************************************************************/
// Phase A Tests
/*****************************************************************************************/
#if defined(PHASE_A0) | defined(PHASE_A)
TEST(PhaseA, Initial){
  /*valarray<int> x;
  
  x.push_back(1);
  x.push_back(1);
  x.push_back(1);

  valarray<int> y;
  y.push_back(3);
  y.push_back(3);
  y.push_back(3);
  y.push_back(3);

  y+=x;

  std::cout << y << "\n";*/

  epl::valarray<float> s;
  s.push_back(1.5f);
  s.push_back(1.5f);

  epl::valarray<double> t;
  t.push_back(2);
  t.push_back(2);
  t.push_back(2);

  epl::valarray<int> z;
  z.push_back(4);
  z.push_back(4);
  z.push_back(4);

  //epl::Valarray<int> v = 

  std::cout << typeid((s+t+z)[0]).name() << "\n";

  //std::cout << v;
  // std::cout << x + y << "\n";
}
#endif

#if defined(PHASE_A0) | defined(PHASE_A)
TEST(PhaseA, initializer) {

  epl::valarray<int> y;
  y.push_back(2);
  y.push_back(2);

  std::cout << 5 + y;
  std::cout << y + 5;

}
#endif

#if defined(PHASE_A0) | defined(PHASE_A)
TEST(PhaseA, assignment) {

  epl::valarray<int> y = {2, 2};

  epl::valarray<int> res = 5 + y;

  res = res + y - 5;

  res = res * 2;

  std::cout << res;
  std::cout << -res;

}
#endif

#if defined(PHASE_A0) | defined(PHASE_A)
TEST(PhaseA, iterator) {
  epl::valarray<int> x = {2, 2};
  epl::valarray<int> y = {3, 3, 3};

  // (x + y).begin();

}
#endif