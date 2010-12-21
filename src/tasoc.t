template <class T> class Hash {
private:
  nat M_;
public:
  explicit Hash(nat M) throw();
  int operator()(const T& x) const throw();
};


tasoc(const string& filename, cache* pC = NULL, nat M = 1009, nat Mt = 1109) throw(error)
{
	fileName = fileName;
	tabla = new int[Mt][2];
	estado = new nat[Mt];
	capacidadMaxima = Mt; 
}

~tasoc() throw(){
	delete tabla;
	delelte estado;  
}

void inserta(const T& x) throw(error)
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

bool contiene(const T& x) const throw() {
  
}

nat size() const throw()
{
  
}

void print(ostream& os) const throw()
{
  
}

tasoc(const tasoc& c) throw() 
{

}

tasoc&  operator=(const tasoc& c) throw() 
{
	return *this;
}



