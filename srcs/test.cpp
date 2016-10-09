#include <iostream>

using namespace std;

extern "C" {
  #include "extApi.h"
}

int main() {
  cout << "Working!" << endl;
  return 0;
}
