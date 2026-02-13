#include <cassert>
#include <string>
#include <vector>

// Local includes
#include "./providers.cpp"

using namespace std;

// CONFIGURATION
// > 3
int LEFT_SIDE_PADDING = 4;
// > 1
int RIGHT_SIDE_PADDING = 2;

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
    lines.push_back(Line("", window));
    lines.push_back(Line("", window));
    lines.push_back(Line("amigo", window));
    lines.push_back(Line("", window));
  }

  void delete_character() {
    if (cursor_x == 0) {
      if (cursor_y == 0) {
        return;
      }

      cursor_x = lines.at(cursor_y - 1).full_string.size();
      lines.at(cursor_y - 1).full_string += lines.at(cursor_y).full_string;
      lines.erase(lines.begin() + cursor_y);
      cursor_y--;
      rewarp_everything();
      return;
    }

    lines[cursor_y].full_string.erase(cursor_x - 1, 1);

    lines[cursor_y].wrap_lines(window);
  }

  void text_render() {
    int current_line = window.first_line;
    int current_screen_line = 0;

    Ncurses::clear_full_screen();

    while (current_screen_line < window.window_h &&
           current_line < lines.size()) {

      Ncurses::move_and_write_to(current_screen_line, 0,
                                 current_line == cursor_y
                                     ? " " + to_string(current_line)
                                     : to_string(current_line));

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

  // TODO must eventually implement "re-warp from up to screen"
  void rewarp_everything() {
    for (auto &line : lines) {
      line.wrap_lines(window);
    }
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
      buf.rewarp_everything();
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
