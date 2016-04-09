#include <cstdint>
#include <stdexcept>
#include <list>
#include "gtest/gtest.h"
#include "Vector.h"

//define PHASE_C

using epl::vector;
using namespace std;

#ifdef PHASE_C

namespace{
  //Class Instrumentation
  class Zoe {
  public:
    int id = 0;

    Zoe(void) = default;

    Zoe(int id) {
      this->id = id;
    }
  };

  class Foo {
  public:
    int key;
    Zoe barnes;

    bool alive;
    
    static uint64_t constructions;
    static uint64_t destructions;
    static uint64_t copies;
    static uint64_t moves;
    static void reset(){ moves=copies=destructions=constructions=0; }


    Foo(int key, Zoe barnes) {
      this->key = key;
      this->barnes = barnes;
      alive = true;
      ++constructions;
    }

    Foo(void) { alive = true; ++constructions; }
    ~Foo(void) { destructions += alive;}
    Foo(const Foo&) noexcept { alive = true; ++copies; }
    Foo(Foo&& that) noexcept { that.alive = false; this->alive = true; ++moves;}
  };

  uint64_t Foo::constructions = 0;
  uint64_t Foo::destructions = 0;
  uint64_t Foo::copies = 0;
  uint64_t Foo::moves = 0;

  class Bar {
    public:
     uint64_t num1;
     uint64_t num2;
     Bar(void) { this->num1 = 0; this->num2 = 0; }
     Bar(uint64_t arg1, uint64_t arg2) { this->num1 = arg1; this->num2 = arg2; }
     bool operator==(Bar const & rhs) { return this->num1 == rhs.num1 && this->num2 == rhs.num2; }
  };
} //namespace

TEST(PhaseC, direct_init_list) {
	epl::vector<int> w{ 1, 2, 3 };
	EXPECT_EQ(3, w.size());
}

#define ARRAY_SIZE(X) (sizeof(X)/sizeof(*X))
TEST(PhaseC, copy_init_list) {
  epl::vector<int> x = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 42, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  int ans[] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 42, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  EXPECT_EQ(ARRAY_SIZE(ans), x.size());
  for(unsigned int i=0; i<ARRAY_SIZE(ans); ++i)
    EXPECT_EQ(x[i], ans[i]);

  EXPECT_EQ(x.size(), 21);
}

TEST(PhaseC, range_for_loop) {
	epl::vector<int32_t> x(10);

	int32_t k = 0;
	for (auto& v : x) {
		v = k++;
	}

	const epl::vector<int32_t>& y = x;
	int32_t s = 0;
	for (const auto& v : y) {
		s += v;
	}

	EXPECT_EQ(45, s);
}

TEST(PhaseC, ItrExceptSevere) {
  epl::vector<int> x(1);
  auto itr = begin(x);
  x.pop_back();
  try{
    int a;
    a = *itr;
    FAIL();
    *itr = a;
  } catch (epl::invalid_iterator ii) {
    EXPECT_EQ(ii.level, epl::invalid_iterator::MILD);
  }
}

TEST(PhaseC, ItrExceptModerate){
	epl::vector<int>  x(3), y{1,2,3};
	auto xi = begin(x);
	
	x = y;
	try{
		*xi = 5;
    FAIL();
	}
	catch(epl::invalid_iterator ex){
    EXPECT_EQ(ex.level, epl::invalid_iterator::MODERATE);
	}
}

TEST(PhaseC, ItrExceptMild){
  epl::vector<int> x(3);
  auto itr = begin(x);
  x.pop_back();
  try{
    volatile int a;
    a = *itr;
    FAIL();
    *itr = a;
  } catch (epl::invalid_iterator ii){
    EXPECT_EQ(ii.level, epl::invalid_iterator::MILD);
  }
}

TEST(PhaseC2, ItrToConstItr) {
    epl::vector<int> x = { 1, 2, 3, 4, 5 };
    auto it = x.begin();
    epl::vector<int>::const_iterator cit = it;
    cit++;
}

TEST(PhaseC, EmplaceBack){
  Foo::reset();

  epl::vector<int> x{};
  
  x.emplace_back(0);
  x.emplace_back(1);
  x.emplace_back(2);

  auto itr = begin(x);

  int i = 0;
  while (itr != end(x)) {
    EXPECT_EQ(*itr, i++);
    itr++;
  }
}

TEST(PhaseC, EmplaceBack_Foo) {
  Foo::reset();

  epl::vector<Foo> x{};

  Foo foo{};
  Foo bar{};

  x.emplace_back(); // T{}
  x.emplace_back(foo); // T{ copy }
  x.emplace_back(bar); // T{ copy }
  x.emplace_back(Foo{}); // T { move }

  x.pop_back(); // destroy

  EXPECT_EQ(4, Foo::constructions);
  EXPECT_EQ(2, Foo::copies);
  EXPECT_EQ(1, Foo::moves);
  EXPECT_EQ(1, Foo::destructions);
}

TEST(PhaseC, EmplaceBack_Foo_Zoe){
  Foo::reset();

  epl::vector<Foo> x{};

  Zoe barnes{21}; 

  x.emplace_back(5, barnes);

  epl::vector<Foo>::iterator itr = begin(x);

  EXPECT_EQ(21, (*itr).barnes.id);
}


TEST(PhaseC, CopyToDifferent){
  Foo::reset();

  epl::vector<Foo> x{};

  epl::vector<Foo> y{};

  x.push_back(Foo{});
  x.push_back(Foo{});
  x.push_back(Foo{});

  epl::vector<Foo>::const_iterator itr = begin(x);

  *itr;

  x = y;

  try{
    *itr;
    FAIL();
  } catch (epl::invalid_iterator ii) {
    EXPECT_EQ(ii.level, epl::invalid_iterator::MODERATE);
  }
}

TEST(PhaseC, CopyToSame){
  Foo::reset();

  epl::vector<Foo> x{};

  x.push_back(Foo{});
  x.push_back(Foo{});
  x.push_back(Foo{});

  epl::vector<Foo>::const_iterator itr = begin(x);

  *itr;

  x = x;

  *itr;
}

//Jo
TEST(PhaseC, ItrEquivalence)
{
    epl::vector<int> x = { 1, 2, 3, 4, 5 };
    auto it = x.begin();
    auto it2 = x.begin();

    EXPECT_TRUE(it == it2);

}
//Jo
TEST(PhaseC, ItrCopyAssn)
{
    epl::vector<int> x = { 1, 2, 3, 4, 5 };
    auto it = x.begin();
    auto it2 = it;

    EXPECT_TRUE(it == it2);
}

//Jo
TEST(PhaseC, ItrCopyConstruct)
{
    epl::vector<int> x = { 1, 2, 3, 4, 5 };
    auto it = x.begin();
    auto it2 (it);

    EXPECT_TRUE(it == it2);
}
//Jo
TEST(PhaseC1, emplacebackPOD)
{
    epl::vector<int> x = { 9, 8, 7, 6, 5, 4, 3 };
    int ans[] = { 9, 8, 7, 6, 5, 4, 3, 2};
    x.emplace_back(2);
    EXPECT_EQ(ARRAY_SIZE(ans), x.size());
    for (unsigned int i = 0; i<ARRAY_SIZE(ans); ++i)
     EXPECT_EQ(x[i], ans[i]);
}
//Jo
TEST(PhaseC1, emplacebackNotPOD)
{
    uint64_t arg1 = 7;
    uint64_t arg2 = 14;
    epl::vector<Bar> x(2);
    Bar ans[] = { Bar{}, Bar{}, Bar{ arg1 , arg2} };
    x.emplace_back(arg1, arg2);
    EXPECT_EQ(ARRAY_SIZE(ans), x.size());
    for (unsigned int i = 0; i<ARRAY_SIZE(ans); ++i)
     EXPECT_TRUE(x[i] == ans[i]);
}

//Jo
TEST(PhaseC2, ItrExceptSevere1)
{
    epl::vector<int> x(5);
    auto itr = end(x);
    x.pop_back();
    try {
     int a;
     a = *itr;
     FAIL();
     *itr = a;
    }
    catch (epl::invalid_iterator ii) {
     EXPECT_EQ(epl::invalid_iterator::SEVERE, ii.level);
    }
}
//Jo
TEST(PhaseC2, ItrExceptOutOfRange)
{
    epl::vector<int> x(5);
    auto itr = end(x);
    try {
     int a;
     a = *itr;
     FAIL();
     *itr = a;
    }
    catch (std::out_of_range ex) {
     //EXPECT_EQ(epl::invalid_iterator::SEVERE, ii.level);
    }
}

//Jo
TEST(PhaseC2, ItrExceptModerate2) {
    epl::vector<int>  x(1);
    auto xi = begin(x);

    //Should cause a resize
    x.push_back(5);

    try {
     *xi = 5;
     FAIL();
    }
    catch (epl::invalid_iterator ex) {
     EXPECT_EQ(ex.level, epl::invalid_iterator::MODERATE);
    }
}

//Jo
TEST(PhaseC2, ItrExceptMild3) {
    epl::vector<int> x(10);
    epl::vector<int>::iterator p = x.begin();
    x.pop_front();
    try {
     if (p == x.begin()) { // p is invalid and MILD
       FAIL();
     }
    }
    catch (epl::invalid_iterator ii) {
     EXPECT_EQ(epl::invalid_iterator::MILD, ii.level);
    }
}


TEST(PhaseC2, ConstructFromRandIterators) {
    Foo::reset();
    {
     epl::vector<Foo> x(20);
     auto it_begin = x.begin();
     auto it_end = x.end();
     epl::vector<Foo> y(it_begin, it_end);

    }

    EXPECT_EQ(20, Foo::constructions);
    EXPECT_EQ(40, Foo::destructions);
    EXPECT_EQ(20, Foo::copies);
    EXPECT_LE(0, Foo::moves);

}

TEST(PhaseC1, ConstructFromListItr) {
    std::list<int> x = { 1, 2, 3, 4, 5 };
    epl::vector<int> y(x.begin(),x.end());
    EXPECT_EQ(y.size(), x.size());
    uint64_t i = 0;
    for (std::list<int>::iterator it = x.begin(); it != x.end(); it++)
    {
     EXPECT_TRUE(*it == y[i]);
     ++i;
    }
}

TEST(PhaseC, ItrSubtraction)
{
    epl::vector<int> x = { 1, 2, 3, 4, 5 };
    auto it_begin = x.begin();
    auto it_end = end(x);

    EXPECT_EQ(x.size(), it_end - it_begin);
}

TEST(PhaseC2, ItrModerateCopy) {
    epl::vector<int> x(1);
    auto x_itr = begin(x); // default

    epl::vector<int> y(1);
    auto y_itr = begin(y); // default
    
    y = x; // 

    *x_itr;

    // x_itr should be valid
    // y_itr should not be valid

    try {
     *y_itr;
     FAIL();
    }
    catch (epl::invalid_iterator ii) {
     EXPECT_EQ(epl::invalid_iterator::MODERATE, ii.level);
    }
}

TEST(PhaseC2, ItrExceptModerateMove1)
{
    epl::vector<int> x(5);
    epl::vector<int> y(5);
    auto x_it = begin(x);

    y = std::move(x);

    try {
     x_it += 4;
     FAIL();
    }
    catch (epl::invalid_iterator ex) {
     EXPECT_EQ(ex.level, epl::invalid_iterator::MODERATE);
    }

}

TEST(PhaseC2, ItrExceptModerateMove2)
{
    epl::vector<int> x(5);
    epl::vector<int> y(5);
    auto y_it = begin(y);

    y = std::move(x);

    try {
     y_it -= 4;
     FAIL();
    }
    catch (epl::invalid_iterator ex) {
     EXPECT_EQ(ex.level, epl::invalid_iterator::MODERATE);
    }

}


TEST(PhaseC, OperatorOverloading) {
    epl::vector<int> x{1, 2, 3, 4, 5};

    auto bIt = begin(x);
    auto eIt = end(x);

    EXPECT_TRUE(bIt < eIt);

    bIt++;
    eIt-=2;

    EXPECT_TRUE(bIt < eIt);
    EXPECT_TRUE(bIt <= eIt);
    EXPECT_TRUE(eIt > bIt);
    EXPECT_TRUE(eIt >= bIt);
    
    EXPECT_EQ(*bIt, 2);
    EXPECT_EQ(bIt[1], 3);
    EXPECT_EQ(*eIt, 4);

    bIt+=1;

    EXPECT_EQ(bIt + 1, eIt);

    EXPECT_TRUE(bIt != eIt);
    EXPECT_TRUE(bIt + 1 == eIt);
}

TEST(PhaseC2, ItrMildEmplaceBack)
{
    uint64_t arg1 = 7;
    uint64_t arg2 = 14;
    epl::vector<Bar> x;

    auto itr = x.begin();
    x.emplace_back(arg1, arg2);
    try {
     itr++;
     FAIL();
     
    }
    catch (epl::invalid_iterator ii) {
     EXPECT_EQ(epl::invalid_iterator::MILD, ii.level);
    }
}

#endif
