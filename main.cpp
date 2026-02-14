#include <cassert>
#include <fstream>
#include <memory>
#include <string>

// Local includes
#include "./providers.cpp"

// CONFIGURATION
// > 3
int LEFT_SIDE_PADDING = 4;
// > 1
int RIGHT_SIDE_PADDING = 2;

using namespace std;

#include "./line_and_window.cpp"
#include "./modes.cpp"

void ncurses_loop(Buffer &buf) {
  keypad(stdscr, TRUE);

  while (true) {
    buf.full_screen_render();

    int ch = Ncurses::get_input_character();

    buf.handle_key_press(ch);
  }
}

int main(int argc, char *argv[]) {
  initscr();
  Buffer test = Buffer(make_unique<Basic>());

  // Verify a file was input-ed (for now)
  if (argc == 1) {
    endwin();
    return 1;
  }

  ifstream in(argv[1]);

  string line;
  while (getline(in, line)) {
    test.lines.push_back(Line(line, test.window));
  }

  ncurses_loop(test);

  return 0;
  endwin();
}
