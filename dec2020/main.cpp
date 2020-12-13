#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

#include "twentyfourseven.cpp"


using namespace std;
using Clock = std::chrono::high_resolution_clock;

template <typename A>
using mat = vector<vector<A>>;

TwentyFourSeven parse_input_file(string filename);
  
int main(int argc, char** argv) {
  // this will allow different input files to be passed
  string filename;
  if (argc > 1) {
    filename = argv[1];
  } else {
    // default input
    filename = "test.txt";
  }
  
  cout << filename << endl;
  TwentyFourSeven test = parse_input_file(filename);
  test.print();
  cout << "Board is valid: " << test.is_valid_board() << endl;

  auto t1 = Clock::now();
  bool solved = test.solve();
  auto t2 = Clock::now();
  test.print();
  chrono::duration<double> elapsed_s = t2 - t1;
  cout << "Elapsed time: " << elapsed_s.count() << "s" << endl;
    
  if (solved) {
    cout << "Solve success" << endl;
  } else {
    cout << "Solve failed" <<endl;
  }
  
  return 0;
}


TwentyFourSeven parse_input_file(string filename) {
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
  TwentyFourSeven puzzle(givens, rows, cols);
  return puzzle;
}
