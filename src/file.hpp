
#ifndef _FILE_HPP
#define _FILE_HPP

#include <fstream>
#include <ps/error>
#include <ps/util>

using namespace std;
using util::nat;
using util::byte;

class file {
public: 

  class pagina {
  public:

    static const nat DEFAULT_PAGE_SIZE = 1024;

    explicit pagina(nat page_size = DEFAULT_PAGE_SIZE, 
                    byte x = 0) throw(error);

    pagina(const pagina &p) throw(error);
    ~pagina() throw();
    pagina& operator=(const pagina &p) throw(error);

    nat size() const throw();

    byte operator[](nat i) const throw(error);

    byte& operator[](nat i) throw(error);

  static const int IndiceFueraRango  = 16;

  private:
    #include "pagina.rep"
  };

  enum modo { OPEN, CREATE };

  explicit file(const string& filename, modo m = OPEN, 
                nat page_size = pagina::DEFAULT_PAGE_SIZE) throw (error);

  ~file() throw ();

  void read(nat i, pagina& p) throw(error);

  void write(nat i, const pagina& p) throw (error);

  nat num_pages() const throw ();
  nat page_size() const throw ();

  static const int FicheroNoExiste   = 11;
  static const int FicheroYaExiste   = 12;
  static const int ErrorApertura     = 13;
  static const int TamanyoPaginaNulo = 14;
  static const int IndiceFueraRango  = 15;

private:

  file(const file& f) throw () {};
  file& operator=(const file& f) throw () { return *this; };

  #include "file.rep"
};
#endif
