#pragma once

#include "./utils/basic.cpp"
#include "../buffer.cpp"

// Mode definitions
class Basic : public Mode {

  void handle_key_press(int ch, Buffer &buf) override {

    switch (ch) {
    case KEY_BACKSPACE:
      BasicUtils::delete_character(buf);
      break;

    case '\t':
      BasicUtils::insert_character(buf, ' ');
      break;
    case '\n':
    case '\r': {
      BasicUtils::insert_new_line(buf, ch);
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
      BasicUtils::insert_character(buf, ch);
      break;
    }
  };
};
