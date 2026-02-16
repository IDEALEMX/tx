#pragma once

#include "./utils/basic.cpp"
#include "../buffer.cpp"
#include "./utils/file_origin.cpp"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <string>

enum Submode {
  INSERT,
  COMMAND,
};

Submode sm = INSERT;

// Mode definitions
class Basic : public Mode {
public:
  // Constructor variables
  FileOrigin origin;
  Basic(std::string relative_origin_path_input)
      : origin(relative_origin_path_input) {};

  // INSERT local variables
  string original_first_line;
  string original_second_line;
  int original_cursor_y;
  int original_cursor_x;
  const string prompt_prefix = ">> ";

  // INSERT Submode
  void insert_key_press(int ch, Buffer &buf, bool &exit) {
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
      buf.lines[buf.cursor_y].wrap_lines(buf.window);
      break;
    case KEY_RIGHT:
      buf.cursor_x += 1;
      buf.lines[buf.cursor_y].wrap_lines(buf.window);
      break;

    case KEY_UP:
      if (buf.cursor_y > 0) {
        buf.cursor_y -= 1;
      }
      buf.lines[buf.cursor_y].wrap_lines(buf.window);
      break;
    case KEY_DOWN:
      if (buf.cursor_y < buf.lines.size() - 1) {
        buf.cursor_y += 1;
      }
      buf.lines[buf.cursor_y].wrap_lines(buf.window);
      break;

    // Ctrl + a
    case 1:
      original_first_line = buf.lines[buf.window.first_line].full_string;
      if (buf.lines.size() > 1) {
        original_second_line = buf.lines[buf.window.first_line + 1].full_string;
      }
      original_cursor_y = buf.cursor_y;
      original_cursor_x = buf.cursor_x;

      buf.cursor_y = buf.window.first_line;
      buf.cursor_x = 3;

      buf.lines[buf.window.first_line].full_string = prompt_prefix;
      if (buf.lines.size() > 1) {
        buf.lines[buf.window.first_line + 1].full_string = "";
      }

      buf.lines[buf.window.first_line].wrap_lines(buf.window);
      if (buf.lines.size() > 1) {
        buf.lines[buf.window.first_line + 1].wrap_lines(buf.window);
      }
      sm = COMMAND;
      break;

    default:
      BasicUtils::insert_character(buf, ch);
      break;
    }
  }

  // COMMAND Submode
  string result_msg;

  string handle_cmd(Buffer &buf, string cmd, bool &exit) {
    if (cmd == "save") {
      origin.save(buf);
      return "File saved to " + origin.origin_path.string();
    } else if (cmd == "exit") {
      exit = true;
      return "Exiting";
    }
    return "Unknown command!";
  }

  void command_key_press(int ch, Buffer &buf, bool &exit) {
    switch (ch) {
    case KEY_BACKSPACE:
      BasicUtils::delete_character_no_backline(buf, 3);
      break;
      // Execute command
    case '\n':
    case '\r': {
      // Parse command
      string cmd = buf.lines[buf.window.first_line].full_string.substr(
          prompt_prefix.size());
      boost::trim_right(cmd);
      boost::trim_left(cmd);

      if (buf.lines[buf.window.first_line].full_string.compare(
              0, prompt_prefix.size(), prompt_prefix) == 0) {
        // Restore
        buf.lines[buf.window.first_line].full_string = original_first_line;
        if (buf.lines.size() > 1) {
          buf.lines[buf.window.first_line + 1].full_string =
              original_second_line;
        }

        buf.lines[buf.window.first_line].full_string =
            handle_cmd(buf, cmd, exit);
        buf.lines[buf.window.first_line].wrap_lines(buf.window);
      }
    }

    // Ctrl + a
    case 1:
      buf.lines[buf.window.first_line].full_string = original_first_line;

      if (buf.lines.size() > 1) {
        buf.lines[buf.window.first_line + 1].full_string = original_second_line;
      }

      buf.cursor_y = original_cursor_y;
      buf.cursor_x = original_cursor_x;

      if (buf.lines.size() > 1) {
        buf.lines[buf.window.first_line + 1].wrap_lines(buf.window);
      }
      sm = INSERT;
      break;

    case KEY_UP:
    case KEY_DOWN:
    case '\t':
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

    default:
      BasicUtils::insert_character(buf, ch);
      break;
    }
  }

  void handle_key_press(int ch, Buffer &buf, bool &exit) override {
    switch (sm) {
    case INSERT:
      insert_key_press(ch, buf, exit);
      break;
    case COMMAND:
      command_key_press(ch, buf, exit);
      break;
    }
  };
};
