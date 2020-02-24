#include <iostream>
#include <cstring>
#include <fstream>

#include "solver.hpp"
using namespace std;

int main(int argc, char *argv[])
{
  if (argc != 2) {
    cerr << "Program take only one argument - path to input file or -h (--help)" << endl;
    return 0;
  }

  if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
    cout << "Input file need to be in the next format:\n"
              << "First line contain objective function coefficients\n"
              << "Second and next lines is constraints\n"
              << "Note that if some variable not exist in current constraint/objective, then it must be a zero.\n"
              << "Also all the coefficients need to be separated by space"
              << "Last two columns must contain also the constraint (<=, =, >=) with corresponding value";
    return 0;
  }

  ifstream fin(argv[1]);
  auto solver = SimplexSolver::read(fin);
  if (!solver) {
    return 0;
  }
  solver->CalculateSimplex();
  cout << "\nOptimized matrix:\n";
  solver->print();

  return 0;
}
