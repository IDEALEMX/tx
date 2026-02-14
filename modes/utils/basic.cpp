#pragma once

#include "../../buffer.cpp"

class BasicUtils {
public:
  static void delete_character(Buffer &buf) {
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

  static void insert_new_line(Buffer &buf, int ch) {
    std::string &current = buf.lines[buf.cursor_y].full_string;

    std::string right = current.substr(buf.cursor_x);
    current = current.substr(0, buf.cursor_x);

    buf.lines.insert(buf.lines.begin() + buf.cursor_y + 1,
                     Line(right, buf.window));

    buf.cursor_y++;
    buf.cursor_x = 0;

    buf.rewarp_everything();
  }

  static void insert_character(Buffer &buf, int ch) {
    auto &line = buf.lines.at(buf.cursor_y).full_string;
    line.insert(buf.cursor_x, 1, ch);
    buf.cursor_x++;
    buf.rewarp_everything();
  }
};
