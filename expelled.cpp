#include <cstdio>
#include <list>
#include <unordered_map>

/*to compile: g++ -std=c++11 -o expelled expelled.cpp */

using namespace std;

class Expelled {
public:
  Expelled()
    : sequence_count(1),
      last_element(0)
  {
    add_elements_to_sequence(100);
  }
  
  int find_expelled(int n)
  {
    while (results.find(n) == results.end()) {
      advance_sequence();
    }
    return results[n];  
  }

  void print_first_n_elements(int n = 20)
  {
    printf("%d: ",sequence_count);
    auto it = sequence.begin();
    for (int i = 0; i < n; ++i) {
      printf("%2d ", *it);
      it++;
    }
    printf("...\n");
  }

private:
  list<int> sequence;
  int sequence_count;
  int last_element;
  unordered_map<int, int> results;

  void add_elements_to_sequence(int n)
  {
    for (int i = 0; i < n; ++i) {
      sequence.push_back(++last_element);
    }
  }

  void advance_sequence()
  {
    if (last_element < sequence_count*3 + 1) {
      add_elements_to_sequence(sequence_count*3 + 1 - last_element);
    }
    auto middle_it = sequence.begin();
    advance(middle_it, sequence_count-1);
    auto forward_it = middle_it;
    auto backward_it = middle_it;
    list<int> temp;
    while(forward_it != sequence.begin()) {
      advance(forward_it, -1);
      advance(backward_it, 1);
      temp.push_back(*forward_it);
      temp.push_back(*backward_it);
    }
    results[*middle_it] = sequence_count;
    // delete repeaated elements
    advance(backward_it, 1);
    sequence.erase(sequence.begin(), backward_it);
    sequence.splice(sequence.begin(), temp);
    sequence_count++;
    print_first_n_elements();

  }

};

int main(int argc, char *argv[])
{
  Expelled expelled;
  int answer = expelled.find_expelled(16);
  printf("%d\n", answer);

  for (int i = 0; i < 21; ++i) {
    answer = expelled.find_expelled(i + 1);
    printf("%2d: %3d\n", i+1, answer);
  }
  
  return 0;
}
