#pragma once

#include <ncurses.h>
#include <string>
#include <vector>

#include "./providers.cpp"

using namespace std;

class Window {
public:
  int window_h;
  int window_w;
  int first_line;

  int padded_size() {
    return window_w - LEFT_SIDE_PADDING - RIGHT_SIDE_PADDING;
  }

  void update_window_size() { getmaxyx(stdscr, window_h, window_w); }

  Window() {
    update_window_size();
    first_line = 0;
  }
};

class Line {
public:
  string full_string;
  vector<string> wrapped_lines;

  void wrap_lines(Window &window) {

    if (window.padded_size() <= 0) {
      return;
    }

    if (full_string == "") {
      wrapped_lines = {""};
      return;
    }

    wrapped_lines.clear();

    int number_of_wrapps =
        Computation::ceil_int_div(full_string.length(), window.padded_size());

    for (int i = 0; i < number_of_wrapps; i++) {
      wrapped_lines.push_back(
          full_string.substr(window.padded_size() * i, window.padded_size()));
    }
  }

  Line(string initial_string, Window &window) {
    full_string = initial_string;
    wrap_lines(window);
  }
};
