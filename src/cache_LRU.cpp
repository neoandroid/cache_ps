#include <ps/error>
#include <ps/util>

#include <iostream>
#include <string>
#include "cache.hpp"


cache_FIFO(nat k) throw(error){
        size_ = k;	
        size = 0;
        prim = NULL;
        ult = NULL;
        file::pagina v[size_];        
}

nodo copia_nodo(nodo * p){
     nodo * aux = new nodo;
     aux->indice_real = p->indice_real;
     aux->indice_v = p->indice_v;
     aux->escrito = p->escrito;
     return aux;
}

cache_FIFO(const cache_FIFO& c) throw(error){   //constructora por copia
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
cache_FIFO& operator=(const cache_FIFO& c) throw(error){       //operador de asgnación, hago lo mismo que en de copia pero devolvemos referencia
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
	return *this
}

~cache_FIFO() throw(){
}

cache* clone() const throw(error){
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
			put_bytes(indice_real,0,v[aux->indice_v]);
			aux = aux->sig;
		}
		if(ult != NULL)put_bytes(indice_real,0,v[aux->indice_v]);
	}
	v = file::pagina v[];
	borrar_cola(prim);
	prim = NULL;
	ult = NULL;
	size = 0;
}

//operacion auxiliar
bool esta_en_cache(file::pagina x, int &ref){
     for(int i = 0; i < v.size(); ++i){
             if(v[i]==x){
                         ref = i;
                         return true;
             }
     }
     return false;
}

file::pagina get_read(nat i) throw(error){
             file::pagina x;
             int j;
             read_bytes(i,0,x);         //error si no hay fichero aosciado
             if(esta_en_cache(x,j)){
                                  hits_++;
                                  nodo * p = prim;
                                  bool b = true;
		                          while(p != NULL && b){  //error si no se encuentra????
			                                            if(p->indice_v == i) b = false;;
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
                     nodo* aux= new nodo;
                     misses_++;  
                     aux-> indice_real = i;
                     aux-> escrito = true;
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
                                     if(ult->escrito) put_bytes(indice_real,0,v[aux->indice_v]);
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
}

file::pagina& get_write(nat i) throw(error){
             int referencia;  
             file::pagina x;
             read_bytes(i,0,x);
             if(esta_en_cache(x,referencia)){
                                  hits_++;
                                  nodo * p = prim;
                                  bool b = true;
		                          while(p != NULL && b){  //error si no se encuentra????
			                                            if(p->indice_v == i) b = false;;
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
               misses_++;
		       nodo* aux= new nodo;
		       aux-> indice_real = i;
		       aux-> escrito = false;
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
					  if(ult->escrito) put_bytes(indice_real,0,v[aux->indice_v]);
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
     return *v[referencia];
}
}

void print(ostream& os) const throw (){
     nodo * aux = prim;
     while(aux->sig!=NULL){
               os << aux->indice_real;
               aux = aux->sig;
     }
     if(ult != NULL)os << ult->indice_real;
}
