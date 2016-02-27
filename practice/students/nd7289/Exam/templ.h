#pragma once
#ifndef _LinkedList_h
#define _LinkedList_h

#include <cstdint>
#include <iostream>

namespace epl {

class Base {
public:
  virtual void doit(void) { std::cout << "TRUE\n"; }
  void foo(void) { doit(); }
};

class Derived : public Base {
   int x;
public:
  virtual void doit(void) { std::cout << "FALSE\n"; }
};

template <typename T> void tfun1(T x) { x.doit(); }
template <typename T> void tfun2(T x) { x.foo(); }

} // Namespace epl.

#endif /* _LinkedList_h */
