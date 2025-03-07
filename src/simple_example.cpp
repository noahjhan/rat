#include <fstream>
#include <iostream>

// this will be a very useless compiler which adds two numbers :)
using namespace std;

int main(int argc, char** argv) {
  ofstream file;
  file.open("test.s");
  file << ".global _start\n\n.section .text\n_start:\n";

  int x = 10;
  int y = 5;

  file << "\tmov x0, #" << x << "\n\tmov x1, #" << y
       << "\n\tadd x2, x0, x1\n\tret";

  file.close();
  return 0;
}
