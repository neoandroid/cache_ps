#include <ps/error>
#include <ps/util>

#include <iostream>
#include <string>
#include "cache.hpp"


cache_LRU::cache_LRU(nat k) throw(error){
        size_ = k;
        size = 0;
	    prim = NULL;
	    ult = NULL;
	    file::pagina* v =new file::pagina[size_];        
}

cache_LRU::nodo* cache_LRU::copia_nodo(cache_LRU::nodo* p){
     nodo * aux = new nodo;
     aux->indice_real = p->indice_real;
     aux->indice_v = p->indice_v;
     aux->escrito = p->escrito;
     return aux;
}

cache_LRU::cache_LRU(const cache_LRU& c) throw(error){    //constructora por copia
	for(int i = 0; i < int(size_); ++i)
		v[i] = c.v[i];
	size = c.size;
	cache_LRU::nodo *p = c.prim;
	if(c.prim != NULL){
		cache_LRU::nodo *aux = new cache_LRU::nodo;
		aux = copia_nodo(p);
		aux-> ant = NULL;
		prim = aux;
		if(prim->sig == NULL)
			ult = aux;
		while(p != c.ult){
			cache_LRU::nodo *aux2 = new cache_LRU::nodo;
			aux2 = copia_nodo(p->sig);
			aux->sig = aux2;
			aux2->ant = aux;
			aux = aux2;
			p = p->sig;
			if(p->sig == NULL)
				ult = aux2;
		}
	}
}
cache_LRU& cache_LRU::operator=(const cache_LRU& c) throw(error){       //operador de asgnación, hago lo mismo que en de copia pero devolvemos referencia
    for(int i = 0; i < int(size_); ++i)
    	v[i] = c.v[i];
	size = c.size;
	cache_LRU::nodo* p = c.prim;
	if(c.prim != NULL)
	{
		cache_LRU::nodo* aux = new cache_LRU::nodo;
		aux = copia_nodo(p);
		aux-> ant = NULL;
		prim = aux;
		if(prim->sig == NULL)
			ult = aux;
		while(p != c.ult){
			cache_LRU::nodo* aux2 = new cache_LRU::nodo;
			aux2 = copia_nodo(p->sig);
			aux->sig = aux2;
			aux2->ant = aux;
			aux = aux2;
			p = p->sig;
			if(p->sig == NULL)
				ult = aux2;
		}
	}
	return *this;
}

cache_LRU::~cache_LRU() throw(){
}

cache* cache_LRU::clone() const throw(error){
       /*cache_LRU c;
       c = this;
       return c;*/
}

void cache_LRU::borrar_cola(nodo * p){       //operacion auxiliar,borramos cola de prioridad
	nodo * aux = p->sig;
	while(aux!=NULL){
		delete p;
		p=aux;
		aux=aux->sig;
	}
}

void cache_LRU::flush() throw(error){
	if(pf_ != NULL){  //error si no hay fichero asociado
		nodo * aux = prim;
		while(aux->sig!=NULL){
			if (aux->escrito == true)
			{
				pf_->write(aux->indice_real,v[aux->indice_v]);
			}
			aux = aux->sig;
		}
		if(ult != NULL)
		{
			if (aux->escrito == true)
				pf_->write(aux->indice_real,v[aux->indice_v]);
		}
	}
	delete v;
	file::pagina* v;
	v = new file::pagina[size_];
	borrar_cola(prim);
	prim = NULL;
	ult = NULL;
	size = 0;
}

//operacion auxiliar
bool cache_LRU::esta_en_cache(int i, int &ref){
        nodo* aux = prim;
		while(aux!=NULL){
			if (aux->indice_real == i){
               ref = aux->indice_v;
               return true;
            }
			aux = aux->sig;
		}
	    return false;
}

file::pagina cache_LRU::get_read(nat i) throw(error){
             file::pagina x;
             int j;
             if(esta_en_cache(i,j)){
                                  hits_++;
                                  x = v[j];
                                  nodo * p = prim;
                                  bool b = true;
		                          while(p != NULL && b){  //error si no se encuentra????
			                                            if(p->indice_v == i) b = false;
                                                        else p = p->sig;
                                  }
                                  nodo * t = p->ant;
                                  t->sig = p->sig;
                                  if(p-> sig != NULL)p->sig->ant = t;
                                  p->sig = prim;
                                  prim->ant = p;
                                  p->ant = NULL;
                                  prim = p;
             }
             else{   
                     pf_->read(i,x);
                     cache_LRU::nodo* aux= new cache_LRU::nodo;
                     misses_++;  
                     aux-> indice_real = i;
                     aux-> escrito = false;
                     aux-> ant = NULL;
                     if(size > 0){ //cache con elementos
                             if(size < size_){         //cache no completa
				                     int n = 1+(ult->indice_v);
                                     v[n] = x;
                                     size++;
                                     aux-> indice_v = n;
                                     aux-> sig = prim;
                                     prim-> ant = aux;
                                     prim = aux;
                              
                              }else{  //cache completa
                                     if(ult->escrito) pf_->write(aux->indice_real,v[aux->indice_v]);
                                     v[ult->indice_v] = x;
                                     aux->indice_v = ult->indice_v;
                                     ult=ult->ant;
                                     delete ult->sig;
                                     aux->sig=prim;
                                     prim->ant = aux;
                                     prim=aux; 
                          }
                    }else{    //cache vacia
			                  v[0] = x;
                            size++;
                            aux->indice_v = 0;
                            aux->sig = NULL;
                            prim = aux;
                            ult = aux;
                    }
             }
             return x;
}


file::pagina& cache_LRU::get_write(nat i) throw(error){
             int referencia;  
             file::pagina x;
             if(esta_en_cache(i,referencia)){
                                  hits_++;
                                  nodo * p = prim;
                                  bool b = true;
		                          while(p != NULL && b){  //error si no se encuentra????
			                                            if(p->indice_v == i) b = false;
                                                        else p = p->sig;
                                  }
                                  p->escrito = true;
                                  nodo * t = p->ant;
                                  t->sig = p->sig;
                                  if(p-> sig != NULL)p->sig->ant = t;
                                  p->sig = prim;
                                  prim->ant = p;
                                  p->ant = NULL;
                                  prim = p;
             }
             else{   
               pf_->read(i,x);
               misses_++;
		       cache_LRU::nodo* aux= new cache_LRU::nodo;
		       aux-> indice_real = i;
		       aux-> escrito = true;
		       aux->ant = NULL;
		       if(size > 0){         //cache con elementos
				 if(size < size_){   //cahe no completa
					  int n = 1+(ult->indice_v);
					  v[n] = x;
					  referencia=n;
					  size++;
					  aux-> indice_v = n;
					  aux -> sig = prim;
					  prim-> ant = aux;
					  prim = aux;
				  }else{  //cache completa
					  if(ult->escrito) pf_->write(aux->indice_real,v[aux->indice_v]);
					  v[ult->indice_v] = x;
					  aux->indice_v = ult->indice_v;
					  ult=ult->ant;
					  referencia=aux->indice_v;
					  delete ult->sig;
					  aux->sig=prim;
					  prim->ant = aux;
					  prim=aux;                                                                   
				  }
			}else{    //cache vacia
			      v[0] = x;
			      size++;
			      referencia = 0;
			      aux->indice_v = 0;
			      aux->sig = NULL;
			      prim = aux;
			      ult = aux;
			}
     }
     return v[referencia];
}


void cache_LRU::print(ostream& os) const throw (){
     nodo * aux = prim;
     while(aux->sig!=NULL){
               os << aux->indice_real;
               aux = aux->sig;
     }
     if(ult != NULL)os << ult->indice_real;
}
