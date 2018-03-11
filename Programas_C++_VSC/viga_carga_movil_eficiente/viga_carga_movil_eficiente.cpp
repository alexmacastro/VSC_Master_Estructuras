/*

  Licencia CC BY-NC 4.0 Creative Commons


    Código viga_carga_movil_eficiente
    Autor: Alejandro E. Martínez Castro
    email: amcastro@ugr.es
    Dep. Mecánica de Estructuras e Ingeniería Hidráulica. Universidad de Granada.

    Descripción: Solución analítica al problema de carga móvil en una viga de Bernoulli-Euler, biapoyada.
                 Basada en análisis modal, con solución analítica tanto en la parte espacial como temporal.
                 Implementación OPTIMIZADA, para
                 - Minimizar operaciones de punto flotante en cada instante de tiempo.

                 Para eso, se almacenan en memoria todas las variables modales que no necesitan ser recalculadas en cada instante de tiempo.

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

using namespace std; //Carga el espacio de nombres de la libería estándard std

/* DEFINICIÓN DE LAS CLASES

    C++ es un lenguaje orientado a OBJETOS.
    Un objeto pertenecen a una CLASE, la cual definiremos como deseemos.

    Primero se define la clase "Viga". Ésta nos permitirá definir objetos de tipo "Viga".

    Así, primero definiremos la clase "Viga", la cual tiene los siguientes MIEMBROS:

      Miembros privados: Sólo accesibles a los objetos de tipo "Viga" y a funciones "amigas" de la clase "Viga".

         Una Viga TIENE COMO CARACTERÍSTICAS:
           - Una longitud lv
           - Un módulo elástico: ev
           - Un momento de inercia: iv
           - Una densidad de masa por unidad de línea: mv
           - Una tasa de amortiguamiento modal constante: zi (en "tanto por uno")
           - Un número de divisiones interno para post-proceso.

      Miembros públicos: Accesibles desde fuera, para otros objetos distintos de "Viga".

      Un objeto de tipo Viga HACE COSAS.
        - Inicializa sus variables, mediante una función que llamaremos "iniciar viga".
          Este tipo de funciones se denominan también "constructores".

        - Además, vamos a definir unas funciones "amigas" de la clase "Viga".
          Las funciones "amigas" no son funciones de la clase,
          pero pueden acceder a los parámetros privados de la clase.
          Este rasgo se denomina en inglés "friendship" y constituye un rasgo de C++.

          Las funciones amigas van a ser las siguientes:

          datospantalla -> Para mostrar por pantalla los datos de la viga.
          analisis -> Para realizar el análisis dinámico.

*/
class Viga{
  private:
   float lv; //longitud de la viga
   float ev; //módulo elástico de la viga
   float iv; //momento de inercia de la viga
   float mv; //masa / longitud
   float zi; //tasa de amortiguamiento modal constante (la almacena en valor).
   int nd; // Numero de divisiones
  public:
   void iniciar_vigaf(); // Función para iniciar la viga desde fichero.
   friend void datos_pantalla(const Viga & );
   friend void analisis(const Viga& , float & , float & , int &  , float &  );
  }; // Este "punto y coma" es muy importante, al definir una CLASE. Omitirlo suele ser un error frecuente en principiantes.

/* Ahora definiremos cómo actúa la función de la clase iniciar_vigaf()

    Si un objeto de tipo viga, es invocado con esta función, la función:
     - Abre un fichero, que vamos a denominar "datosviga.dat".
     - Lee los parámetros del fichero.
     - Inicializa los parámetros privados de la clase "viga".
     - Cierra el fichero.
*/
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
  } // Y OJO, aquí no se pone el punto y coma. Sólo en la definición de clases.

/* Definimos ahora el comportamiento de la función "amiga" de la clase Viga denominada datos_pantalla.
    La función:
    - No devuelve parámetros. Por eso lleva un "void" como tipo de dato de salida (vacío).
    - Se le pasa un objeto de la clase "Viga" por referencia (por eso lleva un ampersand &) y para que no se pueda cambiar el valor
      del objeto viga, se le indica que es "constante" con const. El objeto se llama "beam".

      El paso de parámetros por referencia es fundamental para no saturar la memoria del sistema. Al pasar una variable
      por referencia, no se crean nuevas copias en la memoria, sino que se generan enlaces.

      Esta función nos permite escribir en pantalla (cout) el contenido del objeto "beam", que es de la clase "Viga".

    - Obsérvese cómo se accede a los miembros de la clase: beam.lv; beam.ev, etc.

*/

  void datos_pantalla(const Viga& beam){ // Atención al ampersand (&) para el paso por referencia, y "const" para que no pueda alterarse su valor en la salida
  cout << "Longitud: " << beam.lv <<" (m)" << endl;
  cout << "Modulo de elasticidad: " << beam.ev << " (N/m^2)" << endl;
  cout << "Momento de inercia: " << beam.iv << " (m^4)"<< endl;
  cout << "Masa/longitud: " << beam.mv <<" (kg/m)" << endl;
  cout << "Tasa de amortiguamiento: " << beam.zi*100. << "(%)" <<endl ;
  cout << "Divisiones en la viga: " << beam.nd << endl ;
  }

/* Se define una función "leedatos" para leer otros parámetros que están escritos en el fichero "datosviga.dat" y
   que son necesarios para el análisis dinámico. Esta función no necesita acceder a parámetros de la clase Viga.

   Esta función es análoga a una "subrutina" de FORTRAN.

   Obsérvese el paso de parámetros por referencia. Esta vez no ponemos que sean "const" porque queremos
   que los parámetros se "escriban" y por tanto, la función necesita modificar a la salida los valores
   que tuviesen a la entrada.
*/

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

/* Ahora se define la función que hará el análisis.

   Esta función accede a los datos de la clase "Viga", en el objeto "viga".

   Se implementa la solución analítica para viga isostática. Se define la serie temporal de los desplazamientos.

   La implementación no es eficiente; en cada instante de tiempo, se vuelven a calcular los parámetros modales.

   En otros códigos veremos cómo poder hacerla más eficiente (requiriendo menor número de operaciones de punto flotante).
*/

void analisis(const Viga& viga, float &fv, float &ct, int & nm , float & te ) {
  cout<<"---------------------------------------------------"<<endl;
  cout <<"Analisis - Serie temporal"<<endl;
  cout<<"---------------------------------------------------"<<endl;
  float tviga;
  tviga = viga.lv / ct;
  cout << "Tiempo dentro del puente: " << tviga <<" (s)"<<endl;

  float tau=0.;

// Solución desplazamientos

  ofstream fichero;
  fichero.open ("desplazamientos.txt", ios::out);

  float desplaza;

  float  pi = M_PI; //Así se llama en C++ al número PI; funciona porque hemos incluido #include <cmath> en la cabecera.
  int i;
  float det;

  float * wn, * wd, * gn, * alfa_n, * beta_n, * a_n, * b_n, *c_n; //Se definen como punteros

  wn = new float[nm];
  wd = new float[nm];
  gn = new float[nm];
  alfa_n = new float[nm];
  beta_n = new float[nm];
  a_n = new float[nm];
  b_n = new float[nm];
  c_n = new float[nm]; //Contiene los términos de los puntos de postproceso.

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



   float  q0n,  qp0n;

   float * avl_n, * bvl_n; // Coeficientes a_n y b_n para la parte en vibraciones libres

   avl_n = new float[nm];
   bvl_n = new float[nm];

// Inicialización de solución en vibraciones libres

   for (i=1; i<=nm; i++){

            q0n = exp(-viga.zi*wn[i-1]*tviga)*(a_n[i-1]*cos(wd[i-1]*tviga)+b_n[i-1]*sin(wd[i-1]*tviga))+
            alfa_n[i-1]*cos(gn[i-1]*tviga)+beta_n[i-1]*sin(gn[i-1]*tviga);

            qp0n = -viga.zi*wn[i-1]*exp(-viga.zi*wn[i-1]*tviga)*(a_n[i-1]*cos(wd[i-1]*tviga)+b_n[i-1]*sin(wd[i-1]*tviga));
            qp0n = qp0n + exp(-viga.zi*wn[i-1]*tviga)*(-wd[i-1]*a_n[i-1]*sin(wd[i-1]*tviga)+wd[i-1]*b_n[i-1]*cos(wd[i-1]*tviga));
            qp0n = qp0n -gn[i-1]*alfa_n[i-1]*sin(gn[i-1]*tviga)+gn[i-1]*beta_n[i-1]*cos(gn[i-1]*tviga);

            avl_n[i-1] = q0n;
            bvl_n[i-1] = ( qp0n + viga.zi * wn[i-1] * avl_n[i-1] ) / wd[i-1];
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



  fichero.close();

}


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
