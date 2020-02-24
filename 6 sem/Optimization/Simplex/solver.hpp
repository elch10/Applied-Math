#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <vector>
#include <memory>

typedef std::vector<float> FloatArray;

class SimplexSolver{

private:
  int rows, cols;
  //stores coefficients of all the variables
  std::vector < FloatArray > matr_;
  //stores constants of constraints
  FloatArray b_;
  //stores the coefficients of the objective function
  FloatArray c_;

  float maximum;
  bool isUnbounded;

public:
  SimplexSolver(std::vector <FloatArray > matrix,FloatArray b ,FloatArray c);

  bool simplexAlgorithmCalculataion();

  bool checkOptimality();

  void doPivotting(int pivotRow, int pivotColumn);

  void print();

  //find the least coefficients of constraints in the objective function's position
  int findPivotColumn();

  //find the row with the pivot value.The least value item's row in the B array
  int findPivotRow(int pivotColumn);

  void CalculateSimplex();

  static std::unique_ptr<SimplexSolver> read(std::istream &is);

};

#endif
