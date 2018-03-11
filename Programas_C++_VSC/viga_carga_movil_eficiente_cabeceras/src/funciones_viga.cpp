
#include <iostream>  //Permite la entrada y salida por pantalla básica
#include <fstream>   //Permite la entrada y salida a fichero
#include <iomanip>   //Permite alterar la forma de escribir y leer, tanto por pantalla como por fichero.
#include <cmath>     //Permite operaciones matemáticas.
#include <string>    //Permite usar operaciones avanzadas con cadenas de caracteres
#include <new>

#include "../include/Viga.h"
#include "../include/funciones_viga.h"

/* Licencia CC BY-NC 4.0 Creative Commons */

using namespace std;

  void datos_pantalla(const Viga &beam){ // Atención al ampersand (&) para el paso por referencia, y "const" para que no pueda alterarse su valor en la salida
  cout << "Longitud: " << beam.lv <<" (m)" << endl;
  cout << "Modulo de elasticidad: " << beam.ev << " (N/m^2)" << endl;
  cout << "Momento de inercia: " << beam.iv << " (m^4)"<< endl;
  cout << "Masa/longitud: " << beam.mv <<" (kg/m)" << endl;
  cout << "Tasa de amortiguamiento: " << beam.zi*100. << "(%)" <<endl ;
  cout << "Divisiones en la viga: " << beam.nd << endl ;
  }



  void leedatos(float &fv, float &ct, int & nm , float & te){

  ifstream fichero;

  fichero.open ("datosviga.dat", ios::in);

  while (!fichero.eof())
     {
     string line;
     getline(fichero,line);

     if (line.length() == 0 ){
     fichero >> fv ; //Valor de la fuerza puntual
     fichero >> ct; //valor de la velocidad de paso, en m/s
     fichero >> nm; //Número de modos
     fichero >> te; // tiempo de evaluación
     }
  }
  fichero.close();

  }


void analisis(const Viga& viga, float &fv, float &ct, int & nm , float & te ) {
  cout<<"---------------------------------------------------"<<endl;
  cout <<"Analisis - Serie temporal"<<endl;
  cout<<"---------------------------------------------------"<<endl;
  float tviga;
  tviga = viga.lv / ct;
  cout << "Tiempo dentro del puente: " << tviga <<" (s)"<<endl;

  float tau=0.;

// Solución desplazamientos

  ofstream fichero, modos;
  fichero.open ("desplazamientos.txt", ios::out);
  modos.open ("modos.txt", ios::out);

  float desplaza;

  float  pi = M_PI; //Así se llama en C++ al número PI; funciona porque hemos incluido #include <cmath> en la cabecera.
  int i;
  float det;

  float * wn, * wd, * gn, * alfa_n, * beta_n, * a_n, * b_n, * c_n; //Se definen como punteros

  wn = new float[nm];
  wd = new float[nm];
  gn = new float[nm];
  alfa_n = new float[nm];
  beta_n = new float[nm];
  a_n = new float[nm];
  b_n = new float[nm];
  c_n = new float[nm];

// Inicialización en memoria de los parámetros modales, para evitar tener que recalcularlos en cada paso de tiempo.

// Las matrices en C++ tienen el 0 por primer índice. Por eso asignamos el wn[i-1]
  for (i=1; i<=nm; i++){

    wn[i-1] = sqrt(viga.ev*viga.iv/viga.mv)*pow(i*pi/viga.lv,2);
    wd[i-1] = sqrt((1.0-pow(viga.zi,2)))*wn[i-1];
    gn[i-1] = i*pi*ct/viga.lv;
    det = pow (2*viga.zi*wn[i-1]*gn[i-1],2)+pow(wn[i-1]*wn[i-1]-gn[i-1]*gn[i-1],2);
    alfa_n[i-1] = 4.0*fv/(viga.mv*viga.lv)*viga.zi*wn[i-1]*gn[i-1]/det;
    beta_n[i-1] = 2.0*fv/(viga.mv*viga.lv)*(gn[i-1]*gn[i-1]-wn[i-1]*wn[i-1])/det;
    a_n[i-1] = -alfa_n[i-1];
    b_n[i-1] = -(gn[i-1]*beta_n[i-1]-viga.zi*wn[i-1]*alfa_n[i-1])/wd[i-1];

    c_n[i-1] = sin(i*pi/2.);

  };

// Se escriben a fichero las frecuencias modales


  modos << "Número de modo             Frecuencia (Hz)    " <<endl;
  modos << "===============            ===============    " <<endl;
  float frequency;

  for (i=1;i<=nm;i++){

  frequency = wn[i-1]/(2.0*pi);

  modos << "           " << i << "                          " << frequency <<endl;


  }
  modos.close();

// Análisis serie tempora. Se hace con un bucle do-while. Primero la parte en vibraciones forzadas, y luego en vibraciones libres


  do {


  desplaza = 0.0;
        for(i=1;i<=nm;i++){



            desplaza += (exp(-viga.zi*wn[i-1]*tau)*(a_n[i-1]*cos(wd[i-1]*tau)+b_n[i-1]*sin(wd[i-1]*tau))+
            alfa_n[i-1]*cos(gn[i-1]*tau) + beta_n[i-1]*sin(gn[i-1]*tau))*c_n[i-1];

        };
        fichero <<std::scientific << std::setprecision(5) << std::setw(20) << tau << std::setprecision(5)<< std::setw(20) << desplaza << endl ;

  tau+=te;
  }while (tau < tviga);

// Parte en vibraciones libres. Reescalado del tiempo. Problema con exponencial no acotada.

// Parte en vibraciones libres

// Asignación de avl_n y bvl_n, coeficientes de la solución en vibracioines libres.



   float * q0n, * qp0n;

   q0n = new float[nm];
   qp0n = new float[nm];

   float * avl_n, * bvl_n; // Coeficientes a_n y b_n para la parte en vibraciones libres

   avl_n = new float[nm];
   bvl_n = new float[nm];

// Inicialización de solución en vibraciones libres

   for (i=1; i<=nm; i++){

            q0n[i-1] = exp(-viga.zi*wn[i-1]*tviga)*(a_n[i-1]*cos(wd[i-1]*tviga)+b_n[i-1]*sin(wd[i-1]*tviga))+
            alfa_n[i-1]*cos(gn[i-1]*tviga)+beta_n[i-1]*sin(gn[i-1]*tviga);

            qp0n[i-1] = -viga.zi*wn[i-1]*exp(-viga.zi*wn[i-1]*tviga)*(a_n[i-1]*cos(wd[i-1]*tviga)+b_n[i-1]*sin(wd[i-1]*tviga));
            qp0n[i-1] = qp0n[i-1] + exp(-viga.zi*wn[i-1]*tviga)*(-wd[i-1]*a_n[i-1]*sin(wd[i-1]*tviga)+wd[i-1]*b_n[i-1]*cos(wd[i-1]*tviga));
            qp0n[i-1] = qp0n[i-1] -gn[i-1]*alfa_n[i-1]*sin(gn[i-1]*tviga)+gn[i-1]*beta_n[i-1]*cos(gn[i-1]*tviga);

            avl_n[i-1] = q0n[i-1];
            bvl_n[i-1] = ( qp0n[i-1] + viga.zi * wn[i-1] * avl_n[i-1] ) / wd[i-1];
   }


   tau = tau - tviga;

   do {
        desplaza = 0.;
        for (i=1;i<=nm;i++){

            desplaza += (exp(-viga.zi*wn[i-1]*tau)*(avl_n[i-1]*cos(wd[i-1]*tau)+bvl_n[i-1]*sin(wd[i-1]*tau)))*c_n[i-1];
        };

        fichero <<std::scientific << std::setprecision(5) << std::setw(20) << tau + tviga
        << std::setprecision(5)<< std::setw(20) << desplaza << endl ;



   tau+=te;
   }while (tau < 6.); // Se dejan 6 segundos en vibraciones libres

}
