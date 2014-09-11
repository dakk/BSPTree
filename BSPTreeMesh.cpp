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
    Vertex pov;
    pov.p.init(0.0,0.0,0.0);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glVertexPointer(3, GL_FLOAT, sizeof (Vertex), (GLvoid*)(&V()[0].p));
    glNormalPointer(GL_FLOAT, sizeof (Vertex), (GLvoid*)(((float*)&V()[0].p) + 3));

    _draw (mBSPTreeRoot, pov);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}


/**
 * @brief BSPTreeMesh::_draw
 * @param root
 * @param pov
 */
void BSPTreeMesh::_draw (BSPNode *root, Vertex pov)
{
    if (root == NULL) return;

    if (BSPLeafNode* leaf = dynamic_cast<BSPLeafNode*>(root))
    {
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (GLvoid*)(&leaf->NodeTriangle));
    }
    else if (BSPInternalNode* internal = dynamic_cast<BSPInternalNode*>(root))
    {
        //std::cout << internal->NodeTriangles.size() << "  " << endl << std::flush;
        double det = determinant (internal->NodeTriangles[0], pov);
        Position pos = determinantToPosition(det);

        switch (pos)
        {
        case POS_RIGHT:
            _draw(internal->Left, pov);
            glDrawElements(GL_TRIANGLES, 3 * internal->NodeTriangles.size(),
                           GL_UNSIGNED_INT, (GLvoid*)(&internal->NodeTriangles[0]));
            _draw(internal->Right, pov);
            break;

        case POS_LEFT:
            _draw(internal->Right, pov);
            glDrawElements(GL_TRIANGLES, 3 * internal->NodeTriangles.size(),
                           GL_UNSIGNED_INT, (GLvoid*)(&internal->NodeTriangles[0]));
            _draw(internal->Left, pov);
            break;

        default:
            _draw(internal->Right, pov);
            _draw(internal->Left, pov);
            break;
        }
    }
}




/**
 * @brief BSPTreeMesh::determinantToPosition
 * @param d
 * @return
 */
BSPTreeMesh::Position BSPTreeMesh::determinantToPosition (double d)
{
    if (d < 0.0)
        return POS_LEFT;
    else if (d > 0.0)
        return POS_RIGHT;
    else
        return POS_CENTER;
}


/**
 * @brief BSPTreeMesh::determinant Calcola il determinante di un vertice rispetto ad un piano
 * @param t Piano
 * @param v Vertice da verificare
 * @return determinante
 */
double BSPTreeMesh::determinant (Triangle t, Vertex v)
{
    QMatrix4x4 coplanare;
    double det;

    Vertex v1 = V()[t[0]];
    Vertex v2 = V()[t[1]];
    Vertex v3 = V()[t[2]];
    coplanare.setRow(0, QVector4D (v1.p[0], v1.p[1], v1.p[2], 1));
    coplanare.setRow(1, QVector4D (v2.p[0], v2.p[1], v2.p[2], 1));
    coplanare.setRow(2, QVector4D (v3.p[0], v3.p[1], v3.p[2], 1));
    coplanare.setRow(3, QVector4D (v.p[0], v.p[1], v.p[2], 1));

    det = coplanare.determinant();

    /* Se il valore del determinante e' vicino allo zero, approssimo a zero */
    if (det <= DETERMINANT_ZERO_APPROX && det >= -DETERMINANT_ZERO_APPROX)
        return 0.0;
    else
        return det;
}



/**
 * @brief triangleRespectToPlane Calcola la posizione di un triangolo rispetto ad
 *                                  un piano di taglio
 * @param t Triangolo del quale effettuare il check
 * @param subPlane Piano di taglio considerato
 * @return Posizione di t rispetto a subPlane
 */
BSPTreeMesh::Position BSPTreeMesh::triangleRespectToPlane (Triangle t, Triangle subPlane)
{
    Vertex v1 = V()[t[0]];
    Vertex v2 = V()[t[1]];
    Vertex v3 = V()[t[2]];

    Position p1 = determinantToPosition(determinant (subPlane, v1));
    Position p2 = determinantToPosition(determinant (subPlane, v2));
    Position p3 = determinantToPosition(determinant (subPlane, v3));

    /* Se le posizioni dei punti del triangolo t rispetto al piano sono uguali,
     * rilascio la posizione comune */
    if (p1 == p2 && p2 == p3)
        return p1;
    /* Se non sono tutti uguali, ho un intersezione col piano di taglio */
    else
        return POS_INTERSECT;
}



/**
 * @brief BSPTreeMesh::createBSPTree
 */
void BSPTreeMesh::createBSPTree ()
{
    std::cout << "BSPTreeMesh::createBSPTree()" << std::endl << std::flush;

    mNodesNumber = 0;

    /* Randomizzo l'input (I triangoli) */
    std::random_shuffle(T().begin(), T().end());

    /* Crea il bsp tree */
    mBSPTreeRoot = _createBSPTree (T());

    std::cout << "BSPTreeMesh::createBSPTree() ends with " << mNodesNumber
              << " nodes" << std::endl << std::flush;
}


/**
 * @brief BSPTreeMesh::_createBSPTree Funzione ricorsiva di creazione del bsptree
 * @param s Triangoli della partizione
 * @return
 */
BSPNode* BSPTreeMesh::_createBSPTree (std::vector<Triangle> s)
{
    if (s.size() <= 1)
    {
        /* Nessun triangolo nel vettore */
        if (s.size() == 0)
            return NULL;

        mNodesNumber++;

        /* Un triangolo nel vettore, creo una foglia */
        BSPLeafNode* leaf = new BSPLeafNode ();
        leaf->NodeTriangle = s.at(0);

        return leaf;
    }
    /* Piu' triangoli nel vettore, suddivo gli insiemi e richiamo la funzione per i figli */
    else
    {
        mNodesNumber++;

        std::vector<Triangle> leftTriangles;
        std::vector<Triangle> rightTriangles;
        std::vector<Triangle> centerTriangles;

        /* Utilizzo il primo triangolo della lista come piano di taglio */
        Triangle subdivisionPlane = s.at(0);

        /* Creo il nodo interno */
        BSPInternalNode* internal = new BSPInternalNode ();

        /* Aggiungo alla lista di quelli centrali, il triangolo utilizzato
         * come suddivisione */
        centerTriangles.push_back(s.at(0));

        /* Controllo in che posizioni stanno i restanti triangoli della lista ed agisco di conseguenza */
        for (unsigned i=1; i<s.size(); i++)
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

            /* Il triangolo corrente interseca il piano di taglio; trovo i punti di intersezione,
             * triangolo i punti ottenuti, ed aggiungo i nuovi vertici ed i nuovi triangoli alle
             * liste apposite */
            default:
                //centerTriangles.push_back(s.at(i));
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
