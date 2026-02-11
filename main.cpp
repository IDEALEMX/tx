#include "ncurses.h"
#include <cassert>
#include <string>
#include <vector>
using namespace std;

// > 3
int LEFT_SIDE_PADDING = 4;
// > 1
int RIGHT_SIDE_PADDING = 2;

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
  static void move_and_write_to(int y, int x, string content) {
    auto str = content.c_str();
    mvprintw(y, x, str);
  }
  static void clear_full_screen() { clear(); };
};

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

  void wrap_lines(Window &window) {}

  Line(string initial_string, Window &window) {
    full_string = initial_string;
    wrap_lines(window);
  }
};

class Buffer {
public:
  // full text buffer separated into lines
  vector<Line> lines;

  Window window;

  // Cursor x,y
  int cursor_x;
  int cursor_y;

  Buffer() {
    // Scratch file case
    cursor_x = 0;
    cursor_y = 0;

    window = Window();
    lines.push_back(
        Line("holaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
             "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
             "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
             window));
    lines.push_back(Line("mi", window));
    lines.push_back(Line("amigo", window));
  }

  void delete_character() {}

  void full_screen_render() {}
};

void ncurses_loop(Buffer &buf) {
  keypad(stdscr, TRUE);
  buf.full_screen_render();

  while (true) {
    int ch = Ncurses::get_input_character();

    int lines_sum = 0;
    switch (ch) {
    case KEY_BACKSPACE:
      buf.delete_character();
      break;
    case '\n':
    case '\r':
      break;
    case KEY_RESIZE:
      buf.window.update_window_size();
      break;
    case KEY_LEFT:
      if (buf.cursor_x > 0) {
        buf.cursor_x -= 1;
      }
      break;
    case KEY_RIGHT:
      buf.cursor_x += 1;
      break;

    case KEY_UP:
      if (buf.cursor_y > 0) {
        buf.cursor_y -= 1;
      }
      break;
    case KEY_DOWN:
      if (buf.cursor_y < buf.lines.size() - 1) {
        buf.cursor_y += 1;
      }
      break;
    default:
      break;
    }

    buf.full_screen_render();
  }
}

int main(int argc, char *argv[]) {
  initscr();
  Buffer test = Buffer();
  ncurses_loop(test);
  return 0;
  endwin();
}
