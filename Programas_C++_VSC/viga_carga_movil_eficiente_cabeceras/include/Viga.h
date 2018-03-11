#ifndef VIGA_H
#define VIGA_H

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
   friend void datos_pantalla(const Viga &  );
   friend void analisis(const Viga & , float &  , float & , int &  , float &  );
  };


#endif // VIGA_H
