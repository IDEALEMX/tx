#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Buffer {
public:
  vector<string> lines_above;
  vector<string> lines_bellow;
  string before_cursor;
  string after_cursor;
  int line_number;

  Buffer() { line_number = 0; }

  void insert_line(int index, string content = "");
};

int main(int argc, char *argv[]) {
  auto test = Buffer();
  return 0;
}
