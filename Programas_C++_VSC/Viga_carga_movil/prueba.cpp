// example: class constructor
#include <iostream>
using namespace std;

/* Licencia CC BY-NC 4.0 Creative Commons */

class Rectangle {
    float width, height;
  public:
    Rectangle (float,float);
    float area () {return (width*height);}
};

Rectangle::Rectangle (float a, float b) {
  width = a;
  height = b;
}

int main () {
  Rectangle rect (3.5,4.7);
  Rectangle rectb (5.2,6.3);
  cout << "rect area: " << rect.area() << endl;
  cout << "rectb area: " << rectb.area() << endl;
  return 0;
}
