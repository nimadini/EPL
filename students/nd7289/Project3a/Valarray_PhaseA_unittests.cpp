/*
 * Valarray_PhaseA_unittests.cpp
 * EPL - Spring 2016
 */

#include <chrono>
#include <complex>
#include <cstdint>
#include <future>
#include <iostream>
#include <stdexcept>

#include "Valarray.h"
#include "gtest/gtest.h"

using std::cout;
using std::endl;
using std::string;
using std::complex;

using namespace epl;

int InstanceCounter::counter = 0;

template <typename X, typename Y>
bool match(X x, Y y) {
    double d = x - y;
    if (d < 0) { d = -d; }
    return d < 1.0e-9; // not really machine epsilon, but close enough
}

/*********************************************************************/
// Phase A Tests
/*********************************************************************/


#if defined(PHASE_A0_1) | defined(PHASE_A)
TEST(PhaseA, BracketOp) {
    valarray<complex<double>> x(10);
    for (int i = 0; i < 10; ++i)
        x[i] = complex<double>(i, i);

    valarray<complex<double>> y = x + x;
    for (int i = 0; i < 10; ++i) {
        complex<double> ans(i, i);
        ans += ans;
        EXPECT_TRUE(match(y[i].real(), ans.real()));
        EXPECT_TRUE(match(y[i].imag(), ans.imag()));
    }

    x = y;
    int i = 0;
    for (auto yi : x) {
        complex<double> ans(i, i);
        ++i;
        ans += ans;
        EXPECT_TRUE(match(yi.real(), ans.real()));
        EXPECT_TRUE(match(yi.imag(), ans.imag()));
    }
}
#endif

#if defined(PHASE_A0_0) | defined(PHASE_A)
TEST(PhaseA, Operators) {
    valarray<int> x(10);
    valarray<int> y(20);
    x = -((2 * (1 + x - y + 1)) / 1);

    EXPECT_EQ(10, x.size());

    for (int i = 0; i < 10; i++) {
        EXPECT_EQ(-4, x[i]);
    }
}
#endif


#if defined(PHASE_A1_0) | defined(PHASE_A)
TEST(PhaseA1, Complex) {
    valarray<complex<float>> x(10);
    valarray<double> y = { 1.5, 2.5, 3.5 };

    for (int i = 0; i < 10; i++)
        x[i] = complex<float>(0.0, 0.0);

    valarray<complex<double>> z = y + x;

    auto t = y + x;
    EXPECT_EQ(t[0], complex<double>(1.5, 0));

    valarray<complex<double>> r = y + y + z;
    EXPECT_EQ(r[0], complex<double>(4.5, 0));
    EXPECT_EQ(r[1], complex<double>(7.5, 0));
    EXPECT_EQ(r[2], complex<double>(10.5, 0));

    valarray<int> elems{1, 1, 1};

    double d = 1.5;
    float f = 1.0;
    auto result = elems + f + d;
    EXPECT_EQ(typeid(result[0]), typeid(double));

    auto result2 = elems + f + d + complex<double>(1.0, 1.0);
    EXPECT_EQ(result2[0], complex<double>(4.5, 1.0));
}
#endif

#if defined(PHASE_A2_0) | defined(PHASE_A)
TEST(PhaseA2, Lazy) {
    // lazy evaluation test
    valarray <double> v1, v2, v3, v4;
    for (int i = 0; i<10; ++i) {
        v1.push_back(1.0);
        v2.push_back(1.0);
        v3.push_back(1.0);
        v4.push_back(1.0);
    }
    int cnt = InstanceCounter::counter;
    v1 + v2 - (v3 * v4);
    EXPECT_EQ(cnt, InstanceCounter::counter);
    
    valarray<double> ans(10);
    ans = v1 + v2 - (v3*v4);
    EXPECT_TRUE(match(ans[3], (v1[3] + v2[3] - (v3[3] * v4[3]))));
}
#endif

#if defined(PHASE_A2_0) | defined(PHASE_A)
TEST(PhaseA2, LazyAccurate) {
    // lazy evaluation test
  InstanceCounter::counter = 0;
    valarray <double> v1, v2, v3, v4, v5, v6;
    for (int i = 0; i<10; ++i) {
        v1.push_back(1.0);
        v2.push_back(1.0);
        v3.push_back(1.0);
        v4.push_back(1.0);
        v5.push_back(1.0);
        v6.push_back(1.0);
    }
    int cnt = InstanceCounter::counter;
    v1 + v2 - (v3 * v4);
    EXPECT_EQ(cnt, InstanceCounter::counter);
    
    valarray<double> ans(10);

    cnt = InstanceCounter::counter;
    
    ans = v1 + v2 - v3 * v4 + v5 / v6;

    EXPECT_TRUE(InstanceCounter::counter - cnt == 0);

    cnt = InstanceCounter::counter;

    cout << "\n******************************\n";

    valarray<double> res = v1 + v2 - v3 * v4 + v5 / v6;

    std::cout << "instances: " << InstanceCounter::counter << "\n";

    EXPECT_EQ(1, InstanceCounter::counter - cnt);
}
#endif

#if defined(PHASE_A1_0) | defined(PHASE_A)
TEST(PhaseA1, primitivePromotions) {
    valarray<int> x{1, 1, 1};
    complex<double> d{ 1.5, 1.5 };
    auto z = x + d;

    std::cout << z[0] << "\n**\n";
}
#endif

/*
#if defined(PHASE_A1_0) | defined(PHASE_A)
TEST(PhaseA1, CreationCTor) {
    valarray<int> x{1, 1, 1};
    valarray<double> y{1.5, 1.5};

    auto z = x + y;

    std::cout << "$$HERETYPE$$:\n";
    std::cout << typeid(z).name() << "\n**********\n";

}
#endif
*/

#if defined(PHASE_A1_0) | defined(PHASE_A)
TEST(PhaseA1, EXPR) {
    valarray<int> x{ 1, 1, 1 };
    valarray<double> y{ 2.5, 2.5 };

    Expression<valarray<int>, valarray<double>, std::plus<>> expr {x, y};

    valarray<int> z {3, 3, 3};
    valarray<double> w{ 4.5, 4.5 };

    expr = Expression<valarray<int>, valarray<double>, std::plus<>>{ z, w };
}
#endif



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

#if defined(PHASE_A0) | defined(PHASE_A)
TEST(PhaseA, piazza_131) {
  valarray<complex<float>> x{1,2,3};
 
  valarray<complex<double>> y(3);  y = x;  // copy assignment
  valarray<complex<double>> z = x;  // conversion
  valarray<complex<double>> t{x};  // copy construct
  valarray<complex<int>> s{x};  // reduced precision
}
#endif


#if defined(PHASE_A0_2) | defined(PHASE_A)
TEST(PhaseA, OperatorPlusAssignment) {
    valarray<int> x(10);
    valarray<int> y(20);

    for (uint64_t i = 0; i < x.size(); ++i)
     x[i] = i;

    for (uint64_t i = 0; i < y.size(); ++i)
     y[i] = i + 100;

    valarray<int> z = x + y;
    for (uint64_t i = 0; i < x.size(); ++i) {
     int ans = x[i] + y[i];
     EXPECT_TRUE(match(z[i], ans));
    }
}
#endif

#if defined(PHASE_A0_3) | defined(PHASE_A)
TEST(PhaseA, OperatorPlusEquals) {
    valarray<int> x(10);
    valarray<int> y(20);

    for (uint64_t i = 0; i < x.size(); ++i)
     x[i] = i;

    for (uint64_t i = 0; i < y.size(); ++i)
     y[i] = i + 100;

    y = y + x;
    for (uint64_t i = 0; i < x.size(); ++i) {
     int ans = i + i + 100;
     EXPECT_TRUE(match(y[i], ans));
    }

    for (uint64_t i = x.size(); i < y.size(); ++i) {
     int ans = i + 100;
     EXPECT_TRUE(match(y[i], ans));
    }
}
#endif

#if defined(PHASE_A0_4) | defined(PHASE_A)
TEST(PhaseA, OperatorPlusScalar) {
    valarray<float> x(10);

    for (uint64_t i = 0; i < x.size(); ++i)
     x[i] = i + .5;

    x = x + 5;
    for (uint64_t i = 0; i < x.size(); ++i) {
     double ans = i + .5 + 5;
     EXPECT_NEAR(ans, x[i], 0.001);
    }

    double toAdd = 4.2;
    auto y = toAdd + x;
    for (uint64_t i = 0; i < y.size(); ++i) {
     double ans = i + .5 + 5 + toAdd;
     EXPECT_NEAR(ans, y[i], 0.001);
     //EXPECT_EQ(ans, x[i]);
     //EXPECT_TRUE(match(ans, x[i]));
    }

}
#endif

#if defined(PHASE_A0_5) | defined(PHASE_A)
TEST(PhaseA, OperatorPlusMultipleSame) {
    valarray<double> x(26); //float
    valarray<double> y(15); //double
    valarray<double> z(10); //complex<float>

    for (uint64_t i = 0; i < x.size(); ++i)
     x[i] = i + .5;

    for (uint64_t i = 0; i < y.size(); ++i)
     y[i] = i + .75;

    for (uint64_t i = 0; i < z.size(); ++i)
     z[i] = i + .2; //complex<float>(i +.2, i);

    valarray<double> w = x + y + z;
    for (uint64_t i = 0; i < w.size(); ++i) {
     double ans = 3*i + .5 + .75 +.2;
     EXPECT_TRUE(match(w[i], ans));
    }
    EXPECT_EQ(w.size(), 10);
}
#endif


#if defined(PHASE_A0_6) | defined(PHASE_A)
TEST(PhaseA, OperatorMinusPlusAssignment) {
    valarray<double> x(10);
    //valarray<int> y(20);
    valarray<complex<float>> y(20);

    for (uint64_t i = 0; i < x.size(); ++i)
     x[i] = i + 100;

    for (uint64_t i = 0; i < y.size(); ++i)
     y[i] = complex<float> (i,i);

    auto z = x - y + x - 8;
    for (uint64_t i = 0; i < z.size(); ++i) {
     double ans = x[i] - i + x[i] - 8;
     EXPECT_TRUE(match(z[i].real(), ans));
    }

    EXPECT_EQ(z.size(), std::min(x.size(), y.size()));
}
#endif

#if defined(PHASE_A0_7) | defined(PHASE_A)
TEST(PhaseA, Constructors) {
    valarray<complex<float>> x{ 1,2,3 };

    valarray<complex<double>> a(3); a = x;  // copy assignment
    valarray<complex<double>> b = x;  // conversion
    valarray<complex<double>> c{ x };  // copy construct
    valarray<complex<int>> d{ x };  // reduced precision
    //valarray<float> e{ x };  // complex to not complex, DOESN'T COMPILE
}
#endif

#if defined(PHASE_A1_0) | defined(PHASE_A)
TEST(PhaseA1, Iterator) {
    valarray<complex<float>> x(10);
    valarray<double> y = { 1.5, 2.5, 3.5 };

    for (int i = 0; i < 10; i++)
     x[i] = complex<float>(i, i+.5);

    auto yiter_begin = y.begin();
    auto yiter_end = y.end();
    valarray<double> z(y.begin(), y.end());
    for (uint64_t i = 0; i < z.size(); i++)
    {
     EXPECT_NEAR(z[i], y[i], 0.001);
    }

}
#endif

#if defined(PHASE_A2_0) | defined(PHASE_A)
TEST(PhaseA2, LazyEvalOps) {
    valarray<complex<float>> x(10);
    valarray<double> y = { 1.5, 2.5, 3.5 , 4.5, 5.5, 6.5 };

    for (int i = 0; i < 10; i++)
     x[i] = complex<float>(i, i + .5);

    //cout << (x + y)[1] << endl;

    auto ans1 = (x + y)[1];
    //cout << typeid(ans1).name() << endl;
    EXPECT_NEAR(3.5, ans1.real(), 0.001);
    EXPECT_NEAR(1.5, ans1.imag(), 0.001);

    auto size1 = (x * y).size();
    EXPECT_EQ(std::min(x.size(), y.size()), size1);
}
#endif


#if defined(PHASE_A2_0) | defined(PHASE_A)
TEST(PhaseA2, newShit) {
   
valarray<int> a1{1, 2};
valarray<double> a2{3.5, 3.2};
std::cout << a1 + a2 << std::endl; // prints {4.5, 5.2}
valarray<int> result = a1 + a2;
std::cout << result << std::endl; // prints {4, 5}
}
#endif


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    auto out = RUN_ALL_TESTS();

#ifdef _MSC_VER
    system("pause");
#endif

    return out;
}
