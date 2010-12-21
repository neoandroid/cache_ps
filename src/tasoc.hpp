#ifndef _TASOC_HPP
#define _TASOC_HPP

#include <iostream>

#include <ps/error>
#include <ps/util>

#include "cfile.hpp"

using namespace std;


template <class T>
class Hash {
private:
  nat M_;
public:
  explicit Hash(nat M) throw();
  int operator()(const T& x) const throw();
};


template <class T, template <class> class HashFunct = Hash>
class tasoc {
public:


  tasoc(const string& filename, 
        cache* pC = NULL,
        nat M = 1009, nat Mt = 1109) throw(error);

  ~tasoc() throw();

  void inserta(const T& x) throw(error);

  bool contiene(const T& x) const throw();

  nat size() const throw();

  void print(ostream& os) const throw();

  static const int TamanyoIncorrecto = 41;
  static const int TablaLlena = 42;

private:
  tasoc(const tasoc& c) throw() {}
  tasoc&  operator=(const tasoc& c) throw() { return *this; }


  #include "tasoc.rep"
};
#include "tasoc.t"
#endif
