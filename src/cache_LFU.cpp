#include <ps/error>
#include <ps/util>

#include <iostream>
#include <string>
#include "cache.hpp"


cache_LFU::cache_LFU(nat k) throw(error){
                         size_ = k;
                         size = 0;
	                      prim = NULL;
	                      ult = NULL;
}

cache_LFU::nodo* cache_LFU::copia_nodo(cache_LFU::nodo* p){
	cache_LFU::nodo *aux = new cache_LFU::nodo;
	aux->indice_real = p->indice_real;
	aux->escrito = p->escrito;
	aux->p = p->p;
	return aux;
}

cache_LFU::cache_LFU(const cache_LFU& c) throw(error){
   size = c.size;
	cache_LFU::nodo *p = c.prim;
	if(c.prim != NULL){
		cache_LFU::nodo *aux = new cache_LFU::nodo;
		aux = copia_nodo(p);
		aux-> ant = NULL;
		prim = aux;
		if(prim->sig == NULL)
			ult = aux;
		while(p != c.ult){
			cache_LFU::nodo *aux2 = new cache_LFU::nodo;
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

cache_LFU& cache_LFU::operator=(const cache_LFU& c) throw(error){
 size = c.size;
	cache_LFU::nodo *p = c.prim;
	if(c.prim != NULL){
		cache_LFU::nodo *aux = new cache_LFU::nodo;
		aux = copia_nodo(p);
		aux-> ant = NULL;
		prim = aux;
		if(prim->sig == NULL)
			ult = aux;
		while(p != c.ult){
			cache_LFU::nodo *aux2 = new cache_LFU::nodo;
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

cache_LFU::~cache_LFU() throw(){

}

cache* cache_LFU::clone() const throw(error){

}

void cache_LFU::flush() throw(error){
     if(pf_ != NULL){
            nodo * aux = prim;
            nodo * puntero = prim;
	              while(puntero!=NULL){
                      aux = puntero;
                      if(puntero->escrito) pf_->write(puntero->indice_real,puntero->p);
                      delete puntero;
		                puntero = aux->sig;
                  }
     }
}

void cache_LFU::meter_en_cache(cache_LFU::nodo* c, cache_LFU::nodo *l){
     if(l != NULL){
          cache_LFU::nodo* aux = l;
          while(aux != NULL){
              if(aux->frec >= c->frec) {
                  if(aux->ant != NULL){
                      aux->ant->sig = c;
                      prim = c;
                  }
                  if(aux->ant != NULL)c->ant = aux->ant;
                  else aux->ant = NULL;
                  c->sig = aux;
                  aux->ant = c;
              }
               aux = aux->sig;
          }
     }
}

void cache_LFU::eliminar_nodo(int i,cache_LFU::nodo* l){
     if(l != NULL){
          cache_LFU::nodo* aux = l;
          while(aux != NULL){
              if(aux->indice_real == i) {
                  if(aux->ant != NULL){
                      aux->ant->sig = aux->sig;
                  }
                  if(aux->sig != NULL)aux->sig->ant = aux->ant;
                  delete aux;
              }
          }
     }
}

bool cache_LFU::estaencache(int i,cache_LFU::nodo &n,cache_LFU::nodo* l){
     if(l != NULL){
          cache_LFU::nodo* aux = l;
          while(aux != NULL){
              if(aux->indice_real == i) {
                  n.frec = aux->frec;
                  n.indice_real = aux->indice_real;
                  n.p = aux->p;
                  n.escrito = aux->escrito;
                  if(aux->ant != NULL){
                      aux->ant->sig = aux->sig;
                  }
                  if(aux->sig != NULL)aux->sig->ant = aux->ant;
                  delete aux;
                  return true;
              }
              else aux = aux->sig;
          }
     }
     return false;
}

file::pagina cache_LFU::get_read(nat i) throw(error){
    file::pagina x;
    cache_LFU::nodo n;
    if(estaencache(i,n,prim)){  //se encarga mirar si 'i' esta en la cache y en caso afirmativo eliminarlo y devolver una copia del nodo en 'n'
        hits_++;
        n.frec++;
        x = n.p;
        meter_en_cache(&n,prim);
    }
    else{
        pf_->read(i,x);
		  cache_LFU::nodo* aux= new cache_LFU::nodo;
		  misses_++;
		  aux->frec = 0;
		  aux-> indice_real = i;
		  aux-> escrito = false;
		  aux->p = x;
		  if(size > 0){ //cache con elementos
            if(size < size_){         //cache no completa
                meter_en_cache(aux,prim);
            }
            else{                        //cache completa
                if(ult->escrito)
                    pf_->write(aux->indice_real,aux->p);
                eliminar_nodo(aux->indice_real,prim);   //eliminamos el nodo con indice aux->indice_real de la lista de prim
                meter_en_cache(aux,prim);
            }
        }
        else{   // cache vac�a
             aux->ant = NULL;
             aux->sig = NULL;
             prim = aux;
             ult = aux;
             size_++;
        }
    }
    return x;
}

file::pagina& cache_LFU::get_write(nat i) throw(error){
   file::pagina x;
   file::pagina* p;
   file::pagina * ret;
    cache_LFU::nodo n;
    if(estaencache(i,n,prim)){  //se encarga mirar si 'i' esta en la cache y en caso afirmativo eliminarlo y devolver una copia del nodo en 'n'
        hits_++;
        n.frec++;
        *p = n.p;
        ret = &n.p;
        n.escrito = true;
        meter_en_cache(&n,prim);
    }
    else{
        pf_->read(i,x);
		  cache_LFU::nodo* aux= new cache_LFU::nodo;
		  misses_++;
		  aux->frec = 0;
		  aux-> indice_real = i;
		  aux-> escrito = true;
		  aux->p = x;
		  ret = &aux->p;
		  *p = aux->p;
		  if(size > 0){ //cache con elementos
            if(size < size_){         //cache no completa
                meter_en_cache(aux,prim);
            }
            else{                        //cache completa
                if(ult->escrito)
                    pf_->write(aux->indice_real,aux->p);
                eliminar_nodo(aux->indice_real,prim);   //eliminamos el nodo con indice aux->indice_real de la lista de prim
                meter_en_cache(aux,prim);
            }
        }
        else{   // cache vacia
             aux->ant = NULL;
             aux->sig = NULL;
             prim = aux;
             ult = aux;
             size_++;
        }
    }
    return *ret;
}

void cache_LFU::print(ostream& os) const throw (){
          nodo * aux = prim;
	       while(aux->sig!=NULL){
               os << aux->indice_real;
		         aux = aux->sig;
        }
	     if(ult != NULL)
		  os << ult->indice_real;
}

