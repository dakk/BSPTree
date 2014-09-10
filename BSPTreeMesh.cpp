#include <functional>
#include <iostream>
#include <vector>
#include <qmatrix4x4.h>

#include "BSPTreeMesh.h"
#include "Mesh.h"


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
    std::cout << "yea." << endl;
}




double BSPTreeMesh::determinant (Triangle t, Vertex v)
{
    QMatrix4x4 coplanare;

    std::cout << t[0] << " " << t[1] << " " << t[2] << "\n" << std::flush;

    Vertex v1 = V()[t[0]];
    Vertex v2 = V()[t[1]];
    Vertex v3 = V()[t[2]];
    coplanare.setRow(0, QVector4D (v1.p[0], v1.p[1], v1.p[2], 1));
    coplanare.setRow(1, QVector4D (v2.p[0], v2.p[1], v2.p[2], 1));
    coplanare.setRow(2, QVector4D (v3.p[0], v3.p[1], v3.p[2], 1));
    coplanare.setRow(3, QVector4D (v.p[0], v.p[1], v.p[2], 1));

    return coplanare.determinant();
}

/**
 * @brief triangleRespectToPlane
 * @param t
 * @param subPlane
 * @return
 */
BSPTreeMesh::Position BSPTreeMesh::triangleRespectToPlane (Triangle t, Triangle subPlane)
{
    Vertex v1 = V()[t[0]];
    Vertex v2 = V()[t[1]];
    Vertex v3 = V()[t[2]];
    double d1 = determinant (subPlane, v1);
    double d2 = determinant (subPlane, v2);
    double d3 = determinant (subPlane, v3);

    if (d1 < 0.0 && d2 < 0.0 && d3 < 0.0)
        return POS_LEFT;
    else if (d1 > 0.0 && d2 > 0.0 && d3 > 0.0)
        return POS_RIGHT;
    else if (d1 == 0 && d2 == 0 && d3 == 0)
        return POS_CENTER;
    else
        return POS_INTERSECT;
}


int a = 0;

/**
 * @brief BSPTreeMesh::_createBSPTree funzione ricorsiva di creazione del bsptree
 * @param s triangoli della partizione
 * @return
 */
BSPNode* BSPTreeMesh::_createBSPTree (std::vector<Triangle> s)
{
    if (s.size() <= 1)
    {
        /* Nessun triangolo nel vettore */
        if (s.size() == 0)
            return NULL;

        /* Un triangolo nel vettore, creo una foglia */
        BSPLeafNode* leaf = new BSPLeafNode ();
        leaf->NodeTriangle = s.at(0);
        a++;
        std::cout << a << "\n";
        return leaf;
    }
    /* Piu' triangoli nel vettore, suddivo gli insiemi e richiamo la funzione per i figli */
    else
    {
        std::vector<Triangle> leftTriangles;
        std::vector<Triangle> rightTriangles;
        std::vector<Triangle> centerTriangles;

        Triangle subdivisionPlane = s.at(0);
        BSPInternalNode* internal = new BSPInternalNode ();


        a++;
        std::cout << a << "\n";


        for (unsigned i=0; i<s.size(); i++)
        {
            Position pos = triangleRespectToPlane (s.at(i), subdivisionPlane);

            switch (pos)
            {
            case POS_LEFT:
                leftTriangles.push_back(s.at(i));
                break;

            case POS_CENTER:
                centerTriangles.push_back(s.at(i));
                break;

            case POS_RIGHT:
                rightTriangles.push_back(s.at(i));
                break;

            default://intersection per ora li cravo nel nodo torrente
                centerTriangles.push_back(s.at(i));
                break;
            }
        }

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
void BSPTreeMesh::load_OFF (const std::string &filename)
{
    std::cout << "load_off"<<endl;
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
