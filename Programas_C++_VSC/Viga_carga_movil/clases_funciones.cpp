#include <iostream>
#include <fstream>
using namespace std;

/* Licencia CC BY-NC 4.0 Creative Commons */

 class Viga{
  private:
   float lv; //longitud de la viga
   float ev; //módulo elástico de la viga
   float iv; //momento de inercia de la viga
   float mv; //masa / longitud
   float zi; //tasa de amortiguamiento modal constante (%).
   int nd;
  public:
//   Viga (float,float,float,float,float,int); //Constructor (no definido)
   friend void datos_pantalla(const Viga&); //Funciones "amigas"; pueden leer datos privados de la clase Viga.
   friend void lectura_datos (Viga&);
  };

/*  Viga::Viga (float l, float e, float i, float m, float z, int n){
  lv = l;
  ev = e;
  iv = i;
  mv = m;
  zi = z;
  nd = n;
  };
*/
  void datos_pantalla(const Viga& viga){
  cout << "Longitud: " << viga.lv <<" (m)" << endl;
  cout << "Modulo de elasticidad: " << viga.ev << " (N/m^2)" << endl;
  cout << "Momento de inercia: " << viga.iv << " (m^4)"<< endl;
  cout << "Masa/longitud: " << viga.mv <<" (kg/m)" << endl;
  cout << "Tasa de amortiguamiento: " << viga.zi << "(%)" <<endl ;
  cout << "Divisiones en la viga: " << viga.nd << endl ;
  }

  void lectura_datos(Viga& viga){
  ifstream fichero;
  fichero.open ("datosviga.dat");
  fichero >> viga.lv ;
  fichero >> viga.ev ;
  fichero >> viga.iv ;
  fichero >> viga.mv ;
  fichero >> viga.zi ;
  fichero >> viga.nd ;
  fichero.close();
  }
