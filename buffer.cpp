#pragma once

#include "./line_and_window.cpp"
#include <memory>

class Buffer;

class Mode {
public:
  virtual ~Mode() = default;
  virtual void handle_key_press(int ch, Buffer &buf, bool &exit) = 0;
};
using namespace std;

class Buffer {
public:
  // Full text buffer separated into lines
  vector<Line> lines;

  Window window;

  // Cursor x,y
  int cursor_x;
  int cursor_y;

  unique_ptr<Mode> current_mode;

  void set_mode(unique_ptr<Mode> new_mode) {
    current_mode = std::move(new_mode);
  }

  Buffer(unique_ptr<Mode> initial_mode) {

    current_mode = std::move(initial_mode);

    cursor_x = 0;
    cursor_y = 0;

    window = Window();
  }

  void handle_key_press(int ch, bool &exit) {
    current_mode->handle_key_press(ch, *this, exit);
  }

  void updute_window_position() {}

  void text_render() {
    updute_window_position();

    // Case: mouse before screen
    if (window.first_line > cursor_y) {
      window.first_line = cursor_y;
    }

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
    int lines_in_screen = current_line - window.first_line;

    if (window.first_line + lines_in_screen - 1 < cursor_y) {
      window.first_line = cursor_y - lines_in_screen + 1;
      text_render();
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
