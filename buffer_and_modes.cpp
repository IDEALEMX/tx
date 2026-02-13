#pragma once
#include "./line_and_window.cpp"
#include <memory>

using namespace std;

class Buffer;

class Mode {
public:
  virtual ~Mode() = default;
  virtual void handle_key_press(int ch, Buffer &buf) = 0;
};

class Buffer {
public:
  // full text buffer separated into lines
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

    // Scratch file case
    cursor_x = 0;
    cursor_y = 0;

    window = Window();
    lines.push_back(Line("", window));
  }

  void handle_key_press(int ch) { current_mode->handle_key_press(ch, *this); }

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

// Mode definitions
class Basic : public Mode {

  void delete_character(Buffer &buf) {
    if (buf.cursor_x == 0) {
      if (buf.cursor_y == 0) {
        return;
      }

      buf.cursor_x = buf.lines.at(buf.cursor_y - 1).full_string.size();
      buf.lines.at(buf.cursor_y - 1).full_string +=
          buf.lines.at(buf.cursor_y).full_string;
      buf.lines.erase(buf.lines.begin() + buf.cursor_y);
      buf.cursor_y--;
      buf.rewarp_everything();
      return;
    }

    buf.lines[buf.cursor_y].full_string.erase(buf.cursor_x - 1, 1);
    buf.cursor_x--;

    buf.lines[buf.cursor_y].wrap_lines(buf.window);
  }

  void insert_new_line(Buffer &buf, int ch) {
    std::string &current = buf.lines[buf.cursor_y].full_string;

    std::string right = current.substr(buf.cursor_x);
    current = current.substr(0, buf.cursor_x);

    buf.lines.insert(buf.lines.begin() + buf.cursor_y + 1,
                     Line(right, buf.window));

    buf.cursor_y++;
    buf.cursor_x = 0;

    buf.rewarp_everything();
  }

  void insert_character(Buffer &buf, int ch) {
    auto &line = buf.lines.at(buf.cursor_y).full_string;
    line.insert(buf.cursor_x, 1, ch);
    buf.cursor_x++;
    buf.rewarp_everything();
  }

  void handle_key_press(int ch, Buffer &buf) override {

    switch (ch) {
    case KEY_BACKSPACE:
      delete_character(buf);
      break;

    case '\t':
      insert_character(buf, ' ');
      insert_character(buf, ' ');
      insert_character(buf, ' ');
      insert_character(buf, ' ');
      break;
    case '\n':
    case '\r': {
      insert_new_line(buf, ch);
      break;
    }
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
      insert_character(buf, ch);
      break;
    }
  };
};
