/*
 * TODO
 * optimize everything by:
 * 1) start at 0,0
 * 2) keep track of current line
 * 3] optimize by only re-warping when necessary and the lines that are
 * necessary
 */

#include "ncurses.h"
#include <cassert>
#include <string>
#include <vector>
using namespace std;

// General configuration variables
int LEFT_SIDE_PADDING = 4;
// Most be at least 1!!!
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

class Line {
public:
  string full_string;
  vector<string> wrapped_lines;

  void wrap_lines(int window_w) {

    // TODO equal the distance tolerance
    if (window_w <= 1) {
      return;
    }

    int padded_size = window_w - LEFT_SIDE_PADDING - RIGHT_SIDE_PADDING;

    int wrapped_lines_number =
        Computation::ceil_int_div(full_string.length(), padded_size);
    wrapped_lines.clear();
    if (wrapped_lines_number == 0) {
      wrapped_lines_number = 1;
    }
    for (int i = 0; i < wrapped_lines_number; i++) {
      string line = full_string.substr(padded_size * i, padded_size);
      wrapped_lines.push_back(line);
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
             window.window_w));
    lines.push_back(Line("mi", window.window_w));
    lines.push_back(Line("amigo", window.window_w));
  }

  void delete_character() {
    if (lines.empty() || cursor_y < 0 || cursor_y >= lines.size()) {
      return;
    }

    if (cursor_y == 0 && cursor_x == 0) {
      return;
    }

    if (cursor_x == 0) {
      // TODO
      return;
    }

    string text_line = lines[cursor_y].full_string;

    if (text_line.size() == 0) {
      lines.erase(lines.begin() + cursor_y);
      if (cursor_y > 0) {
        cursor_y--;
        cursor_x = 10000;
      }
    } else {
      assert(cursor_x - 1 >= 0);
      lines[cursor_y].full_string.erase(cursor_x - 1, 1);
      cursor_x--;
    }
  }

  void full_screen_render() {
    Ncurses::clear_full_screen();

    int screen_line = 0;
    int line_number = window.first_line;
    while (screen_line < window.window_h) {
      if (lines.size() <= line_number) {
        break;
      }

      auto &current_line = lines[line_number];

      if (LEFT_SIDE_PADDING > 3) {
        Ncurses::move_and_write_to(screen_line, 0, to_string(line_number));
      }

      for (int wrapped_line_num = 0;
           wrapped_line_num < current_line.wrapped_lines.size();
           wrapped_line_num++) {

        Ncurses::move_and_write_to(
            screen_line, LEFT_SIDE_PADDING,
            current_line.wrapped_lines[wrapped_line_num]);

        screen_line++;
      }
      line_number++;
    }
    Ncurses::refresh_screen();
  }

  void draw_cursor_in_screen() {
    int screen_line = 0;
    int line_number = window.first_line;

    if (window.padded_size() <= 1) {
      return;
    }

    // Find screen location in
    while (screen_line < window.window_h && line_number < lines.size()) {
      if (line_number == cursor_y) {

        if (cursor_x > lines[line_number].full_string.size()) {
          cursor_x = lines[line_number].full_string.size();
        }

        int wrapped_line = (cursor_x) / (window.padded_size());
        int x_possition = (cursor_x) % (window.padded_size());

        if (x_possition >
            lines[line_number].wrapped_lines[wrapped_line].size()) {
          x_possition = lines[line_number].wrapped_lines[wrapped_line].size();
        }

        Ncurses::move_cursor(screen_line + wrapped_line,
                             x_possition + LEFT_SIDE_PADDING);
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

    int lines_sum = 0;
    switch (ch) {
    case KEY_BACKSPACE:
      buf.delete_character();
      buf.window.update_window_size();
      for (auto &line : buf.lines) {
        line.wrap_lines(buf.window.window_w);
      }
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
