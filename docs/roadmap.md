# Roadmap
## What is this file
This file presents a general overview of the features & changes that are intended to be implemented in version **v0.2.0** 
## Changes    
the following changes are ordered by intended implementation order.  

### Migrate from # pragma once to header model for file organization  
- [ ] Implemented?

### Revamp rendering flow 
- [ ] Implemented?
    
Rendering is now intended to be sperated into two different stages:
1. **buffer_changes**
Any changes to the buffer state will be performed at this point.
2. **pre_rendering**
In this stage the real buffer will be transformed into a form that may be displayed onto the screen, this is called the virtual_buffer.
3. **virtual_buffer**
In this stage, the n number of modes attached to the buffer will modify the virtual_buffer, allowing for visual enhancements that will not affect the real content of the buffer.
4. **rendering**
Once all necessary changes have been completed in the virtual_buffer, it will be resized if necessary (in case new lines where added) and will be displayed to the user.

### Revamp entire project architecture
- [ ] Implemented?
  
This will be explained on a different document for the sake of practicality.
See docs/architecture.md

### Expand std lib   
for a more robust default editing experience.  
- [ ] Implemented

