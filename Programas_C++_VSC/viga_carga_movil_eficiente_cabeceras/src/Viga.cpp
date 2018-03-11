#include "../include/Viga.h"
#include <fstream>
#include <iostream>

/* Licencia CC BY-NC 4.0 Creative Commons */

using namespace std;

void Viga::iniciar_vigaf (){
  ifstream fichero;
  fichero.open ("datosviga.dat");
  float l, e, i, m, z;
  int n;
  fichero >> l ;
  fichero >> e ;
  fichero >> i ;
  fichero >> m ;
  fichero >> z ;
  fichero >> n ;
  lv = l;
  ev = e;
  iv = i;
  mv = m;
  zi = z/100.;
  nd = n;
  fichero.close();
  }

