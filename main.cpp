#include "ncurses.h"
#include <string>
#include <vector>
using namespace std;

// This class wasn't really necessary, but some of the function names in the
// n-curses lib are just too unreadable, so this works as an interface for the
// commands I use.
class Ncurses {
public:
  static void move_cursor(int x, int y) { move(x, y); }
  static int get_input_character() { return getch(); }
  static void clear_current_line() { clrtoeol(); }
  static void refresh() { refresh(); }
  static void move_and_write_to(int y, int x, string content) {
    mvprintw(y, x, content.c_str());
  }
};

// Logic of a single buffer
class Buffer {
public:
  vector<string> lines_above;
  vector<string> lines_bellow;
  string before_cursor;
  string after_cursor;
  int line_number;

  // Cursor x,y
  int cursor_x;
  int cursor_y;

  Buffer() {
    // Scratch file case
    lines_above = {};
    lines_bellow = {};
    before_cursor = "";
    after_cursor = " << esto es un ejemplo!";
    cursor_x = 0;
    cursor_y = 5;
  }

  void print_current_line() { print_line(cursor_y); };

  void print_line(int line_number) {
    string line = before_cursor + after_cursor;
    Ncurses::clear_current_line();
    Ncurses::move_and_write_to(line_number, 0, line);
    Ncurses::move_cursor(cursor_y, cursor_x);
  };

  void insert_char(char ch) {
    before_cursor.push_back(ch);
    cursor_x += 1;
  }

  void delete_char() {
    if (before_cursor == "") {
      return;
    }
    before_cursor.pop_back();
    cursor_x -= 1;
  }

  void move_to_cursor_location() { Ncurses::move_cursor(cursor_y, cursor_x); }
};

void ncurses_loop(Buffer buf) {
  initscr();
  keypad(stdscr, TRUE);
  buf.print_current_line();
  while (true) {
    int ch = Ncurses::get_input_character();

    switch (ch) {
    case KEY_BACKSPACE:
      buf.delete_char();
      break;
    case '\n':
    case '\r':
      // TODO
      break;
    default:
      buf.insert_char(ch);
      break;
    }

    buf.print_current_line();
  }
  endwin();
}

int main(int argc, char *argv[]) {
  Buffer test = Buffer();
  ncurses_loop(test);
  return 0;
}
