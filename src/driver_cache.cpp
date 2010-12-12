#include <iostream>
#include <fstream>
#include <string>
#include <list>

#include <ps/error>
#include <ps/gen_driver>
#include <ps/util>
#include <ps/mem_din>

#include "file.hpp"
#include "cache.hpp"
#include "cfile.hpp"
#include "tasoc.hpp"

using util::nat;
using util::byte;
using namespace std;


// define la clase palabra y Hash<palabra>
// tambien define una funcion para imprimir.

class palabra {
public:
  palabra() {
      for (nat i = 0; i <= MAX_LENGTH_PALABRA; ++i) p_[i] = '\0';
  }
  palabra(const palabra& p) {
    for (nat i = 0; i <= MAX_LENGTH_PALABRA; ++i) p_[i] = p.p_[i];
  }
  palabra& operator=(const palabra& p) {
    for (nat i = 0; i <= MAX_LENGTH_PALABRA; ++i) p_[i] = p.p_[i];
    return *this;
  }
  palabra(const string& s) {
    for (nat i = 0; i <= MAX_LENGTH_PALABRA; ++i) p_[i] = '\0';
    for (nat i = 0; i < s.length() && i < MAX_LENGTH_PALABRA; ++i) 
      p_[i] = s[i];
  }
  char operator[](nat i) const {
    return p_[i];
  }
  char& operator[](nat i) {
    return p_[i];
  }
  void print(ostream& os) const {
    for (nat i = 0; i < MAX_LENGTH_PALABRA && p_[i] != '\0'; ++i)
      os << p_[i];
  }
  bool operator==(const palabra& q) const {
    nat i = 0;
    while ((p_[i] == q.p_[i]) && p_[i] != '\0' && q.p_[i] != '\0') ++i;
    return p_[i] == q.p_[i];
  }
private:
  static const nat MAX_LENGTH_PALABRA = 32;
  char p_[MAX_LENGTH_PALABRA + 1];
};

ostream& operator<<(ostream& os, const palabra& pal) {
  pal.print(os);
  return os;
}

template <> class Hash<palabra> {
private:
  nat M_;
public:
  explicit Hash(nat M) throw() : M_(M) {}
  int operator()(const palabra& p) const throw() {
    int sum = 0;
    for (nat i = 0; p[i] != 0; ++i) 
      sum = (sum + 27 * p[i] * p[i]) % M_;
    return sum;
  }
};


// funcion de hash tonta para int's
template <> class Hash<int> {
private:
  nat M_;
public:
  explicit Hash(nat M) throw() : M_(M) {}
  int operator()(const int& p) const throw() {
    return p % M_;
  }
};

// funciones de utilidad

// imprime un booleano
ostream& operator<=(ostream& os, const bool b) {
  os << (b ? "true" : "false");
  return os;
}

// imprime una lista de T's
template <typename T>
ostream& operator<<(ostream& os, const list<T>& L) {
  typename list<T>::const_iterator it = L.begin();
  if (it == L.end()) { 
    os << "[]";  
  }
  else {
    T prev = *it;
    os << "[";
    ++it;
    while (it != L.end()) {
      os << prev << ",";
      prev = *it;
      ++it;
    }
    os << prev << "]";
  }
  return os;
}

// imprime una pagina
ostream& operator<< (ostream &os, const file::pagina &p) {
  for (nat i=0; i < p.size(); ++i) {
    os << p[i];
  }
  return os;
}

file::modo file_modo(const string& s) throw(error){
  file::modo m;
  if (s == "OPEN") m = file::OPEN;
  else if (s == "CREATE") m = file::CREATE;
  else throw error(gen_driver::WrongTypeArgs,gen_driver::nom_mod,
	      gen_driver::WrongTypeArgsMsg);
  return m;
}

// user init
void* user_init(gen_driver& dr) {
  string id = dr.args(1);
  string tid = dr.args(2);
  
  if (tid == "file") {
    switch(dr.nargs()){
     case 3:
      return static_cast<void*>(new file(dr.args(3)));
    case 4:{
      file::modo m = file_modo(dr.args(4));
      return static_cast<void*>(new file(dr.args(3), m));
    }
    case 5:{
      file::modo m = file_modo(dr.args(4));
      return static_cast<void*>(new file(dr.args(3), m,
                                         util::toint(dr.args(5))));
    }
    default:
      throw error(gen_driver::WrongNumArgs,gen_driver::nom_mod,
		  gen_driver::WrongNumArgsMsg);
    }
  }
  else if (tid == "cache_LRU") {
    if (dr.nargs()==3) {
      return static_cast<void*>(new cache_LRU(util::toint(dr.args(3)))); 
    }
  }
  else if (tid == "cache_FIFO") {
    if (dr.nargs()==3) {
      return static_cast<void*>(new cache_FIFO(util::toint(dr.args(3))));
    }
  }
  else if (tid == "cache_LFU") {
    if (dr.nargs()==3) {
      return static_cast<void*>(new cache_LFU(util::toint(dr.args(3))));
    }
  }
  else if (tid == "cfile<palabra>") {
    switch(dr.nargs()){
    case 3: {
      return static_cast<void*>(new cfile<palabra>(dr.args(3)));
    }
    case 4: {
      file::modo m = file_modo(dr.args(4));
      return static_cast<void*>(new cfile<palabra>(dr.args(3), m));
    }
    case 5: {
      file::modo m = file_modo(dr.args(4));
      cache* pC = dr.object<cache>(dr.args(5));
      return static_cast<void*>(new cfile<palabra>(dr.args(3), m, pC));
    }
    case 7: {
      file::modo m = file_modo(dr.args(4));
      cache* pC = dr.object<cache>(dr.args(5));
      return static_cast<void*>(new cfile<palabra>(dr.args(3), m, pC,
						   util::toint(dr.args(6)),
						   util::toint(dr.args(7))));
    }
    default:
      throw error(gen_driver::WrongNumArgs, gen_driver::nom_mod, 
		  gen_driver::WrongNumArgsMsg);
    }
  }
  else if (tid == "tasoc<palabra>") {
    switch(dr.nargs()){
    case 3:
      return static_cast<void*>(new tasoc<palabra>(dr.args(3)));
    
    case 4: {
      cache* pC = dr.object<cache>(dr.args(4));
      return static_cast<void*>(new tasoc<palabra>(dr.args(3), pC));
    }
    case 6: {
      cache* pC = dr.object<cache>(dr.args(4));
      return static_cast<void*>(new tasoc<palabra>(dr.args(3), pC, 
                                                  util::toint(dr.args(5)), 
                                                  util::toint(dr.args(6))));
    }
    default:
    throw error(gen_driver::WrongNumArgs,gen_driver::nom_mod,
		gen_driver::WrongNumArgsMsg);
    }
  }
  else if (tid == "tasoc<int>") {
    switch(dr.nargs()){
    case 3:
      return static_cast<void*>(new tasoc<int>(dr.args(3)));
    
    case 4: {
      cache* pC = dr.object<cache>(dr.args(4));
      return static_cast<void*>(new tasoc<int>(dr.args(3), pC));
    }
    case 6: {
      cache* pC = dr.object<cache>(dr.args(4));
      return static_cast<void*>(new tasoc<int>(dr.args(3), pC, 
                                                  util::toint(dr.args(5)), 
                                                  util::toint(dr.args(6))));
    }
    default:
    throw error(gen_driver::WrongNumArgs,gen_driver::nom_mod,
		gen_driver::WrongNumArgsMsg);
    }
  }
  
  throw error(gen_driver::WrongTypeArgs,gen_driver::nom_mod,
	      gen_driver::WrongTypeArgsMsg);
}

/* TypeTraits */
template <> const char* TypeTraits<file::pagina>::name = "file::pagina";
template <> const char* TypeTraits<file>::name = "file";
template <> const char* TypeTraits<cache_LRU>::name = "cache_LRU";
template <> const char* TypeTraits<cache_FIFO>::name = "cache_FIFO";
template <> const char* TypeTraits<cache_LFU>::name = "cache_LFU";
template <> const char* TypeTraits<cfile<palabra> >::name = "cfile<palabra>";
template <> const char* TypeTraits<tasoc<palabra> >::name = "tasoc<palabra>";
template <> const char* TypeTraits<tasoc<int> >::name = "tasoc<int>";


/** Metodos de cache
 **/
void cache_asocia_fichero(gen_driver &dr){
  cache *c = dr.object<cache>();
  c -> asocia_fichero(dr.object<file>(1));
}

void cache_size(gen_driver &dr){
  cache *c = dr.object<cache>();
  dr.get_ostream() <<  c -> size() << endl;
}

void cache_page_size(gen_driver &dr){
  cache *c = dr.object<cache>();
  dr.get_ostream() <<  c -> page_size() << endl;
}

void cache_hits(gen_driver &dr){
  cache *c = dr.object<cache>();
  dr.get_ostream() <<  c -> hits() << endl;
}

void cache_misses(gen_driver &dr){
  cache *c = dr.object<cache>();
  dr.get_ostream() <<  c -> misses() << endl;
}

void cache_info(gen_driver &dr){
  cache *c = dr.object<cache>();
  dr.get_ostream() << "size : " <<  c -> size() << endl;
  dr.get_ostream() << "page_size : " <<  c -> page_size() << endl;
  dr.get_ostream() << "hits : " <<  c -> hits() << endl;
  dr.get_ostream() << "misses : " << c -> misses() << endl;
  dr.get_ostream() << "print : ";
  c -> print(dr.get_ostream());
  dr.get_ostream() << endl;
}

void cache_clone(gen_driver &dr){
  cache *c = dr.object<cache>() -> clone();
  dr.get_ostream() << "size : " <<  c -> size() << endl;
  dr.get_ostream() << "page_size : " <<  c -> page_size() << endl;
  dr.get_ostream() << "hits : " <<  c -> hits() << endl;
  dr.get_ostream() << "misses : " << c -> misses() << endl;
  dr.get_ostream() << "print : ";
  c -> print(dr.get_ostream());
  dr.get_ostream() << endl;
  delete c;
}

void cache_flush(gen_driver &dr){
  dr.object<cache>() -> flush();
}

void cache_print(gen_driver& dr) {
  dr.object<cache>() -> print(dr.get_ostream());
  dr.get_ostream() << endl;
}

void cache_get_read(gen_driver &dr){
  nat i = util::toint(dr.args(1));

  file::pagina p = dr.object<cache>() -> get_read(i);
  dr.get_ostream() << p << endl;
}

void cache_get_write(gen_driver &dr){
  nat i = util::toint(dr.args(1));
  // sempre es canvia la pagina agafada de la cache per aquesta
  nat ps = dr.object<cache>() -> page_size();
  file::pagina p(ps);
  for (nat i=0; i < ps; ++i) {
    p[i] = 'z';
  }
  dr.object<cache>() -> get_write(i) = p;
  // els canvis es comproven a posteriori amb 
  //   % cmp ...
}

// test de memoria de cache_LRU

void crea_vacia_cache_LRU(cache_LRU*& pc) {
  pc = new cache_LRU(3);
}

void crea_una_cache_LRU(cache_LRU*& pc) {
  string nfile = "12k.dat";
  file* pf = NULL; pc = NULL;
  try {
    pf = new file(nfile, file::OPEN, 1024);
    pc = new cache_LRU(3);
    pc -> asocia_fichero(pf);
    pc -> get_write(1);
    pc -> get_write(2);
    pc -> get_write(3);
    pc -> get_write(4);
    pc -> get_write(1);
    pc -> get_write(2);
    pc -> asocia_fichero(NULL);
    delete pf;
  } catch (error) {
    if (pc)
      pc -> asocia_fichero(NULL);
    delete pf;
    throw;
  }
}

void crea_otra_cache_LRU(cache_LRU*& pc) {
  string nfile = "12k.dat";
  file* pf = NULL; pc = NULL;
  try {
    pf = new file(nfile, file::OPEN, 1024);
    pc = new cache_LRU(3);
    pc -> asocia_fichero(pf);
    pc -> get_write(1);
    pc -> get_write(2);
    pc -> get_write(3);
    pc -> get_write(1);
    pc -> get_write(2);
    pc -> get_write(3);
    pc -> get_write(1);
    pc -> get_write(2);
    pc -> asocia_fichero(NULL);
    delete pf;
  } catch (error) {
    if (pc)
      pc -> asocia_fichero(NULL);
    delete pf;
    throw;
  }
}

bool test_ctor_cache_LRU(gen_driver &dr) {
  return dr.generic_memtest<cache_LRU>(crea_vacia_cache_LRU,
					    "ctor cache_LRU vacia");
}

bool test_get_write_cache_LRU(gen_driver &dr) {
  return dr.generic_memtest<cache_LRU>(crea_una_cache_LRU, 
					    "get_write");
}

bool test_copyctor_cache_LRU(gen_driver &dr) {
  return dr.copyctor_memtest<cache_LRU>(crea_una_cache_LRU);
}

bool test_asg_cache_LRU(gen_driver &dr) {
  return (dr.assgn_memtest<cache_LRU>(crea_una_cache_LRU, crea_otra_cache_LRU) &&
       dr.assgn_memtest<cache_LRU>(crea_otra_cache_LRU, crea_una_cache_LRU));
}

// test de memoria de cache_FIFO

void crea_vacia_cache_FIFO(cache_FIFO*& pc) {
  pc = new cache_FIFO(3);
}

void crea_una_cache_FIFO(cache_FIFO*& pc) {
  string nfile = "12k.dat";
  file* pf = NULL; pc = NULL;
  try {
    pf = new file(nfile, file::OPEN, 1024);
    pc = new cache_FIFO(3);
    pc -> asocia_fichero(pf);
    pc -> get_write(1);
    pc -> get_write(2);
    pc -> get_write(3);
    pc -> get_write(4);
    pc -> get_write(1);
    pc -> get_write(2);
    pc -> asocia_fichero(NULL);
    delete pf;
  } catch (error) {
    if (pc)
      pc -> asocia_fichero(NULL);
    delete pf;
    throw;
  }
}

void crea_otra_cache_FIFO(cache_FIFO*& pc) {
  string nfile = "12k.dat";
  file* pf = NULL; pc = NULL;
  try {
    pf = new file(nfile, file::OPEN, 1024);
    pc = new cache_FIFO(3);
    pc -> asocia_fichero(pf);
    pc -> get_write(1);
    pc -> get_write(2);
    pc -> get_write(3);
    pc -> get_write(1);
    pc -> get_write(2);
    pc -> get_write(3);
    pc -> get_write(1);
    pc -> get_write(2);
    pc -> asocia_fichero(NULL);
    delete pf;
  } catch (error) {
    if (pc)
      pc -> asocia_fichero(NULL);
    delete pf;
    throw;
  }
}

bool test_ctor_cache_FIFO(gen_driver &dr) {
  return dr.generic_memtest<cache_FIFO>(crea_vacia_cache_FIFO,
					    "ctor cache_FIFO vacia");
}

bool test_get_write_cache_FIFO(gen_driver &dr) {
  return dr.generic_memtest<cache_FIFO>(crea_una_cache_FIFO, 
					    "get_write");
}

bool test_copyctor_cache_FIFO(gen_driver &dr) {
  return dr.copyctor_memtest<cache_FIFO>(crea_una_cache_FIFO);
}

bool test_asg_cache_FIFO(gen_driver &dr) {
  return (dr.assgn_memtest<cache_FIFO>(crea_una_cache_FIFO, crea_otra_cache_FIFO) &&
       dr.assgn_memtest<cache_FIFO>(crea_otra_cache_FIFO, crea_una_cache_FIFO));
}

// test de memoria de cache_LFU

void crea_vacia_cache_LFU(cache_LFU*& pc) {
  pc = new cache_LFU(3);
}

void crea_una_cache_LFU(cache_LFU*& pc) {
  string nfile = "12k.dat";
  file* pf = NULL; pc = NULL;
  try {
    pf = new file(nfile, file::OPEN, 1024);
    pc = new cache_LFU(3);
    pc -> asocia_fichero(pf);
    pc -> get_write(1);
    pc -> get_write(2);
    pc -> get_write(3);
    pc -> get_write(4);
    pc -> get_write(1);
    pc -> get_write(2);
    pc -> asocia_fichero(NULL);
    delete pf;
  } catch (error) {
    if (pc)
      pc -> asocia_fichero(NULL);
    delete pf;
    throw;
  }
}

void crea_otra_cache_LFU(cache_LFU*& pc) {
  string nfile = "12k.dat";
  file* pf = NULL; pc = NULL;
  try {
    pf = new file(nfile, file::OPEN, 1024);
    pc = new cache_LFU(3);
    pc -> asocia_fichero(pf);
    pc -> get_write(1);
    pc -> get_write(2);
    pc -> get_write(3);
    pc -> get_write(1);
    pc -> get_write(2);
    pc -> get_write(3);
    pc -> get_write(1);
    pc -> get_write(2);
    pc -> asocia_fichero(NULL);
    delete pf;
  } catch (error) {
    if (pc)
      pc -> asocia_fichero(NULL);
    delete pf;
    throw;
  }
}

bool test_ctor_cache_LFU(gen_driver &dr) {
  return dr.generic_memtest<cache_LFU>(crea_vacia_cache_LFU,
					    "ctor cache_LFU vacia");
}

bool test_get_write_cache_LFU(gen_driver &dr) {
  return dr.generic_memtest<cache_LFU>(crea_una_cache_LFU, 
					    "get_write");
}

bool test_copyctor_cache_LFU(gen_driver &dr) {
  return dr.copyctor_memtest<cache_LFU>(crea_una_cache_LFU);
}

bool test_asg_cache_LFU(gen_driver &dr) {
  return (dr.assgn_memtest<cache_LFU>(crea_una_cache_LFU, crea_otra_cache_LFU) &&
       dr.assgn_memtest<cache_LFU>(crea_otra_cache_LFU, crea_una_cache_LFU));
}

/** Metodos cfile<palabra>
 **/
void cfile_read(gen_driver &dr) {
  cfile<palabra> *cf = dr.object<cfile<palabra> >();
  nat i = util::toint(dr.args(1));
  palabra pal;
// stringtopal(dr.args(2), pal);
  cf -> read(i, pal); 
  dr.get_ostream() << pal << endl;
}

void cfile_write(gen_driver &dr) {
  cfile<palabra> *cf = dr.object<cfile<palabra> >();
  nat i = util::toint(dr.args(1));
// palabra pal;
//  stringtopal(dr.args(2), pal);
  cf -> write(i, palabra(dr.args(2)));
}

void cfile_size(gen_driver &dr) {
  cfile<palabra> *cf = dr.object<cfile<palabra> >();
  dr.get_ostream() << cf -> size() << endl;
}

/** Metodos tasoc<palabra>
 **/
void tasoc_inserta(gen_driver &dr) {
  if (dr.object_type() == "tasoc<palabra>") {
    tasoc<palabra> *t = dr.object<tasoc<palabra> >();
    t -> inserta(palabra(dr.args(1)));
  } else if (dr.object_type() == "tasoc<int>") {
    tasoc<int>* t = dr.object<tasoc<int> >();
    t -> inserta(util::toint(dr.args(1)));
  } else 
    throw error(gen_driver::WrongTypeArgs,gen_driver::nom_mod,
		gen_driver::WrongTypeArgsMsg);
}

void tasoc_contiene(gen_driver &dr) {
  if (dr.object_type() == "tasoc<palabra>") {
    tasoc<palabra> *t = dr.object<tasoc<palabra> >();
    dr.get_ostream() <= t -> contiene(palabra(dr.args(1)));
    dr.get_ostream() << endl;
  } else if (dr.object_type() == "tasoc<int>") {
    tasoc<int>* t = dr.object<tasoc<int> >();
    dr.get_ostream() <= t -> contiene(util::toint(dr.args(1)));
    dr.get_ostream() << endl;
  } else 
    throw error(gen_driver::WrongTypeArgs,gen_driver::nom_mod,
		gen_driver::WrongTypeArgsMsg);
}

void tasoc_size(gen_driver &dr) {
  if (dr.object_type() == "tasoc<palabra>") {
    tasoc<palabra> *t = dr.object<tasoc<palabra> >();
    dr.get_ostream() << t -> size() << endl;
  } else if (dr.object_type() == "tasoc<int>") {
    tasoc<int>* t = dr.object<tasoc<int> >();
    dr.get_ostream() << t -> size() << endl;
  } else 
    throw error(gen_driver::WrongTypeArgs,gen_driver::nom_mod,
		gen_driver::WrongTypeArgsMsg);
}

void tasoc_print(gen_driver &dr) {
  if (dr.object_type() == "tasoc<palabra>") {
    tasoc<palabra> *t = dr.object<tasoc<palabra> >();
    t -> print(dr.get_ostream());
    dr.get_ostream() << endl;
  } else if (dr.object_type() == "tasoc<int>") {
    tasoc<int>* t = dr.object<tasoc<int> >();
    t -> print(dr.get_ostream());
    dr.get_ostream() << endl;
  }
  else 
    throw error(gen_driver::WrongTypeArgs,gen_driver::nom_mod,
		gen_driver::WrongTypeArgsMsg);
}

/*------------------------------< MAIN >-----------------------*/

int main(){  

  mem_din::switch_off();

  gen_driver d("cache.err", false);
    
  //cache
  d.add_call("cache_asocia_fichero", cache_asocia_fichero, "any", "file");
  d.add_call("asociac", cache_asocia_fichero, "any", "file");
  d.add_call("cache_size", cache_size, "any");
  d.add_call("sizec", cache_size, "any");
  d.add_call("cache_page_size", cache_page_size, "any");
  d.add_call("psizec", cache_page_size, "any");
  d.add_call("cache_hits", cache_hits, "any");
  d.add_call("hitsc", cache_hits, "any");
  d.add_call("cache_misses", cache_misses, "any");
  d.add_call("missesc", cache_misses, "any");
  d.add_call("infoc", cache_info, "any");

  d.add_call("cache_clone", cache_clone, "any");
  d.add_call("clonec", cache_clone, "any");
  d.add_call("cache_flush", cache_flush, "any");
  d.add_call("flushc", cache_flush, "any");
  d.add_call("cache_get_read", cache_get_read, "any", "nat");
  d.add_call("get_read_c", cache_get_read, "any", "nat");
  d.add_call("cache_get_write", cache_get_write, "any", "nat");
  d.add_call("get_write_c", cache_get_write, "any", "nat");
  d.add_call("cache_print", cache_print, "any");
  d.add_call("printc", cache_print, "any");


  //cfile<palabra>
  d.add_call("cfile_read", cfile_read, "cfile<palabra>", "nat");
  d.add_call("readcf", cfile_read, "cfile<palabra>", "nat");
  d.add_call("cfile_write", cfile_write, "cfile<palabra>", "nat string");
  d.add_call("writecf", cfile_write, "cfile<palabra>", "nat string");
  d.add_call("cfile_size", cfile_size, "cfile<palabra>");
  d.add_call("sizecf", cfile_size, "cfile<palabra>");


  // tasoc<...>
  d.add_call("tasoc_inserta", tasoc_inserta, "any", "any");
  d.add_call("inst", tasoc_inserta, "any", "any");
  d.add_call("tasoc_contiene", tasoc_contiene, "any", "any");
  d.add_call("contt", tasoc_contiene, "any", "any");
  d.add_call("tasoc_size", tasoc_size, "any");
  d.add_call("sizet", tasoc_size, "any");
  d.add_call("tasoc_print", tasoc_print, "any");
  d.add_call("printt", tasoc_print, "any");


  // instalacion de tipos
  d.install_std_type<file> ();
  d.install_std_type<file::pagina> ();
  d.install_type<cache_LRU>();
  d.install_type<cache_FIFO>();
  d.install_type<cache_LFU>();
  d.install_std_type<cfile<palabra> >();  
  d.install_std_type<tasoc<palabra> >();  
  d.install_std_type<tasoc<int> >();
 
 // test de memoria dinamica

  /*  
  d.add_memory_test<cache_LRU>(test_ctor_cache_LRU, "cache_LRU");
  d.add_memory_test<cache_LRU>(test_get_write_cache_LRU, "cache_LRU");
  d.add_memory_test<cache_LRU>(test_copyctor_cache_LRU, "cache_LRU");
  d.add_memory_test<cache_LRU>(test_asg_cache_LRU, "cache_LRU");

  d.add_memory_test<cache_FIFO>(test_ctor_cache_FIFO, "cache_FIFO");
  d.add_memory_test<cache_FIFO>(test_get_write_cache_FIFO, "cache_FIFO");
  d.add_memory_test<cache_FIFO>(test_copyctor_cache_FIFO, "cache_FIFO");
  d.add_memory_test<cache_FIFO>(test_asg_cache_FIFO, "cache_FIFO");
  
  d.add_memory_test<cache_LFU>(test_ctor_cache_LFU, "cache_LFU");
  d.add_memory_test<cache_LFU>(test_get_write_cache_LFU, "cache_LFU");
  d.add_memory_test<cache_LFU>(test_copyctor_cache_LFU, "cache_LFU");
  d.add_memory_test<cache_LFU>(test_asg_cache_LFU, "cache_LFU");
  */

  d.go();
}
