template <class T>
	cfile<T>::cfile(const string& filename, file::modo m, cache* pC, nat num_pages, nat page_size) throw(error)
	{
		if (m == file::OPEN)
		{
			// Abrimos un archivo existente
			try
			{
				archivo = new file(filename, m, page_size);
				num_pages_ = archivo->num_pages();
				page_size_ = archivo->page_size();
				cacheImp = pC;
				size_ = 0;
			}
			catch (int e)
			{
				throw(e);
			}
		}
		else if (m == file::CREATE)
		{
			// Creamos un nuevo archivo
			try
			{
				archivo = new file(filename, m, page_size);
				num_pages_ = num_pages;
				page_size_ = page_size;
				cacheImp = pC;
				size_ = 0;
			}
			catch (int e)
			{
				throw e;
			}
		}
		// Comprobamos que podamos almacenar almenos un elemento de tipo T
		if (page_size < sizeof(T))
		{
			throw(TamanyoPaginaInsuf);
		}
		// Comprobamos si la cache es nula
		if (cacheImp == NULL)
		{
			nat paginas = 1;
			cacheImp = new cache_FIFO(paginas);
			cacheImp->asocia_fichero(archivo);
		}
	}

template <class T>
	cfile<T>::~cfile() throw()
	{
		
	}

template <class T>
	void cfile<T>::read(nat i, T& x) const throw(error)
	{
		if (i >= size_)
		{
			throw(IndiceFueraRango);
		}
		else
		{
			nat pagina = i/page_size_;
			nat offset = i-(pagina*page_size_); 
			file::pagina* p = new file::pagina(page_size_);
			read_bytes(*p, offset, x);
		}
	}

template <class T>
	void cfile<T>::write(nat i, const T& x) throw(error)
	{
		
	}

template <class T>
	nat cfile<T>::size() const throw()
	{
		  return size_;
	}
