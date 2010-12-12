#ifndef _CACHE_HPP
#define _CACHE_HPP
#include <iostream>

#include <error>
#include <util>

#include "file.hpp"
using util::nat;


class cache {
public:

  virtual cache* clone() const throw(error) = 0;

  virtual ~cache() {};

  void asocia_fichero(file* f) throw() {
    flush();
    pf_ = f;
    if (pf_ != NULL)
	page_size_ = pf_ -> page_size();
    else
	page_size_ = 0;
  };

  virtual void flush() throw(error) = 0;

  virtual file::pagina get_read(nat i) throw(error) = 0;

  virtual file::pagina& get_write(nat i) throw(error) = 0;

  nat size() const throw() {
    return size_;
  };
  nat page_size() const throw() {
    return page_size_;
  };
  nat hits() const throw() {
    return hits_; 
  };
  nat misses() const throw() {
    return misses_; 
  };


  virtual void print(ostream& os) const throw () = 0;

  static const int CacheNoAsociadaFichero = 31;
  static const int TamanyoCacheNulo       = 32;

protected:
  file* pf_;
  nat size_;
  nat page_size_;
  nat hits_, misses_;
  #include "cache.rep"
};

class cache_LRU : public cache {
public:

  cache_LRU(nat k) throw(error);

  cache_LRU(const cache_LRU& c) throw(error);
  cache_LRU& operator=(const cache_LRU& c) throw(error);
  ~cache_LRU() throw();

  cache* clone() const throw(error);
  void flush() throw(error);
  file::pagina get_read(nat i) throw(error);
  file::pagina& get_write(nat i) throw(error);
  void print(ostream& os) const throw ();

private:
  #include "cache_LRU.rep"
};

class cache_FIFO : public cache {
public:

  cache_FIFO(nat k) throw(error);

  cache_FIFO(const cache_FIFO& c) throw(error);
  cache_FIFO& operator=(const cache_FIFO& c) throw(error);
  ~cache_FIFO() throw();

  cache* clone() const throw(error);
  void flush() throw(error);
  file::pagina get_read(nat i) throw(error);
  file::pagina& get_write(nat i) throw(error);
  void print(ostream& os) const throw ();

private:
  #include "cache_FIFO.rep"
};

class cache_LFU : public cache {
public:

  cache_LFU(nat k) throw(error);

  cache_LFU(const cache_LFU& c) throw(error);
  cache_LFU& operator=(const cache_LFU& c) throw(error);
  ~cache_LFU() throw();

  cache* clone() const throw(error);
  void flush() throw(error);
  file::pagina get_read(nat i) throw(error);
  file::pagina& get_write(nat i) throw(error);
  void print(ostream& os) const throw ();

private:
  #include "cache_LFU.rep"
};

#endif
