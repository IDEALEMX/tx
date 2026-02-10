#include "ncurses.h"
#include <string>
#include <vector>
using namespace std;

// General configuration variables
int SIDE_NUMBER_PADDING = 3;

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
    mvprintw(y, x, content.c_str());
  }
  static void clear_full_screen() { clear(); };
};

class Line {
public:
  string full_string;
  vector<string> wrapped_lines;

  void wrap_lines(int window_w) {

    if (window_w <= 1) {
      return;
    }

    int wrapped_lines_number =
        Computation::ceil_int_div(full_string.length(), window_w);
    wrapped_lines.clear();
    for (int i = 0; i < wrapped_lines_number; i++) {
      wrapped_lines.push_back(full_string.substr(window_w * i, window_w));
    }
  }

  Line(string initial_string, int initial_window_w) {
    full_string = initial_string;
    wrap_lines(initial_window_w);
  }
};

class Window {
public:
  int window_h;
  int window_w;
  int line_offset;

  void update_window_size() {
    getmaxyx(stdscr, window_h, window_w);
    window_w -= SIDE_NUMBER_PADDING;
  }

  Window() {
    update_window_size();
    line_offset = 0;
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
    cursor_x = 1;
    cursor_y = 0;

    window = Window();
    lines.push_back(
        Line("holaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
             "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
             "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
             "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
             "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
             "aaaaaaaaaaaaaaaaaaaaaaaaa",
             window.window_w));
    lines.push_back(Line("mi", window.window_w));
    lines.push_back(Line("amigo", window.window_w));
  }

  void full_screen_render() {
    Ncurses::clear_full_screen();
    int screen_line = 0;
    int line_number = 0;
    while (screen_line < window.window_h) {
      if (lines.size() <= line_number) {
        break;
      }

      auto &current_line = lines[line_number];

      if (SIDE_NUMBER_PADDING != 0) {
        Ncurses::move_and_write_to(screen_line, 0, to_string(line_number));
      }

      for (int wrapped_line_num = 0;
           wrapped_line_num < current_line.wrapped_lines.size();
           wrapped_line_num++) {

        Ncurses::move_and_write_to(
            screen_line, SIDE_NUMBER_PADDING,
            current_line.wrapped_lines[wrapped_line_num]);

        screen_line++;
      }
      line_number++;
    }
    Ncurses::refresh_screen();
  }

  void draw_cursor_in_screen() {
    int screen_line = 0;
    int line_number = 0;

    if (window.window_w <= SIDE_NUMBER_PADDING) {
      return;
    }

    // Find screen location in
    while (screen_line < window.window_h) {
      if (line_number == cursor_y) {

        if (cursor_x > lines[line_number].full_string.size()) {
          cursor_x = lines[line_number].full_string.size();
        }

        int wrapped_line = (cursor_x) / (window.window_w - SIDE_NUMBER_PADDING);
        int x_possition = (cursor_x) % (window.window_w - SIDE_NUMBER_PADDING);

        // The actual x_possition of the mouse in memory won't change
        // this this condition will need to be applied when inserting to append
        // to the end.
        if (x_possition >
            lines[line_number].wrapped_lines[wrapped_line].size()) {
          x_possition = lines[line_number].wrapped_lines[wrapped_line].size();
        }

        Ncurses::move_cursor(screen_line + wrapped_line,
                             x_possition + SIDE_NUMBER_PADDING);

        break;
      }

      auto &current_line = lines[line_number];
      for (int wrapped_line_num = 0;
           wrapped_line_num < current_line.wrapped_lines.size();
           wrapped_line_num++) {
        screen_line++;
      }
      line_number++;
    }
  }
};

void ncurses_loop(Buffer &buf) {
  keypad(stdscr, TRUE);
  buf.full_screen_render();
  buf.draw_cursor_in_screen();

  while (true) {
    int ch = Ncurses::get_input_character();

    switch (ch) {
    case KEY_BACKSPACE:
      break;
    case '\n':
    case '\r':
      break;
    case KEY_RESIZE:
      buf.window.update_window_size();
      for (auto &line : buf.lines) {
        line.wrap_lines(buf.window.window_w);
      }
      break;
    default:
      break;
    }

    buf.full_screen_render();
    buf.draw_cursor_in_screen();
  }
}

int main(int argc, char *argv[]) {
  initscr();
  Buffer test = Buffer();
  ncurses_loop(test);
  return 0;
  endwin();
}
