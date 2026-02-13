#include <ncurses.h>
#include <string>

#pragma once

class Computation {
public:
  static int ceil_int_div(int a, int b) { return (a + b - 1) / b; }
};

// This class wasn't really necessary, but some of the function names in the
// n-curses lib are just too unreadable, so this works as an interface for the
// commands I use.
class Ncurses {
public:
  static void move_cursor(int y, int x) { move(y, x); }
  static int get_input_character() { return getch(); }
  static void clear_current_line() { clrtoeol(); }
  static void refresh_screen() { refresh(); }
  static void move_and_write_to(int y, int x, std::string content) {
    auto str = content.c_str();
    mvprintw(y, x, str);
  }
  static void clear_full_screen() { clear(); };
};
