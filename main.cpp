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

  Buffer() {
    // Scratch file case
    line_number = 0;
    lines_above = {};
    lines_bellow = {};
    before_cursor = "";
    after_cursor = "";
  }

  void insert_line(int index, string content = "");
};

int main(int argc, char *argv[]) {
  auto test = Buffer();
  return 0;
}
