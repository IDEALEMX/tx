# I'm too lazy to actually bother making a proper make file lol,
# this compilation is intended for arch linux, but setting up an alternative
# for your os of choice shouldn't be too hard.
#
# Required:
# 1. Clang
# 2. N-curses

clang++ -fdiagnostics-color=always -fcaret-diagnostics ./main.cpp -lncurses
