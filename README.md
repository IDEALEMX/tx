# TX
## What is it
Tx is a small text editor written in c++, with a focus on being simple, fast
and simple at its core, allowing for very manual and fine tuned configuration.
  
One important thing to note is that TX is configured not via a config file but
by extending the c++ code via "module" classes and recompiling.
This is an intended feature and will not be changed, as it allows for the
highest level of control over the editor, and, assuming a llvm based compiler
is used, it should allow for the use of a large number of programming languages
for configuration without issue,  
This still hasn't been tested tho, and may not function as the project
currently stands.

## Basic mode
Basic mode is the default mode in Tx, it can be fully modified and rewritten,
however, it may serve as a nice blueprint for how extending should work.

Basic features: 
- Movement Using arrow keys
- Typing
- Command prompt (activated with Ctrl + A)
- Default commands:
  - save: for saving a project.
  - exit: for exiting.

## Current state
### v. 0.1
First version of the project still very small and minimal, more of a proof of
concept than anything.
Code is still unoptimized, and structure will still almost certainly change,
but the core idea is there.

Please report any issues!
And thanks for stopping by :3

