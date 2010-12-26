template <class T, template <class> class HashFunct = Hash>
tasoc<T, HashFunct<T> >::tasoc(const string& filename, cache* pC , nat M , nat Mt) throw(error)
{
	fileName = *filename;
	tabla = new int[Mt][2];
	estado = new nat[Mt];
	capacidadMaxima = Mt; 
}

template <class T, template <class> class HashFunct = Hash>
tasoc<T, Hash >::~tasoc() throw(){
	delete tabla;
	delelte estado;  
}

template <class T, template <class> class HashFunct = Hash>
void tasoc<T,Hash<T>>::inserta(const T& x) throw(error)
{
	if (!contiene(x))
	{
		if (size == Mt)
		{
			excepcion = new error();
			throw(excepcion);
		}
		else
		{
			int i = hash(x);
			if 
		}
	}	  
}

template <class T, template <class> class HashFunct = Hash>
bool tasoc<T,Hash>::contiene(const T& x) const throw() {
  
}

template <class T, template <class> class HashFunct = Hash>
nat tasoc<T,Hash>::size() const throw()
{
  
}

template <class T, template <class> class HashFunct = Hash>
void tasoc<T,Hash>::print(ostream& os) const throw()
{
  
}

template <class T, template <class> class HashFunct = Hash>
tasoc<T,Hash>::tasoc(const tasoc& c) throw() 
{

}

template <class T, template <class> class HashFunct = Hash>
tasoc&  tasoc<T,Hash>::operator=(const tasoc& c) throw() 
{
	return *this;
}



