BSPTree
===
Generate and render the bsp tree of a 3d mesh (Qt/OpenGL).


Dependencies
---
- Qt 5.2 or greater
- OpenGL (tested on mesa-gl)
- QtCreator


Multithreading
---
The software implement concurrent loading of mesh; if this feature give problems on another
version of qt or another os, disable it commenting "#define THREADING" on window_gl.cpp.


To do:
---
- Implement load() and save() for BSPTreeMesh