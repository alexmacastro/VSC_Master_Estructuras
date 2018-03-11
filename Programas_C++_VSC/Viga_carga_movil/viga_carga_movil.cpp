 /*
    C�digo viga_carga_movil
    Autor: Alejandro E. Mart�nez Castro
    email: amcastro@ugr.es
    Dep. Mec�nica de Estructuras e Ingenier�a Hidr�ulica. Universidad de Granada.

    Descripci�n: Soluci�n anal�tica al problema de carga m�vil en una viga de Bernoulli-Euler, biapoyada.
                 Basada en an�lisis modal, con soluci�n anal�tica tanto en la parte espacial como temporal.
                 Implementaci�n no optimizada, con fines did�cticos.

    Curso: Vibraciones de Sistemas Continuos. Master de Estructuras.

    Objetivos: Implementar la soluci�n de la viga de Bernoulli-Euler sometida a carga m�vil aislada,
               en un entorno basado en Software Libre. El lenguaje de programaci�n elegido ha sido C++.
               Este lenguaje ofrece numerosas ventajas y es recomendable su aprendizaje para alumnos
               que deseen ampliar sus conocimientos en programaci�n.

               Este lenguaje permite tambi�n aprovechar buenos c�digos escritos en otros lenguajes, como FORTRAN.

               Se recomienda el siguiente tutorial:

               http://www.cplusplus.com/doc/tutorial/

Licencia CC BY-NC 4.0 Creative Commons


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
  float wn, wd, gn, pi;
  pi = M_PI; //As� se llama en C++ al n�mero PI; funciona porque hemos incluido #include <cmath> en la cabecera.
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
