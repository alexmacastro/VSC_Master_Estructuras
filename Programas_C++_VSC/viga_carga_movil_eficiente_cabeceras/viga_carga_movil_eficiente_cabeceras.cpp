/*

Licencia CC BY-NC 4.0 Creative Commons

    Código viga_carga_movil_eficiente_cabeceras
    Autor: Alejandro E. Martínez Castro
    email: amcastro@ugr.es
    Dep. Mecánica de Estructuras e Ingeniería Hidráulica. Universidad de Granada.

    Descripción: Solución analítica al problema de carga móvil en una viga de Bernoulli-Euler, biapoyada.
                 Basada en análisis modal, con solución analítica tanto en la parte espacial como temporal.

    Características particulares:

                 - Descomposición en varios ficheros .cpp
                 - Uso de cabeceras (ficheros .h).


    Curso: Vibraciones de Sistemas Continuos. Master de Estructuras.

    Objetivos: Observar cómo se aplica el recurso de almacenamiento en memoria para evitar operaciones de punto flotante.

               Observar cómo se reescriben las operacines para minimizar el número de operaciones de punto flotante por instante de tiempo.

               Se emplean arrays. Las arrays en C++ tienen diferencias respecto a FORTRAN.

               Se recomienda el tutorial
               http://www.cplusplus.com/doc/tutorial/

               Especialmente los aspectos de punteros y arrays.

----------------------------------------------
ESTRUCTURA DEL CÓDIGO
---------------------------------------------
    Se lee en el fichero datosviga.txt los siguientes parámetros ordenados por líneas

    Parámetros de la viga:
    =========================
    lv = longitud de la viga (m)
    ev = módulo de elasticidad (N/m^2)
    iv = momento de inercia de la sección transversal de la viga (m^4)
    mv = masa/unidad de longitud (kg/m)
    zi = tasa de amortiguamiento modal constante
    nd = número de puntos internos en los que se divide la viga para mostrar las formas modales.

    Parámetros de la carga móvil:
    =============================
    fv = fuerza (positiva en dirección negativa) en N
    ct = velocidad de paso

    Parámetros del análisis modal:
    ==============================
    nm = número de modos de vibración.
    te = paso de tiempo de evaluación.

*/

#include <iostream>  //Permite la entrada y salida por pantalla básica
#include <fstream>   //Permite la entrada y salida a fichero
#include <iomanip>   //Permite alterar la forma de escribir y leer, tanto por pantalla como por fichero.
#include <cmath>     //Permite operaciones matemáticas.
#include <string>    //Permite usar operaciones avanzadas con cadenas de caracteres
#include <new>       //Incluye el operador new[] necesario para inicializar de forma dinámica las matrices


#include "./include/Viga.h"
#include "./include/funciones_viga.h"

using namespace std; //Carga el espacio de nombres de la libería estándard std





 int main(){

// Lectura de datos a fichero

   Viga viga;
   viga.iniciar_vigaf();

 //  datos_pantalla (viga);

// Lectura de parámetros del análisis. El fichero "datosviga" tiene un espacio en blanco identificador.
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
