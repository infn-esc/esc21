#include "simpleSinCos.h"
#include <cmath>
#include <iostream>

int main()
{
  float b = 50.;

  /*
  for (float p=-M_PI; p<M_PI; p+=0.1) {
    int x = b*std::cos(p);
    int y = b*std::sin(p);
    int q = b*p;
    int w = b*(1.f-0.5f*p*p);
    std::cout << p << ' ' << x << ' ' << y
              << ' ' << w << ' ' << q << std::endl;
  }
  */

  for (float p = -M_PI; p < M_PI; p += 0.001) {
    int x = b * std::cos(p);
    int y = b * std::sin(p);
    int q = b * simpleSin(p);
    int w = b * simpleCos(p);
    if (x != w || y != q)
      std::cout << p << ' ' << x << ' ' << y << ' ' << w << ' ' << q
                << std::endl;
  }
  return 0;
}
