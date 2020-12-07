#include <iostream>
#include <fstream>
#include <string>
#include <vector>

/*to compile: g++ -std=c++11 -o study_and_ponder study_and_ponder.cpp */

using namespace std;

int main(int argc, char *argv[])
{
  ifstream infile;
  infile.open("string.txt");
  string to_compare;
  getline(infile, to_compare);
  // cout<<to_compare<<endl;
  infile.close();
  
  infile.open("s_and_p_list.txt");
  vector<string> tickers;
  int ind = 0;
  string s;
  while (getline(infile,s)) {
    tickers.push_back(s);
    // cout<<tickers[ind]<<endl;
    ind++;
  }
  infile.close();
  for (int i = 0; i < tickers.size(); ++i) {
    if (to_compare.find(tickers[i]) == to_compare.npos) {
      cout<<tickers[i]<<endl;
    }
  }
  return 0;
}
