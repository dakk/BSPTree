#ifndef BSPTREEMESH_H
#define BSPTREEMESH_H

#include "Mesh.h"

/**
 * @brief The BSPTreeMesh class
 */
class BSPTreeMesh : public Mesh
{
public:
    /** @brief Nodo del BSP Tree */
    typedef struct
    {
    } BSPNode;


            BSPTreeMesh    ();

    void    draw           ();
    void    load_OFF       (const std::string &filename);


protected:
    void    createBSPTree  ();
};

#endif // BSPTREEMESH_H
