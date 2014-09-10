#include <functional>
#include <iostream>
#include <vector>

#include "BSPTreeMesh.h"


/**
 * @brief BSPTreeMesh::BSPTreeMesh
 */
BSPTreeMesh::BSPTreeMesh()
{
}


/**
 * @brief BSPTreeMesh::save salva il BSPTree
 * @param filename path di salvataggio
 * @return true se l'operazione e' andata a buon fine
 */
bool BSPTreeMesh::save (const std::string &filename)
{
    // Il file designato e' filename + ".tree"
    return false;
}


/**
 * @brief BSPTreeMesh::load carica il BSPTree
 * @param filename path di caricamento
 * @return true se l'operazione e' andata a buon fine
 */
bool BSPTreeMesh::load (const std::string &filename)
{
    // Il file designato e' filename + ".tree"
    return false;
}


/**
 * @brief BSPTreeMesh::draw
 */
void BSPTreeMesh::draw(/* QUI CI SARA IL PUNTO DI VISTO (SE MI LASCIATE LA LIBERTA') */)
{
    // Nothing, per ora uso la funzione tradizionale
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof (Vertex), (GLvoid*)(&V()[0].p));
    glNormalPointer(GL_FLOAT, sizeof (Vertex), (GLvoid*)(((float*)&V()[0].p) + 3));
    glDrawElements(GL_TRIANGLES, 3 * T().size(), GL_UNSIGNED_INT, (GLvoid*)(&T()[0]));
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}



/**
 * @brief BSPTreeMesh::createBSPTree
 */
void BSPTreeMesh::createBSPTree ()
{
    mBSPTreeRoot = _createBSPTree (T());
}



std::vector<Mesh::Triangle> leftTo (Mesh::Triangle t, std::vector<Mesh::Triangle> s)
{
    std::vector<Mesh::Triangle> tt;
    return tt;
}

std::vector<Mesh::Triangle> rightTo (Mesh::Triangle t, std::vector<Mesh::Triangle> s)
{
    std::vector<Mesh::Triangle> tt;
    return tt;
}

std::vector<Mesh::Triangle> centerTo (Mesh::Triangle t, std::vector<Mesh::Triangle> s)
{
    std::vector<Mesh::Triangle> tt;
    return tt;
}

/**
 * @brief BSPTreeMesh::_createBSPTree funzione ricorsiva di creazione del bsptree
 * @param s triangoli della partizione
 * @return
 */
BSPNode* BSPTreeMesh::_createBSPTree (std::vector<Triangle> s)
{
    if (s.size() <= 1)
    {
        if (s.size() == 0)
            return NULL;

        BSPLeafNode* leaf = new BSPLeafNode ();
        leaf->NodeTriangle = s.at(0);
        return leaf;
    }
    else
    {
        Triangle subdivisionPlane = s.at(0);
        BSPInternalNode* internal = new BSPInternalNode ();
        std::vector<Triangle> leftTriangles = leftTo (subdivisionPlane, s);
        std::vector<Triangle> rightTriangles = rightTo (subdivisionPlane, s);
        std::vector<Triangle> centerTriangles = centerTo (subdivisionPlane, s);

        internal->Left = _createBSPTree (leftTriangles);
        internal->Right = _createBSPTree (rightTriangles);
        internal->NodeTriangles = centerTriangles;

        return internal;
    }
}




/**
 * @brief BSPTreeMesh::load_OFF
 * @param filename
 */
void BSPTreeMesh::load_OFF(const std::string &filename)
{
    Mesh::load_OFF (filename);

    /* Prova a caricare il bsptree */
    if (load (filename))
    {

    }
    /* Altrimenti lo genero */
    else
    {
        createBSPTree ();
        save (filename);
    }
}
