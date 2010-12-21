



	cfile(const string& filename, file::modo m = file::OPEN, cache* pC = NULL, nat num_pages = 0, nat page_size = file::pagina::DEFAULT_PAGE_SIZE) throw(error)
	{
		if (m == file::OPEN)
		{
			// Abrimos un archivo existente
			try
			{
				archivo = new file(filname, m);
				num_pages_ = archivo.num_pages();
				page_size_ = archivo.page_size();
				cacheImp = pC;
				size = 0;
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
				archivo = new file(filename, m, page_size)
				num_pages_ = num_pages;
				page_size_ = page_size;
				cacheImp = pC;
				size = 0;
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
			cacheImp.asocia_fichero(archivo);
		}
	}

	~cfile() throw()
	{
		
	}

	void read(nat i, T& x) const throw(error)
	{
		if (i >= size)
		{
			throw(IndiceFueraRango);
		}
		else
		{
			nat pagina = i/page_size;
			nat offset = i-(pagina*page_size); 
			fiel::pagina p = new file::pagina(page_size);
			read_bytes(&p, offset, x);
		}
	}

	void write(nat i, const T& x) throw(error)
	{
		
	}

	nat size() const throw()
	{
		  return size;
	}


