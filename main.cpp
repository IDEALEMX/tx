// some bs, go one commit back for actual change
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
             "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
             window));
    lines.push_back(Line("mi", window));
    lines.push_back(Line("amigo", window));
    lines.push_back(Line("", window));
  }

  void delete_character() {}

  void text_render() {
    int current_line = window.first_line;
    int current_screen_line = 0;

    while (current_screen_line < window.window_h &&
           current_line < lines.size()) {

      for (auto &line : lines.at(current_line).wrapped_lines) {
        Ncurses::move_and_write_to(current_screen_line, LEFT_SIDE_PADDING,
                                   line);
        current_screen_line++;
      }
      current_line++;
    }
  }

  void cursor_render() {
    int current_line = window.first_line;
    int current_screen_line = 0;

    while (current_screen_line < window.window_h &&
           current_line < lines.size()) {

      if (current_line == cursor_y) {

        // Clamp cursor
        if (cursor_x > lines[current_line].full_string.size()) {
          cursor_x = lines[current_line].full_string.size();
        }

        int wrapped_line = cursor_x / window.padded_size();
        int x_position = cursor_x % window.padded_size();

        Ncurses::move_cursor(current_screen_line + wrapped_line,
                             x_position + LEFT_SIDE_PADDING);

        return;
      }

      for (auto &line : lines.at(current_line).wrapped_lines) {
        current_screen_line++;
      }

      current_line++;
    }
  }

  void full_screen_render() {
    text_render();
    cursor_render();
  }
};

void ncurses_loop(Buffer &buf) {
  keypad(stdscr, TRUE);

  while (true) {
    buf.full_screen_render();

    int ch = Ncurses::get_input_character();

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
  }
}

int main(int argc, char *argv[]) {
  initscr();
  Buffer test = Buffer();
  ncurses_loop(test);
  return 0;
  endwin();
}
