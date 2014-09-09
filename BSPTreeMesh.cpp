#include "BSPTreeMesh.h"


/**
 * @brief BSPTreeMesh::BSPTreeMesh
 */
BSPTreeMesh::BSPTreeMesh()
{
}



/**
 * @brief BSPTreeMesh::draw
 */
void BSPTreeMesh::draw()
{
    // Nothing
    Mesh::draw ();
}



/**
 * @brief BSPTreeMesh::createBSPTree
 */
void BSPTreeMesh::createBSPTree ()
{

}


/**
 * @brief BSPTreeMesh::load_OFF
 * @param filename
 */
void BSPTreeMesh::load_OFF(const std::string &filename)
{
    Mesh::load_OFF (filename);

    createBSPTree ();
}
