#ifndef _CFILE_HPP
#define _CFILE_HPP
#include <iostream>

#include <ps/error>
#include <ps/util>

#include "file.hpp"
#include "cache.hpp"

using namespace std;

using util::nat;


template <class T>
class cfile {
public:

  explicit cfile(const string& filename, file::modo m = file::OPEN,
              cache* pC = NULL,
              nat num_pages = 0, 
              nat page_size = file::pagina::DEFAULT_PAGE_SIZE) 
        throw(error);

  ~cfile() throw();

  void read(nat i, T& x) const throw(error);

  void write(nat i, const T& x) throw(error);

  nat size() const throw();

  static const int IndiceFueraRango   = 21;
  static const int TamanyoPaginaInsuf = 22;
private: 

  cfile(const cfile& c) throw() {}
  cfile&  operator=(const cfile& c) throw() { return *this; }

  #include "cfile.rep"
};


template <class T>
void read_bytes(const file::pagina& p, nat ini, T& x) {
  byte* c = new byte[sizeof(T)];
  for (nat i = 0; i < sizeof(T); ++i) { 
    c[i] = p[ini + i];
  }
  x = *(reinterpret_cast<T*>(c));
  delete[] c;
}

template <class T>
void put_bytes(file::pagina& p, nat ini, const T& x) {  
  for (nat i = 0; i < sizeof(T); ++i) {
    p[ini + i] = *(reinterpret_cast<const byte*>(&x) + i);
  }
}

#include "cfile.t"
#endif
