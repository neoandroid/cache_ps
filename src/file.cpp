#include <ps/error>
#include <ps/util>

#include "file.hpp"
#include <iostream>

using namespace std;
using util::nat;
using util::byte;

const nat OFFSET = sizeof(nat);

//////////////////////////////////////////////////////////////
// file


// Constructora
// Coste: O(1)

file::file(const string& filename, modo m, nat page_size) throw (error) {
  switch (m) {
  case OPEN : {
    // comprobamos que el fichero existe intentando abrirlo para lectura
    ifstream aux_in(filename.c_str());
    if (!aux_in) throw error(FicheroNoExiste);

    // leemos el tamaño de las páginas (page_size_), es un natural 
    // almacenado en los 
    // primeros OFFSET bytes del fichero
    aux_in.read(reinterpret_cast<char*>(&page_size_), sizeof(nat));

    // situandonos al final del fichero y descontando los primeros
    // OFFSET bytes averiguamos el numero de paginas en el fichero
    aux_in.seekg(0,ios::end);
    num_pages_ = (static_cast<nat>(aux_in.tellg()) - OFFSET) / page_size_;

    break;
  }
  case CREATE: {
    // comprobamos que el fichero no existe intentando abrirlo para lectura
    ifstream aux_in(filename.c_str());   
    if (aux_in) throw error(FicheroYaExiste); 

    // lo reabrimos para escrituras
    ofstream aux_out(filename.c_str());   

    // si el tamaño de las paginas es 0, lanzamos error
    if (page_size == 0)
      throw error(TamanyoPaginaNulo);

    // inicializamos los atributos del file
    page_size_ = page_size;
    num_pages_ = 0;
  
    // y escribimos page_size_ al inicio del fichero de acceso directo
    aux_out.write(reinterpret_cast<char*>(&page_size_), sizeof(nat));
  }
  }

  // ahora ya abrimos el fichero para escrituras y lecturas; si no se puede
  // lanzamos un error
  fd_.open(filename.c_str(),fstream::in|fstream::out);
  if (!fd_) throw error(ErrorApertura);
}
	
	
// Destructora: cierra el fichero
// Coste: O(1)
// Comentario: En realidad no ha de hacer nada, las destructoras 
// de los atributos hacen todo el trabajo
file::~file() throw () {

}
		
// Read: lee la pagina i-esima del file
// Coste: O(1) (respecto al numero de paginas) = O(page_size)
void file::read(nat i, pagina& p) throw(error) {
  if (i >= num_pages_) throw error(IndiceFueraRango);

  // lee page_size_ bytes en un buffer; la pagina i arranca en el byte
  // page_size_ * i + OFFSET
  char* buffer = new char[page_size_];
  fd_.seekp(sizeof(char) * page_size_ * i + OFFSET,ios::beg);
  fd_.read(buffer, page_size_);
  
  // traspasa los contenidos del buffer a la pagina
  for (nat i = 0; i < page_size_; ++i)
    p[i] = static_cast<byte>(buffer[i]);
  delete[] buffer;
}

// Write: escribe la pagina i-esima del file
// Coste: O(1) (respecto al numero de paginas) = O(page_szie)
void file::write(nat i, const pagina& p) throw (error) {
  if (i > num_pages_) throw error(IndiceFueraRango);
  // si i == num_pages_ tenemos una pagina mas, al final
  if (i == num_pages_) ++num_pages_;

  // nos situamos el byte page_size_ * i + OFFSET
  fd_.seekp(sizeof(char) * page_size_ * i + OFFSET, ios::beg);

  // rellenamos un buffer con los contenidos de la pagina
  char* buffer = new char[page_size_];
  for (nat i = 0; i < page_size_; ++i)
    buffer[i] = static_cast<char>(p[i]);

  // y escribimos el buffer en el fichero de acceso directo
  fd_.write(buffer, page_size_);

  delete[] buffer;
}
	
// Num_pages: retorna el numero de paginas en el fichero
// Coste: O(1)
nat file::num_pages() const throw () {
  return num_pages_;
}

// Page_size: retorna el tamaño de las paginas en bytes
// Coste: O(1)
nat file::page_size() const throw() {
  return page_size_;
}

///////////////////////////////////////////////////////////////////
// file::pagina

// Constructora
// Coste: O(page_size)
file::pagina::pagina(nat page_size, byte fill) throw(error) : 
  cont_(new byte[page_size]), page_size_(page_size) {
  for (nat i = 0; i < page_size_; ++i) 
    cont_[i] = fill;
}

// Swap: operacion privada que intercambia los atributos de p con los de
// *this
// Coste: O(1)
void file::pagina::swap(pagina& p) {
  byte* aux = p.cont_; p.cont_ = cont_; cont_ = aux;
  nat auxs = p.page_size_; p.page_size_ = page_size_; page_size_ = auxs;
}

// Constructora por copia
// Coste: O(p.page_size_)
file::pagina::pagina(const pagina& p) throw(error) : 
  cont_(new byte[p.page_size_]), page_size_(p.page_size_) {
  for (nat i = 0; i < p.page_size_; ++i) 
    cont_[i] = p.cont_[i];
}

// Destructora
// Coste: O(page_size_)
file::pagina::~pagina() throw() {
  delete[] cont_;
}

// Asignacion
// Coste: O(max{page_size_, p.page_size_})
// Comentario: crea una pagina auxiliar que es una copia de p, e intercambia
// los atributos de aux con los de *this; al destruir el objeto local aux, se
// destruyen los antiguos contenidos de *this
file::pagina& file::pagina::operator=(const pagina& p) throw(error) {
  pagina aux(p);
  swap(aux);
  return *this;
}

// Size: retorna el tamaño de la pagina en bytes
// Coste: O(1)
nat file::pagina::size() const throw() {
  return page_size_;
}

// Operator[] const: retorna una copia del byte i-esimo
// Coste: O(1)
byte file::pagina::operator[](nat i) const throw(error) {
  if (i >= page_size_) throw(IndiceFueraRango);
  return cont_[i];
}

// Operator[]: retorna una referencia al byte i-esimo
// Coste: O(1)
byte& file::pagina::operator[](nat i) throw(error) {
  if (i >= page_size_) throw(IndiceFueraRango);
  return cont_[i];
}

