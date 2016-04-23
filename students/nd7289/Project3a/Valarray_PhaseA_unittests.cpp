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
  valarray<int> x;
  
  x.push_back(1);
  x.push_back(1);
  x.push_back(1);

  valarray<int> y;
  y.push_back(3);
  y.push_back(3);
  y.push_back(3);
  y.push_back(3);

  y+=x;

  std::cout << y << "\n";
  // std::cout << x + y << "\n";
}
#endif
