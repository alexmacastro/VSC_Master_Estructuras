 /*
    Código viga_carga_movil
    Autor: Alejandro E. Martínez Castro
    email: amcastro@ugr.es
    Dep. Mecánica de Estructuras e Ingeniería Hidráulica. Universidad de Granada.

    Descripción: Solución analítica al problema de carga móvil en una viga de Bernoulli-Euler, biapoyada.
                 Basada en análisis modal, con solución analítica tanto en la parte espacial como temporal.
                 Implementación no optimizada, con fines didácticos.

    Curso: Vibraciones de Sistemas Continuos. Master de Estructuras.

    Objetivos: Implementar la solución de la viga de Bernoulli-Euler sometida a carga móvil aislada,
               en un entorno basado en Software Libre. El lenguaje de programación elegido ha sido C++.
               Este lenguaje ofrece numerosas ventajas y es recomendable su aprendizaje para alumnos
               que deseen ampliar sus conocimientos en programación.

               Este lenguaje permite también aprovechar buenos códigos escritos en otros lenguajes, como FORTRAN.

               Se recomienda el siguiente tutorial:

               http://www.cplusplus.com/doc/tutorial/

Licencia CC BY-NC 4.0 Creative Commons


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
  float wn, wd, gn, pi;
  pi = M_PI; //Así se llama en C++ al número PI; funciona porque hemos incluido #include <cmath> en la cabecera.
  int i;
  float det, alfa_n, beta_n, a_n, b_n;

// Parte en vibraciones forzadas.
  do {


  desplaza = 0.0;
        for(i=1;i<=nm;i++){
            wn = sqrt(viga.ev*viga.iv/viga.mv)*pow(i*pi/viga.lv,2);

            wd = sqrt((1.0-pow(viga.zi,2)))*wn;
            gn = i*pi*ct/viga.lv;

            det = pow (2*viga.zi*wn*gn,2)+pow(wn*wn-gn*gn,2);
            alfa_n = 4.0*fv/(viga.mv*viga.lv)*viga.zi*wn*gn/det;
            beta_n = 2.0*fv/(viga.mv*viga.lv)*(gn*gn-wn*wn)/det;
            a_n = -alfa_n;
            b_n = -(gn*beta_n-viga.zi*wn*alfa_n)/wd;


            desplaza += (exp(-viga.zi*wn*tau)*(a_n*cos(wd*tau)+b_n*sin(wd*tau))+alfa_n*cos(gn*tau)+beta_n*sin(gn*tau))*sin(i*pi/2.);
          //  desplaza = se calcula en el centro de vano (por eso termina en sen(i*pi*x/l) para x=l/2
        };
        fichero <<std::scientific << std::setprecision(5) << std::setw(20) << tau << std::setprecision(5)<< std::setw(20) << desplaza << endl ;

  tau+=te;
  }while (tau < tviga);

// Parte en vibraciones libres. Reescalado del tiempo. Problema con exponencial no acotada.

   float q0n, qp0n;
   tau = tau - tviga;
   do {
        desplaza = 0.;
        for (i=1;i<=nm;i++){

            wn = sqrt(viga.ev*viga.iv/viga.mv)*pow(i*pi/viga.lv,2);

            wd = sqrt((1.0-pow(viga.zi,2)))*wn;
            gn = i*pi*ct/viga.lv;

            det = pow (2*viga.zi*wn*gn,2)+pow(wn*wn-gn*gn,2);
            alfa_n = 4.0*fv/(viga.mv*viga.lv)*viga.zi*wn*gn/det;
            beta_n = 2.0*fv/(viga.mv*viga.lv)*(gn*gn-wn*wn)/det;
            a_n = -alfa_n;
            b_n = -(gn*beta_n-viga.zi*wn*alfa_n)/wd;

            q0n = exp(-viga.zi*wn*tviga)*(a_n*cos(wd*tviga)+b_n*sin(wd*tviga))+alfa_n*cos(gn*tviga)+beta_n*sin(gn*tviga);

            qp0n = -viga.zi*wn*exp(-viga.zi*wn*tviga)*(a_n*cos(wd*tviga)+b_n*sin(wd*tviga));
            qp0n = qp0n + exp(-viga.zi*wn*tviga)*(-wd*a_n*sin(wd*tviga)+wd*b_n*cos(wd*tviga));
            qp0n = qp0n -gn*alfa_n*sin(gn*tviga)+gn*beta_n*cos(gn*tviga);

            a_n = q0n;
            b_n = ( qp0n + viga.zi * wn * a_n ) / wd;

            desplaza += (exp(-viga.zi*wn*tau)*(a_n*cos(wd*tau)+b_n*sin(wd*tau)))*sin(i*pi/2.);
        };

        fichero <<std::scientific << std::setprecision(5) << std::setw(20) << tau + tviga
        << std::setprecision(5)<< std::setw(20) << desplaza << endl ;



   tau+=te;
   }while (tau < 6.); // Se dejan 6 segundos en vibraciones libres

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
