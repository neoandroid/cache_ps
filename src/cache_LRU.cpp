

#include <iostream>
#include "cache.hpp"


cache_LRU::cache_LRU(nat k) throw(error){
}

cache_LRU::cache_LRU(const cache_LRU& c) throw(error){
}

cache_LRU& cache_LRU::operator=(const cache_LRU& c) throw(error){
}

cache* cache_LRU::clone() const throw(error){
}

void cache_LRU::flush() throw(error){
}

pagina cache_LRU::get_read(nat i) throw(error){
}

pagina& cache_LRU::get_write(nat i) throw(error){
}

void cache_LRU::print(ostream& os) const throw (){
}
