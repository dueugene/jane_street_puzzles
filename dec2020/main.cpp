#include <iostream>
#include <fstream>

#include "twentyfourseven.cpp"


using namespace std;

template <typename A>
using mat = vector<vector<A>>;


// todo:
// confirm that a solved state will pass
// perhaps prune the solve algorithm
// possible as opposed to validate grid
// add input method
// find better way for checking 2x2 and seen constraints so that method is const

int main(int argc, char** argv) {
  // this will allow different input files to be passed
  string filename;
  if (argc > 1) {
    filename = argv[1];
  } else {
    // default input
    filename = "test.txt";
  }

  // read input and populate matrices
  ifstream input;
  input.open(filename);
  int a, b, c;
  int n_givens;
  mat<int> givens;
  input >> n_givens;
  while (n_givens--) {
    input >> a >> b >> c;
    givens.push_back({a, b, c});
  }
  n_givens = 7;
  mat<int> rows;
  while (n_givens--) {
    input >> a >> b;
    rows.push_back({a, b});
  }
  n_givens = 7;
  mat<int> cols;
  while (n_givens--) {
    input >> a >> b;
    cols.push_back({a, b});
  } 
  input.close();

  cout << "Hello World" << endl; 
  TwentyFourSeven test(givens, rows, cols);
  test.print();
  cout << test.is_valid_board() << endl;
  cout << test.solve() << endl;
  test.print();
  return 0;
}
