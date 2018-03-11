/*

  Licencia CC BY-NC 4.0 Creative Commons


    C�digo viga_carga_movil_eficiente
    Autor: Alejandro E. Mart�nez Castro
    email: amcastro@ugr.es
    Dep. Mec�nica de Estructuras e Ingenier�a Hidr�ulica. Universidad de Granada.

    Descripci�n: Soluci�n anal�tica al problema de carga m�vil en una viga de Bernoulli-Euler, biapoyada.
                 Basada en an�lisis modal, con soluci�n anal�tica tanto en la parte espacial como temporal.
                 Implementaci�n OPTIMIZADA, para
                 - Minimizar operaciones de punto flotante en cada instante de tiempo.

                 Para eso, se almacenan en memoria todas las variables modales que no necesitan ser recalculadas en cada instante de tiempo.

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

using namespace std; //Carga el espacio de nombres de la liber�a est�ndard std

/* DEFINICI�N DE LAS CLASES

    C++ es un lenguaje orientado a OBJETOS.
    Un objeto pertenecen a una CLASE, la cual definiremos como deseemos.

    Primero se define la clase "Viga". �sta nos permitir� definir objetos de tipo "Viga".

    As�, primero definiremos la clase "Viga", la cual tiene los siguientes MIEMBROS:

      Miembros privados: S�lo accesibles a los objetos de tipo "Viga" y a funciones "amigas" de la clase "Viga".

         Una Viga TIENE COMO CARACTER�STICAS:
           - Una longitud lv
           - Un m�dulo el�stico: ev
           - Un momento de inercia: iv
           - Una densidad de masa por unidad de l�nea: mv
           - Una tasa de amortiguamiento modal constante: zi (en "tanto por uno")
           - Un n�mero de divisiones interno para post-proceso.

      Miembros p�blicos: Accesibles desde fuera, para otros objetos distintos de "Viga".

      Un objeto de tipo Viga HACE COSAS.
        - Inicializa sus variables, mediante una funci�n que llamaremos "iniciar viga".
          Este tipo de funciones se denominan tambi�n "constructores".

        - Adem�s, vamos a definir unas funciones "amigas" de la clase "Viga".
          Las funciones "amigas" no son funciones de la clase,
          pero pueden acceder a los par�metros privados de la clase.
          Este rasgo se denomina en ingl�s "friendship" y constituye un rasgo de C++.

          Las funciones amigas van a ser las siguientes:

          datospantalla -> Para mostrar por pantalla los datos de la viga.
          analisis -> Para realizar el an�lisis din�mico.

*/
class Viga{
  private:
   float lv; //longitud de la viga
   float ev; //m�dulo el�stico de la viga
   float iv; //momento de inercia de la viga
   float mv; //masa / longitud
   float zi; //tasa de amortiguamiento modal constante (la almacena en valor).
   int nd; // Numero de divisiones
  public:
   void iniciar_vigaf(); // Funci�n para iniciar la viga desde fichero.
   friend void datos_pantalla(const Viga & );
   friend void analisis(const Viga& , float & , float & , int &  , float &  );
  }; // Este "punto y coma" es muy importante, al definir una CLASE. Omitirlo suele ser un error frecuente en principiantes.

/* Ahora definiremos c�mo act�a la funci�n de la clase iniciar_vigaf()

    Si un objeto de tipo viga, es invocado con esta funci�n, la funci�n:
     - Abre un fichero, que vamos a denominar "datosviga.dat".
     - Lee los par�metros del fichero.
     - Inicializa los par�metros privados de la clase "viga".
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
  } // Y OJO, aqu� no se pone el punto y coma. S�lo en la definici�n de clases.

/* Definimos ahora el comportamiento de la funci�n "amiga" de la clase Viga denominada datos_pantalla.
    La funci�n:
    - No devuelve par�metros. Por eso lleva un "void" como tipo de dato de salida (vac�o).
    - Se le pasa un objeto de la clase "Viga" por referencia (por eso lleva un ampersand &) y para que no se pueda cambiar el valor
      del objeto viga, se le indica que es "constante" con const. El objeto se llama "beam".

      El paso de par�metros por referencia es fundamental para no saturar la memoria del sistema. Al pasar una variable
      por referencia, no se crean nuevas copias en la memoria, sino que se generan enlaces.

      Esta funci�n nos permite escribir en pantalla (cout) el contenido del objeto "beam", que es de la clase "Viga".

    - Obs�rvese c�mo se accede a los miembros de la clase: beam.lv; beam.ev, etc.

*/

  void datos_pantalla(const Viga& beam){ // Atenci�n al ampersand (&) para el paso por referencia, y "const" para que no pueda alterarse su valor en la salida
  cout << "Longitud: " << beam.lv <<" (m)" << endl;
  cout << "Modulo de elasticidad: " << beam.ev << " (N/m^2)" << endl;
  cout << "Momento de inercia: " << beam.iv << " (m^4)"<< endl;
  cout << "Masa/longitud: " << beam.mv <<" (kg/m)" << endl;
  cout << "Tasa de amortiguamiento: " << beam.zi*100. << "(%)" <<endl ;
  cout << "Divisiones en la viga: " << beam.nd << endl ;
  }

/* Se define una funci�n "leedatos" para leer otros par�metros que est�n escritos en el fichero "datosviga.dat" y
   que son necesarios para el an�lisis din�mico. Esta funci�n no necesita acceder a par�metros de la clase Viga.

   Esta funci�n es an�loga a una "subrutina" de FORTRAN.

   Obs�rvese el paso de par�metros por referencia. Esta vez no ponemos que sean "const" porque queremos
   que los par�metros se "escriban" y por tanto, la funci�n necesita modificar a la salida los valores
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
     fichero >> nm; //N�mero de modos
     fichero >> te; // tiempo de evaluaci�n
     }
  }
  fichero.close();

  }

/* Ahora se define la funci�n que har� el an�lisis.

   Esta funci�n accede a los datos de la clase "Viga", en el objeto "viga".

   Se implementa la soluci�n anal�tica para viga isost�tica. Se define la serie temporal de los desplazamientos.

   La implementaci�n no es eficiente; en cada instante de tiempo, se vuelven a calcular los par�metros modales.

   En otros c�digos veremos c�mo poder hacerla m�s eficiente (requiriendo menor n�mero de operaciones de punto flotante).
*/

void analisis(const Viga& viga, float &fv, float &ct, int & nm , float & te ) {
  cout<<"---------------------------------------------------"<<endl;
  cout <<"Analisis - Serie temporal"<<endl;
  cout<<"---------------------------------------------------"<<endl;
  float tviga;
  tviga = viga.lv / ct;
  cout << "Tiempo dentro del puente: " << tviga <<" (s)"<<endl;

  float tau=0.;

// Soluci�n desplazamientos

  ofstream fichero;
  fichero.open ("desplazamientos.txt", ios::out);

  float desplaza;

  float  pi = M_PI; //As� se llama en C++ al n�mero PI; funciona porque hemos incluido #include <cmath> en la cabecera.
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
  c_n = new float[nm]; //Contiene los t�rminos de los puntos de postproceso.

// Inicializaci�n en memoria de los par�metros modales, para evitar tener que recalcularlos en cada paso de tiempo.

// Las matrices en C++ tienen el 0 por primer �ndice. Por eso asignamos el wn[i-1]
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

// Asignaci�n de avl_n y bvl_n, coeficientes de la soluci�n en vibracioines libres.



   float  q0n,  qp0n;

   float * avl_n, * bvl_n; // Coeficientes a_n y b_n para la parte en vibraciones libres

   avl_n = new float[nm];
   bvl_n = new float[nm];

// Inicializaci�n de soluci�n en vibraciones libres

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
