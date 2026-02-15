#pragma once

#include "./utils/basic.cpp"
#include "../buffer.cpp"

enum Submode {
  INSERT,
  COMMAND,
};

Submode sm = INSERT;

// Mode definitions
class Basic : public Mode {

  // INSERT local variables
  string original_first_line;
  string original_second_line;
  int original_cursor_y;
  int original_cursor_x;

  // INSERT Submode
  void insert_key_press(int ch, Buffer &buf) {
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

    // Ctrl + a
    case 1:
      original_first_line = buf.lines[buf.window.first_line].full_string;
      original_second_line = buf.lines[buf.window.first_line + 1].full_string;
      original_cursor_y = buf.cursor_y;
      original_cursor_x = buf.cursor_x;

      buf.cursor_y = buf.window.first_line;
      buf.cursor_x = 3;

      buf.lines[buf.window.first_line].full_string = ">> ";
      buf.lines[buf.window.first_line + 1].full_string = "";

      buf.lines[buf.window.first_line].wrap_lines(buf.window);
      buf.lines[buf.window.first_line + 1].wrap_lines(buf.window);
      sm = COMMAND;
      break;

    default:
      BasicUtils::insert_character(buf, ch);
      break;
    }
  }

  // COMMAND Submode
  void command_key_press(int ch, Buffer &buf) {
    switch (ch) {
    case KEY_BACKSPACE:
      BasicUtils::delete_character_no_backline(buf, 3);
      break;

    case '\t':
    case '\n':
    case '\r':
    case KEY_UP:
    case KEY_DOWN:
      break;
    case KEY_RESIZE:
      buf.window.update_window_size();
      buf.rewarp_everything();
      break;
    case KEY_LEFT:
      if (buf.cursor_x > 3) {
        buf.cursor_x -= 1;
      }
      break;
    case KEY_RIGHT:
      buf.cursor_x += 1;
      break;

    // Ctrl + a
    case 1:
      buf.lines[buf.window.first_line].full_string = original_first_line;
      buf.lines[buf.window.first_line + 1].full_string = original_second_line;

      buf.cursor_y = original_cursor_y;
      buf.cursor_x = original_cursor_x;

      buf.lines[buf.window.first_line].wrap_lines(buf.window);
      buf.lines[buf.window.first_line + 1].wrap_lines(buf.window);
      sm = INSERT;
      break;

    default:
      BasicUtils::insert_character(buf, ch);
      break;
    }
  }

  void handle_key_press(int ch, Buffer &buf) override {
    switch (sm) {
    case INSERT:
      insert_key_press(ch, buf);
      break;
    case COMMAND:
      command_key_press(ch, buf);
      break;
    }
  };
};
