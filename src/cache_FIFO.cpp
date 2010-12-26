#include <ps/error>
#include <ps/util>

#include <iostream>
#include <string>
#include "cache.hpp"



cache_FIFO::cache_FIFO(util::nat k) throw(error){
	size_ = k;
	size = 0;
	prim = NULL;
	ult = NULL;
	file::pagina* v =new file::pagina[size_];
}

cache_FIFO::nodo* cache_FIFO::copia_nodo(cache_FIFO::nodo* p){
	cache_FIFO::nodo *aux = new cache_FIFO::nodo;
	aux->indice_real = p->indice_real;
	aux->indice_v = p->indice_v;
	aux->escrito = p->escrito;
	return aux;
}

cache_FIFO::cache_FIFO(const cache_FIFO& c) throw(error){   //constructora por copia
	for(int i = 0; i < int(size_); ++i)
		v[i] = c.v[i];
	size = c.size;
	cache_FIFO::nodo *p = c.prim;
	if(c.prim != NULL){
		cache_FIFO::nodo *aux = new cache_FIFO::nodo;
		aux = copia_nodo(p);
		aux-> ant = NULL;
		prim = aux;
		if(prim->sig == NULL)
			ult = aux;
		while(p != c.ult){
			cache_FIFO::nodo *aux2 = new cache_FIFO::nodo;
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

cache_FIFO& cache_FIFO::operator=(const cache_FIFO& c) throw(error){       //operador de asgnacion, hago lo mismo que en de copia pero devolvemos referencia
	for(int i = 0; i < int(size_); ++i)
    	v[i] = c.v[i];
	size = c.size;
	cache_FIFO::nodo* p = c.prim;
	if(c.prim != NULL)
	{
		cache_FIFO::nodo* aux = new cache_FIFO::nodo;
		aux = copia_nodo(p);
		aux-> ant = NULL;
		prim = aux;
		if(prim->sig == NULL)
			ult = aux;
		while(p != c.ult){
			cache_FIFO::nodo* aux2 = new cache_FIFO::nodo;
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

cache_FIFO::~cache_FIFO() throw(){
}

cache* cache_FIFO::clone() const throw(error){
	/*cache_FIFO c = new;
	c = *this;
	return &c;*/
}

void cache_FIFO::borrar_cola(nodo * p){       //operacion auxiliar,borramos cola de prioridad
	nodo * aux = p->sig;
	while(aux!=NULL){
		delete p;
		p=aux;
		aux=aux->sig;
	}
}


void cache_FIFO::flush() throw(error){
	if(pf_ != NULL){  //error si no hay fichero asociado
		nodo * aux = prim;
		while(aux->sig!=NULL){
			if (aux->escrito == true)
			{
               
				//put_bytes(aux->indice_real,0,v[aux->indice_v]);
                //file::pagina x;
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
bool cache_FIFO::esta_en_cache(int i, int &ref){
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
             

file::pagina cache_FIFO::get_read(nat i) throw(error){
	file::pagina x;
	int j;
	if(esta_en_cache(i,j)){
		hits_++;
		x = v[j];
    }
	else{
        pf_->read(i,x);
		cache_FIFO::nodo* aux= new cache_FIFO::nodo;
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
				if(ult->escrito)
					pf_->write(ult->indice_real,v[ult->indice_v]);
				aux->indice_v = ult->indice_v;
				v[aux->indice_v] = x;
				if(size > 1){
                    ult=ult->ant;
				        delete ult->sig;
				        aux->sig=prim;
				        prim->ant = aux;
                }else {
                      delete ult;
                      prim = aux;
                      ult = aux;
                      aux->sig = NULL;
                }
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

file::pagina& cache_FIFO::get_write(nat i) throw(error){
	int referencia;
	file::pagina x;
	if(esta_en_cache(i,referencia)) hits_++;   //esrito = true;
	else{
		misses_++;
		pf_->read(i,x);
		cache_FIFO::nodo* aux= new cache_FIFO::nodo;
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
				if(ult->escrito)
					pf_->write(aux->indice_real,v[aux->indice_v]);
				v[ult->indice_v] = x;
				aux->indice_v = ult->indice_v;
				referencia=aux->indice_v;
				if(size > 1){
                        ult=ult->ant;
				        delete ult->sig;
				        aux->sig=prim;
				        prim->ant = aux;
                }else {
                      delete ult;
                      prim = aux;
                      ult = aux;
                      aux->sig = NULL;
                }
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
	//file::pagina* p = &v[referencia];
	return v[referencia];
}

void cache_FIFO::print(ostream& os) const throw (){
	nodo * aux = prim;
	while(aux->sig!=NULL){
		os << aux->indice_real;
		aux = aux->sig;
	}
	if(ult != NULL)
		os << ult->indice_real;
}
