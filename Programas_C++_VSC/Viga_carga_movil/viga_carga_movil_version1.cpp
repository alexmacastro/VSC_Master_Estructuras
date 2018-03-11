/*
    C�digo viga_carga_movil
    Autor: Alejandro E. Mart�nez Castro
    email: amcastro@ugr.es
    Dep. Mec�nica de Estructuras e Ingenier�a Hidr�ulica. Universidad de Granada.

    Descripci�n: Soluci�n anal�tica al problema de carga m�vil en una viga de Bernoulli-Euler, biapoyada.
                 Basada en an�lisis modal, con soluci�n anal�tica tanto en la parte espacial como temporal.

    Curso: Vibraciones de Sistemas Continuos. Master de Estructuras.

    Objetivos: Implementar la soluci�n de la viga de Bernoulli-Euler sometida a carga m�vil aislada,
               en un entorno basado en Software Libre. El lenguaje de programaci�n elegido ha sido C++.
               Este lenguaje ofrece numerosas ventajas y es recomendable su aprendizaje para alumnos
               que deseen ampliar sus conocimientos en programaci�n.

               Se recomienda el siguiente tutorial:

               http://www.cplusplus.com/doc/tutorial/
*/

   /* Se lee en el fichero datosviga.txt los siguientes par�metros ordenados por l�neas

   Par�metros de la viga:
   =========================
   lv = longitud de la viga (m)
   ev = m�dulo de elasticidad (N/m^2)
   iv = momento de inercia de la secci�n transversal de la viga (m^4)
   mv = masa/unidad de longitud (kg/m)
   zi = tasa de amortiguamiento modal constante
   nd = n�mero de puntos internos en los que se divide la viga para mostrar las formas modales.

   Par�metros de la carga m�vil:
   =============================
   fv = fuerza (positiva en direcci�n negativa) en N
   ct = velocidad de paso

   Par�metros del an�lisis modal:
   ==============================
   nm = n�mero de modos de vibraci�n.
   te = paso de tiempo de evaluaci�n.

*/

#include <iostream>
#include <fstream>

using namespace std;

class Viga{
  private:
   float lv; //longitud de la viga
   float ev; //m�dulo el�stico de la viga
   float iv; //momento de inercia de la viga
   float mv; //masa / longitud
   float zi; //tasa de amortiguamiento modal constante (%).
   int nd; // Numero de divisiones
  public:
   Viga (float,float,float,float,float,int); //Constructor de la clase (c�mo iniciar los miembros privados)
   friend void datos_pantalla(const Viga&); // Funciones "amigas"; pueden leer datos privados de la clase Viga.
  };

// Constructor de la clase
  Viga::Viga (float l, float e, float i, float m, float z, int n){
  lv = l;
  ev = e;
  iv = i;
  mv = m;
  zi = z;
  nd = n;
  };
// Friendship. Funci�n datos_pantalla. Accede a los datos privados de la clase.
  void datos_pantalla(const Viga& viga){
  cout << "Longitud: " << viga.lv <<" (m)" << endl;
  cout << "Modulo de elasticidad: " << viga.ev << " (N/m^2)" << endl;
  cout << "Momento de inercia: " << viga.iv << " (m^4)"<< endl;
  cout << "Masa/longitud: " << viga.mv <<" (kg/m)" << endl;
  cout << "Tasa de amortiguamiento: " << viga.zi << "(%)" <<endl ;
  cout << "Divisiones en la viga: " << viga.nd << endl ;
  }




 int main(){

// Lectura de datos a fichero

  ifstream fichero;

  fichero.open ("datosviga.dat");
  float longitud, moduloe, inercia, masa, zeta;
  int ndiv;
  fichero >> longitud ;
  fichero >> moduloe ;
  fichero >> inercia ;
  fichero >> masa ;
  fichero >> zeta ;
  fichero >> ndiv ;

  Viga viga(longitud,moduloe,inercia,masa,zeta,ndiv);

// Se muestran datos por pantalla.

// Lectura de par�metros modales en el fichero
   int nmodos; // N�mero de modos
   fichero >> nmodos;

   cout << "Numero de modos " << nmodos;

// Lectura del paso de tiempo de evaluaci�n de la soluci�n




  datos_pantalla (viga);
//  lectura_otrosdatos();
//  datosfile(); // Escribe a fichero los resultados de modos y frecuencias
//  desplazamientos(); // Escribe a fichero la serie del desplazamiento en el centro y cuartos de vano.
//  aceleraciones(); // Escribe a fichero la serie temporal de las aceleraciones verticales en centro y cuarto de vano.



//  fichero.close();
  return 0;
}
