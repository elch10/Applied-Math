#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <cstring>
#include <fstream>
using namespace std;

/*

The main method is in this program itself.

Instructions for compiling=>>

Run on any gcc compiler=>>

Special***** should compile in -std=c++11 or C++14 -std=gnu++11  *********  (mat be other versions syntacs can be different)

turorials point online compiler
==> go ti link   http://cpp.sh/ or  https://www.tutorialspoint.com/cplusplus/index.htm and click try it(scorel below) and after go to c++ editor copy code and paste.
after that click button execute.

if you have -std=c++11 you can run in command line;
g++ -o output Simplex.cpp
./output


How to give inputs to the program =>>>

   Example:
	colSizeA = 6 // input colmn size
	rowSizeA = 3  // input row size

  float C[N]={-6,-5,-4,0,0,0};  //Initialize the C array  with the coefficients of the constraints of the objective function
  float B[M]={240,360,300};//Initialize the B array constants of the constraints respectively


   //initialize the A array by giving all the coefficients of all the variables
   float A[M][N] =  {
         { 2,  1,  1,   1,  0, 0},
        { 1,  3,  2,   0,  1, 0 },
        {   2,  1,  2,   0,  0,  1}
        };

*/

typedef vector<float> FloatArray;

class SimplexSolver{

private:
  int rows, cols;
  //stores coefficients of all the variables
  vector < FloatArray > matr_;
  //stores constants of constraints
  FloatArray b_;
  //stores the coefficients of the objective function
  FloatArray c_;

  float maximum;
  bool isUnbounded;

public:
  SimplexSolver(vector <FloatArray > matrix,FloatArray b ,FloatArray c):
    maximum(0), isUnbounded(false), rows(matrix.size()), cols(matrix[0].size()),
    matr_(matrix), b_(b), c_(c)
  {}

  bool simplexAlgorithmCalculataion()
  {
    //check whether the table is optimal,if optimal no need to process further
    if (checkOptimality()) {
      return true;
    }

    //find the column which has the pivot.The least coefficient of the objective function(C array).
    int pivotColumn = findPivotColumn();

    if (isUnbounded == true) {
      cout<<"Error unbounded"<<endl;
      return true;
    }

    //find the row with the pivot value.The least value item's row in the B array
    int pivotRow = findPivotRow(pivotColumn);

    //form the next table according to the pivot value
    doPivotting(pivotRow, pivotColumn);

    return false;
  }

  bool checkOptimality()
  {
    //if the table has further negative constraints,then it is not optimal
    //check if the coefficients of the objective function are negative
    //if all the constraints are positive now,the table is optimal
    return all_of(c_.begin(), c_.end(), [](const float &el) {
      return el >= 0;
    });
  }

  void doPivotting(int pivotRow, int pivotColumn)
  {

    //gets the pivot value
    float pivotValue = matr_[pivotRow][pivotColumn];

    //the column with the pivot
    FloatArray pivotRowVals(matr_[pivotRow].begin(), matr_[pivotRow].begin() + cols);

    //the row with the pivot
    FloatArray pivotColVals(rows);

    //update the maximum step by step
    maximum = maximum - (c_[pivotColumn]*(b_[pivotRow]/pivotValue));

    //get the column that has the pivot value
    for (int j = 0; j < rows; j++) {
      pivotColVals[j] = matr_[j][pivotColumn];
    }

    //the row after processing the pivot value
    FloatArray rowNew(pivotRowVals.begin(), pivotRowVals.end());
    
    //set the row values that has the pivot value divided by the pivot value and put into new row
    transform(rowNew.begin(), rowNew.end(), rowNew.begin(), [pivotValue](const float &v) {
      return v / pivotValue;
    });

    b_[pivotRow] = b_[pivotRow]/pivotValue;


    //process the other coefficients in the A array by subtracting
    for (int m = 0; m < rows; m++) {
      //ignore the pivot row as we already calculated that
      if (m !=pivotRow) {
        float multiplyValue = pivotColVals[m];
        for (int p = 0; p < cols; p++) {
          matr_[m][p] = matr_[m][p] - (multiplyValue*rowNew[p]);
        }
      }
    }

    //process the values of the B array
    for (int i = 0;i < b_.size(); i++) {
      if (i != pivotRow) {
          float multiplyValue = pivotColVals[i];
          b_[i] = b_[i] - (multiplyValue*b_[pivotRow]);
      }
    }

    //the least coefficient of the constraints of the objective function
    float multiplyValue = c_[pivotColumn];
    for (int i = 0; i < c_.size(); i++) {
      c_[i] = c_[i] - (multiplyValue*rowNew[i]);
    }

    //replacing the pivot row in the new calculated A array
    copy_n(rowNew.begin(), cols, matr_[pivotRow].begin());
  }

  //print the current A array
  void printMatrix()
  {
    for (int i = 0; i < rows; i++) {
      copy(matr_[i].begin(), matr_[i].end(), ostream_iterator<float>(cout, " "));
      cout << endl;
    }
  }

  //find the least coefficients of constraints in the objective function's position
  int findPivotColumn()
  {
    return min_element(c_.begin(), c_.end()) - c_.begin();
  }

  //find the row with the pivot value.The least value item's row in the B array
  int findPivotRow(int pivotColumn)
  {
    FloatArray positiveValues(rows);
    FloatArray result(rows,0);
    //float result[rows];
    int negativeValueCount = 0;

    for (int i = 0; i < rows; i++){
      if (matr_[i][pivotColumn] > 0) {
        positiveValues[i] = matr_[i][pivotColumn];
      } else {
        positiveValues[i] = 0;
        negativeValueCount += 1;
      }
    }
    //checking the unbound condition if all the values are negative ones
    if (negativeValueCount == rows) {
      isUnbounded = true;
    } else {
      for (int i = 0;i < rows; i++) {
        float value = positiveValues[i];
        if (value > 0) {
          result[i] = b_[i] / value;
        } else {
          result[i] = 0;
        }
      }
    }

    //find the minimum's location of the smallest item of the B array
    float minimum = 99999999;
    int location = 0;
    for (int i = 0; i < result.size(); i++) {
      if (0 < result[i] && result[i] < minimum){
          minimum = result[i];
          location = i;
      }
    }
    return location;
  }

  void CalculateSimplex()
  {
    cout<<"initial array(Not optimal)"<<endl;
    printMatrix();

    cout<<" "<<endl;
    cout<<"final array(Optimal solution)"<<endl;

    while (!simplexAlgorithmCalculataion()) {}

    cout<<"Answers for the Constraints of variables"<<endl;

    // every basic column has the values, get it from B array
    for (int i = 0; i < matr_.size(); i++){
      int count0 = 0, index = 0;
      for (int j = 0; j < rows; j++) {
        if (matr_[j][i] == 0.0) {
          count0 += 1;
        } else if (matr_[j][i] == 1) {
          index = j;
        }
      }
      // every basic column has the values, get it from B array
      cout << "variable" << index+1 << ": " << (count0 == rows-1 ? b_[index]: 0) << endl;
    }

    cout << endl;
    cout << "maximum value: " << maximum << endl;  //print the maximum values
  }

};

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
  string s, tmp;
  getline(fin, s);

  FloatArray c;

  istringstream is(s);
  while (is >> tmp) {
    c.push_back(-stof(tmp));
  }

  vector<FloatArray> vec2D;
  FloatArray b;

  // enum 
  vector<string> constraintTypes;

  while (getline(fin, s)) {
    istringstream is(s);
    vec2D.push_back(FloatArray());
    while (is >> tmp) {
      if (tmp == "<=" || tmp == "=" || tmp == ">=") {
        constraintTypes.push_back(tmp);
        continue;
      }
      vec2D[vec2D.size()-1].push_back(stof(tmp));
    }
    if ((vec2D.size() > 1 && vec2D.back().size() != vec2D.back().size())
        || vec2D.back().size() != c.size() + 1) {
      cerr << "Number of variables in constraints doesn't match "
           << "or number of variables in constraints doesn't match objective";
      return 0;
    }
    if (constraintTypes.back() == ">=") {
      constraintTypes.back() = "<=";
      for (float &el: vec2D.back()) {
        el = -el;
      }
    }
  }
  
  int cntSlack = count(constraintTypes.begin(), constraintTypes.end(), "<=");
  for (int i = 0, j = 0; i < vec2D.size(); ++i) {
    b.push_back(vec2D[i].back());
    vec2D[i].pop_back();
    fill_n(std::back_inserter(vec2D[i]), cntSlack, 0);
    if (constraintTypes[i] == "<=") {
      vec2D[i][vec2D[i].size()-cntSlack+(j++)] = 1;
    }
  }

  // hear the make the class parameters with A[m][n] vector b[] vector and c[] vector
  SimplexSolver solver(vec2D,b,c);
  solver.CalculateSimplex();
  solver.printMatrix();

  return 0;
}
