#include "error"
#include "util"

#include <iostream>
#include <string>
#include "cache.hpp"

cache_FIFO::cache_FIFO(nat k) throw(error){
        size_ = k;
        size = 0;
        prim = NULL;
        ult = NULL;
        vector<pagina> v();
}

nodo cache_FIFO::copia_nodo(nodo * p){
     nodo * aux = new nodo;
     aux->indice_real = p->indice_real;
     aux->indice_v = p->indice_v;
     aux->escrito = p->escrito;
     return aux;
}

cache_FIFO::cache_FIFO(const cache_FIFO& c) throw(error){   //constructora por copia
	for(int i = 0; i < v.size(); ++i) v[i] = c.v[i];
	size = c.size;
	nodo * p = c.prim;
	if(c.prim != NULL){
	        nodo aux = new nodo;
	        aux = copia_nodo(p);
	        aux-> ant = NULL;
	        prim = aux;
	        if(prim->sig == NULL) ult = aux;
	        while(p != c.ult){
                    nodo aux2 = new nodo;
                    aux2 = copia_nodo(p->sig);
                    aux->sig = aux2;
                    aux2->ant = aux;
                    aux = aux2;
                    p = p->sig;
                    if(p->sig = NULL) ult = aux2;
            }
         		
	}
}
cache_FIFO& cache_FIFO::operator=(const cache_FIFO& c) throw(error){       //operador de asgnaci�n, hago lo mismo que en de copia pero devolvemos referencia
    for(int i = 0; i < v.size(); ++i) v[i] = c.v[i];
	size = c.size;
	nodo * p = c.prim;
	if(c.prim != NULL){
	        nodo aux = new nodo;
	        aux = copia_nodo(p);
	        aux-> ant = NULL;
	        prim = aux;
	        if(prim->sig == NULL) ult = aux;
	        while(p != c.ult){
                    nodo aux2 = new nodo;
                    aux2 = copia_nodo(p->sig);
                    aux->sig = aux2;
                    aux2->ant = aux;
                    aux = aux2;
                    p = p->sig;
                    if(p->sig = NULL) ult = aux2;
            }
         		
	}
	return *this;
}

cache_FIFO::~cache_FIFO() throw(){
}

cache* cache_FIFO::clone() const throw(error){
       cache_FIFO c;
       c = this;
       return *c;
}

void borrar_cola(nodo * p){       //operacion auxiliar,borramos cola de prioridad
     nodo * aux = p->sig;
     while(aux!=NULL){
                      delete p;
                      p=aux;
                      aux=aux->sig;
     }
}

void flush() throw(error){
	if(pf_ != NULL){  //error si no hay fichero asociado
		nodo * aux = prim;
		while(aux->sig!=NULL){
			write(aux->indice_real,v[aux->indice_v]);
			aux = aux->sig;
		}
		if(ult != NULL)write(ult->indice_real,v[ult->indice_v]);
	}
	v = vector<pagina> v();
	borrar_cola(prim);
	prim = NULL;
	ult = NULL;
	size = 0;
}

//operacion auxiliar
bool esta_en_cache(pagina x){
     for(int i = 0; i < v.size(); ++i){
             if(v[i]==x)return true;
     }
     return false;
}
             

pagina get_read(nat i) throw(error){
             nodo* aux= new nodo;
             pagina x;
             pf_.read(i,x);         //error si no hay fichero aosciado
             if(!esta_en_cache(x)){     
		     aux-> indice_real = i;
                     aux-> escrito = true;
                     aux-> ant = NULL;
                     if(size > 0){ //cahe no completa
			     if(size < size_){
				     int n = 1+(ult->indice_v);
                                     v[n] = x;
                                     size++;
                                     aux-> indice_v = n;
                                     aux-> sig = prim;
                                     prim-> ant = aux;
                                     prim = aux;
			     }else{  //cache completa
				     if(ult->escrito) write(ult->indice_real,v[ult->indice_v]);
                                     v[ult->indice_v] = x;
                                     aux->indice_v = ult->indice_v;
                                     ult=ult->ant;
                                     delete ult->sig;
                                     aux->sig=prim;
                                     prim->ant = aux;
                                     prim=aux; 
			     }
                    }else{    //cache vac�a
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

pagina& get_write(nat i) throw(error){
             int referencia;  
             pagina x;
             pf_.read(i,x);
             if(!esta_en_cache(x)){
		       nodo* aux= new nodo;
		       aux-> indice_real = i;
		       aux-> escrito = false;
		       aux->ant = NULL;
		       if(size > 0){ //cahe no completa
				 if(size < size_){
					  int n = 1+(ult->indice_v);
					  v[n] = x;
					  referencia=n;
					  size++;
					  aux-> indice_v = n;
					  aux -> sig = prim;
					  prim-> ant = aux;
					  prim = aux;
				  }else{  //cache completa
					  if(ult->escrito) write(ult->indice_real,v[ult->indice_v]);
					  v[ult->indice_v] = x;
					  aux->indice_v = ult->indice_v;
					  ult=ult->ant;
					  referencia=aux->indice_v;
					  delete ult->sig;
					  aux->sig=prim;
					  prim->ant = aux;
					  prim=aux;                                                                   
				  }
			}else{    //cache vac�a
			      v[0] = x;
			      size++;
			      referencia = 0;
			      aux->indice_v = 0;
			      aux->sig = NULL;
			      prim = aux;
			      ult = aux;
			}
     }
     return *v[referencia];
}

void print(ostream& os) const throw (){
     nodo * aux = prim;
     while(aux->sig!=NULL){
               os << aux->indice_real;
               aux = aux->sig;
     }
     if(ult != NULL)os << ult->indice_real;
}
