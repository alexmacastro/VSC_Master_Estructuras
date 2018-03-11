/*

Licencia CC BY-NC 4.0 Creative Commons

    C�digo viga_carga_movil_eficiente_cabeceras
    Autor: Alejandro E. Mart�nez Castro
    email: amcastro@ugr.es
    Dep. Mec�nica de Estructuras e Ingenier�a Hidr�ulica. Universidad de Granada.

    Descripci�n: Soluci�n anal�tica al problema de carga m�vil en una viga de Bernoulli-Euler, biapoyada.
                 Basada en an�lisis modal, con soluci�n anal�tica tanto en la parte espacial como temporal.

    Caracter�sticas particulares:

                 - Descomposici�n en varios ficheros .cpp
                 - Uso de cabeceras (ficheros .h).


    Curso: Vibraciones de Sistemas Continuos. Master de Estructuras.

    Objetivos: Observar c�mo se aplica el recurso de almacenamiento en memoria para evitar operaciones de punto flotante.

               Observar c�mo se reescriben las operacines para minimizar el n�mero de operaciones de punto flotante por instante de tiempo.

               Se emplean arrays. Las arrays en C++ tienen diferencias respecto a FORTRAN.

               Se recomienda el tutorial
               http://www.cplusplus.com/doc/tutorial/

               Especialmente los aspectos de punteros y arrays.

----------------------------------------------
ESTRUCTURA DEL C�DIGO
---------------------------------------------
    Se lee en el fichero datosviga.txt los siguientes par�metros ordenados por l�neas

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

#include <iostream>  //Permite la entrada y salida por pantalla b�sica
#include <fstream>   //Permite la entrada y salida a fichero
#include <iomanip>   //Permite alterar la forma de escribir y leer, tanto por pantalla como por fichero.
#include <cmath>     //Permite operaciones matem�ticas.
#include <string>    //Permite usar operaciones avanzadas con cadenas de caracteres
#include <new>       //Incluye el operador new[] necesario para inicializar de forma din�mica las matrices


#include "./include/Viga.h"
#include "./include/funciones_viga.h"

using namespace std; //Carga el espacio de nombres de la liber�a est�ndard std





 int main(){

// Lectura de datos a fichero

   Viga viga;
   viga.iniciar_vigaf();

 //  datos_pantalla (viga);

// Lectura de par�metros del an�lisis. El fichero "datosviga" tiene un espacio en blanco identificador.
   float fv, ct, te;
   int nm;
   leedatos (fv, ct, nm, te);

   cout << "Valor de Fuerza vertical: "<< fv << " (N)"<<endl;
   cout << "Valor de Velocidad: "<< ct << "( m/s)"<<endl;
   cout << "Valor de Numero de modos: "<< nm <<endl ;
   cout << "Valor de te: "<< te << " (s)"<<endl;




   datos_pantalla (viga);

   analisis ( viga, fv , ct , nm , te );

//  fichero.close();
  return 0;
}
