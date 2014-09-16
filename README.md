BSPTree
===
Generate and render the bsp tree of a 3d mesh (Qt/OpenGL).


To do:
---

1. Move BSPTreeMesh::createBSPTree () to another thread and create a signal to inform the mainthread that the 
	bsp creation has finished. It could be good to have a signal for percentage notify, but how?
	I can use number of triangle in S for the current recursion step as percentage (near 0 -> near 100%).
2. Solve point of view retriving