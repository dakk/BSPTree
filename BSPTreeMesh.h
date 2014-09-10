#ifndef BSPTREEMESH_H
#define BSPTREEMESH_H

#include <vector>

#include "Vec3D.h"
#include "Mesh.h"


/**
 * @brief Un generico nodo del BSPTree; in questo modo, ereditando da questa
 * classe posso definire nodi interni e foglie, ed uso il tipo BSPNode come
 * tipo per i puntatori ai figli (in modo da includere sia le foglie che i ndoi interni)
 */
class BSPNode
{
};

/**
 * @brief Un nodo interno del BSPTree
 *
 * Il piano di suddivisione, puo' essere definito da 3 punti; utilizzo quindi il primo
 * triangolo della lista dei triangoli come nodo di suddivisione.
 */
class BSPInternalNode : public BSPNode
{
public:
    BSPNode*                      Left;             /**< Figlio sinistro */
    BSPNode*                      Right;            /**< Figlio destro */
    std::vector<Mesh::Triangle>   NodeTriangles;    /**< Triangoli nel piano di taglio */
};

/** @brief Un nodo foglia BSPTree */
class BSPLeafNode : public BSPNode
{
public:
    Mesh::Triangle        NodeTriangle;          /**< Triangolo della foglia */
};



/**
 * @brief The BSPTreeMesh class
 */
class BSPTreeMesh : public Mesh
{
public:
    /* Metodi e attributi */
            BSPTreeMesh    ();

    void    draw           ();
    void    load_OFF       (const std::string &filename);

    bool    save           (const std::string &filename);
    bool    load           (const std::string &filename);


protected:
    void        createBSPTree   ();
    BSPNode*    mBSPTreeRoot;

private:
    BSPNode*    _createBSPTree  (std::vector<Triangle> s);
};

#endif // BSPTREEMESH_H
