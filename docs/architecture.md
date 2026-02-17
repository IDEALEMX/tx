# Architecture
## What is this document?
This document presents an in-depth explanation of the intended project architecture as of v0.2.0 
[!NOTE]
> The implementation of this architecture model is still in development

## Elements

### Dir structure
The following contains only the directories relevant to the topic at hand

- tx/
    - core/ 
    - modes/
        - std/
        - ModeExample/
            - ModeExample.cpp
    - helpers/
        - std/
        - HelperExample/
            - HelperExample.cpp
    - utils/
        - std/
        - UtilExample/
            - UtilExample.cpp
    - init.cpp

### Program general flow
#### init.cpp
The init.cpp file exports a single function, init(), which allows the user to: 
    1. Define any logic that should take place at **the start and end** of the program.
    2. Define any logic related to the commandline arguments.
    3. Define the logic responsible for creating the init buffer instance.
The final point will be expanded more in depth later.

#### buffer
The buffer class as the name may imply, has as its main responsibility to store piece of text in memory so that it can later be:
* Displayed on the screen
* Used by other elements of the program
* Retrieved when necessary

Said buffer has the following traits & methods that may be used by the user:

##### cursor_x, cursor_y
As the name may imply, these manage the position of the cursor inside the buffer.
> [!IMPORTANT]
> These manage the position of the cursor **relative to the text inside the buffer** and **not to the screen**, meaning that the same position in the same line of the same text could result in a different screen position depending on where it is rendered

##### lines
This variable, of the type vector<string>, is the very core of the buffer.
It holds the actual text contains of the buffer as a list of lines, whose implementation now follows.

### State and the StateMap
In general you probably want to avoid state beyond that of the buffer being handled, however, in case it is necessary for a given extension, all state must be handled via the global StateMap instance with StateNodes.

> [!NOTE]
> Todo: explain the API for the state map in detail

Example: "The marker plugin keeps track of which open buffers are marked to list them to the user when called"
For this assumption, as should probably be the standard, we will be working with the following init structure:

StateMap:
- FileBufferNode
    - MarkedNode: bool



### Node

Node{
    
};


