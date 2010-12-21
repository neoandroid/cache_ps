#include <iostream>
#include "cache.hpp"

void escribe(nat i, file::pagina& p){
     write(i,p);     
     
}


void lee(nat i, file::pagina& p){
     read(i,p);     
     
}
