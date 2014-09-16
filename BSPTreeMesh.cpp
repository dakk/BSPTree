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
    mBSPTreeRoot = NULL;
}


/**
 * @brief BSPTreeMesh::save Salva il BSPTree
 * @param filename Path di salvataggio
 * @return true se l'operazione e' andata a buon fine
 */
bool BSPTreeMesh::save (const std::string &filename)
{
    // Il file designato e' filename + ".tree"
    return false;
}


/**
 * @brief BSPTreeMesh::load Carica il BSPTree
 * @param filename Path di caricamento
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
void BSPTreeMesh::draw(Vec3Df cameraPosition)
{
    /* Numero di triangoli renderizzati nella draw corrente */
    unsigned renderedTriangles;

    Vertex pov;
    pov.p = cameraPosition;

    std::cout << pov.p << "\n" << std::flush;

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glVertexPointer(3, GL_FLOAT, sizeof (Vertex), (GLvoid*)(&V()[0].p));
    glNormalPointer(GL_FLOAT, sizeof (Vertex), (GLvoid*)(((float*)&V()[0].p) + 3));

    renderedTriangles = _draw (mBSPTreeRoot, pov);
    std::cout << "BSPTreeMesh::draw() - " << renderedTriangles << " triangles of " << T().size() << " total\n" << std::flush;

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}


/**
 * @brief BSPTreeMesh::_draw
 * @param root
 * @param pov
 * @return Numero di triangoli renderizzati
 */
unsigned BSPTreeMesh::_draw (BSPNode *root, Vertex pov)
{
    if (root == NULL) return 0;

    if (BSPLeafNode* leaf = dynamic_cast<BSPLeafNode*>(root))
    {
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (GLvoid*)(&(leaf->NodeTriangle)));
        return 1;
    }
    else if (BSPInternalNode* internal = dynamic_cast<BSPInternalNode*>(root))
    {
        unsigned renderedTriangles = 0;
        double det = determinant (internal->NodeTriangles[0], pov);
        Position pos = determinantToPosition(det);

        switch (pos)
        {
        case POS_RIGHT:
            renderedTriangles += _draw(internal->Left, pov);
            glDrawElements(GL_TRIANGLES, 3 * internal->NodeTriangles.size(),
                           GL_UNSIGNED_INT, (GLvoid*)(&internal->NodeTriangles[0]));
            renderedTriangles += internal->NodeTriangles.size();
            renderedTriangles += _draw(internal->Right, pov);
            break;

        case POS_LEFT:
            renderedTriangles += _draw(internal->Right, pov);
            glDrawElements(GL_TRIANGLES, 3 * internal->NodeTriangles.size(),
                           GL_UNSIGNED_INT, (GLvoid*)(&internal->NodeTriangles[0]));
            renderedTriangles += internal->NodeTriangles.size();
            renderedTriangles += _draw(internal->Left, pov);
            break;

        case POS_CENTER:
            #ifdef DEBUG_TRIANGULATION
                glDrawElements(GL_TRIANGLES, 3 * internal->NodeTriangles.size(),
                               GL_UNSIGNED_INT, (GLvoid*)(&internal->NodeTriangles[0]));
                renderedTriangles += internal->NodeTriangles.size();
            #endif

            _draw(internal->Right, pov);
            _draw(internal->Left, pov);
            break;
        }
        return renderedTriangles;
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
 * @return Determinante
 */
double BSPTreeMesh::determinant (Triangle t, Vertex v, double eps)
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
    if (det <= eps && det >= -eps)
        return 0.0;
    else
        return det;
}



/**
 * @brief BSPTreeMesh::createBSPTree
 */
void BSPTreeMesh::createBSPTree ()
{
    std::cout << "BSPTreeMesh::createBSPTree()" << std::endl << std::flush;

    mNodesNumber = 0;

    if (mBSPTreeRoot != NULL)
        delete mBSPTreeRoot;

    /* Randomizzo l'input (i triangoli) */
    std::random_shuffle(T().begin(), T().end());

    /* Crea il bsp tree */
    mBSPTreeRoot = _createBSPTree (T());
    recompute_normals();
    recompute_bbox();

    std::cout << "BSPTreeMesh::createBSPTree() ends with " << mNodesNumber
              << " nodes" << std::endl << std::flush;
}


/**
 * @brief BSPTreeMesh::normalOfTriangle Calcola la normale di un triangolo
 * @param t Triangolo di cui calcolare la normale
 * @return Vettore della normale
 */
Vec3Df BSPTreeMesh::normalOfTriangle (Triangle t)
{
    Vec3Df dir = Vec3Df::cross_product(V()[t[1]].p - V()[t[0]].p, V()[t[2]].p - V()[t[0]].p);
    dir.normalize();
    return dir;
}


/**
 * @brief BSPTreeMesh::planeSegmentIntersection Calcola l'intersezione tra un piano
 *              (definito da 3 punti, ed un segmento definito da due vertici)
 * @param plane
 * @param a
 * @param b
 * @return Il punto di intersezione o NULL
 * @todo Verificare se effettivamente funziona come stabilito
 */
Vec3Df* BSPTreeMesh::planeSegmentIntersection (Triangle plane, Vertex a, Vertex b)
{
    Vec3Df coord = V()[plane[0]].p;
    Vec3Df normal = normalOfTriangle (plane);
    Vec3Df u = b.p - a.p;
    Vec3Df w = a.p - coord;

    float D = Vec3Df::dot_product(normal, u);
    float N = -Vec3Df::dot_product(normal, w);

    if (fabs(D) < EPS)
    {           // segment is parallel to plane
        if (N == 0)                      // segment lies in plane
        {
            //std::cout << "BSPTreeMesh::planeSegmentIntersection () - segment lies in plane\n" << std::flush;
            return NULL;
        }
        else
            return NULL;                    // no intersection
    }
    // they are not parallel
    // compute intersect param
    float sI = N / D;

    if (sI < 0.0 || sI > 1.0)
        return NULL;                        // no intersection

    Vec3Df* contact = new Vec3Df(a.p + sI * u);        // compute segment intersect point

    if (Vec3Df::distance(*contact, a.p) < 0.000001 || Vec3Df::distance(*contact, b.p) < 0.000001)
    {
        //std::cout << "BSPTreeMesh::planeSegmentIntersection () - same point\n" << std::flush;
        return NULL;
    }
    else
        return contact;
}






/**
 * @brief triangulate Dato il vecchio triangolo ed il piano di taglio, ritriangola l'area
 * @param oldTriangle Triangolo da ritriangolare
 * @param cutPlane Piano di taglio considerato
 * @return Nuova triangolazione
 * @note Devo realizzare dei triangoli con l'ordine corretto dei vertici, per preservare
 *          il front side della faccia
 * @note Deve esistere un edge tra le intersezioni, sempre
 */
std::vector<Mesh::Triangle> BSPTreeMesh::triangulate(Triangle oldTriangle, Triangle cutPlane)
{
    /* Crea un vertice dato un vec3df, lo inserisce nella lista V(),
     * restituisce l'indice */
    std::function<unsigned(Vec3Df)> createVertex = [&, this](Vec3Df p)
    {
        Vertex v;
        v.p = p;
        /*v.n = p;
        v.n.normalize();*/
        this->V().push_back (v);
        return this->V().size()-1;
    };

    std::vector<Triangle> newTriangles; // Nuovi triangoli generati
    CircularVec<Vec3Df> vertices;       // Dati dei vertici
    CircularVec<unsigned> vertices_i;   // Indici devi vertici
    CircularVec<bool> intersect_v;      // Lista di bool dove true = il vertice i e' un intersezione
    Vec3Df *intersect;                  // Puntatore di appoggio per le intersezioni

    /* Aggiungo i vertici in ordine di comparsa; parto dal primo vertice
     * aggiungo l'intersezione tra il primo e secondo vertice (se esiste), aggiungo il secondo indice,
     * intersezione tra secondo e terzo, terzo vertice. In questo modo ho una lista ordinata (circolare) in senso
     * antiorario dei nuovi vertici da triangolare. */
    vertices.push_back(V()[oldTriangle[0]].p);
    vertices_i.push_back(oldTriangle[0]);
    intersect_v.push_back(false);

    intersect = planeSegmentIntersection (cutPlane, V()[oldTriangle[0]], V()[oldTriangle[1]]);
    if (intersect)
    {
        vertices.push_back (*intersect);
        vertices_i.push_back (createVertex(*intersect));
        intersect_v.push_back(true);

        delete intersect;
    }

    vertices.push_back(V()[oldTriangle[1]].p);
    vertices_i.push_back(oldTriangle[1]);
    intersect_v.push_back(false);

    intersect = planeSegmentIntersection (cutPlane, V()[oldTriangle[1]], V()[oldTriangle[2]]);
    if (intersect)
    {
        vertices.push_back (*intersect);
        vertices_i.push_back (createVertex(*intersect));
        intersect_v.push_back(true);

        delete intersect;
    }

    vertices.push_back(V()[oldTriangle[2]].p);
    vertices_i.push_back(oldTriangle[2]);
    intersect_v.push_back(false);

    intersect = planeSegmentIntersection (cutPlane, V()[oldTriangle[2]], V()[oldTriangle[0]]);
    if (intersect)
    {
        vertices.push_back (*intersect);
        vertices_i.push_back (createVertex(*intersect));
        intersect_v.push_back(true);

        delete intersect;
    }


    /* Ora verifico quanti vertici ho; posso averne 4 (una sola intersezione), 5 (due intersezioni) */
    /* Ho solo una intersezione, creo due triangoli */
    if (vertices.size() == 4)
    {
        /* Trovo l'indice di quello intersezione */
        int inti = 0;
        for (inti = 0; !intersect_v[inti]; inti++);

        /* I triangoli da fare sono T(inti, inti+1, inti+2) e T(inti-2, inti-1, inti) */
        newTriangles.push_back (Triangle(vertices_i[inti],vertices_i[inti+1], vertices_i[inti+2]));
        newTriangles.push_back (Triangle(vertices_i[inti-2],vertices_i[inti-1], vertices_i[inti]));
    }
    /* Due intersezioni, creo 3 triangoli */
    else if (vertices.size() == 5)
    {
        /* Trovo l'indice di quello intersezione */
        int inti = 0;
        for (inti = 0; !intersect_v[inti]; inti++);

        /* Se l'intersezione che ha trovato il for, e' distante 3 vertici dal successivo
         * ritorno al caso generico impostando inti con l'indice dell'altra intersezione */
        if (intersect_v[inti+3])
            inti = (inti + 3) % intersect_v.size();

        /* Creo il triangoletto solitario */
        newTriangles.push_back (Triangle(vertices_i[inti],vertices_i[inti+1], vertices_i[inti+2]));

        /* Triangolo il restante quadrilatero */
        newTriangles.push_back (Triangle(vertices_i[inti+2],vertices_i[inti+3], vertices_i[inti]));
        newTriangles.push_back (Triangle(vertices_i[inti+3],vertices_i[inti+4], vertices_i[inti]));
    }
    /* Nessuna intersezione, in teoria non dovrebbe accadere; potrebbe accadere per errori
     * numerici, ma non e' mai accaduto coi modelli di prova. Lascio comunque questo ramo else
     * per debug */
    else if (vertices.size() == 3)
    {
        newTriangles.push_back(Triangle (vertices_i[0],vertices_i[1],vertices_i[2]));
    }

    return newTriangles;
}



/**
 * @brief positionOfTriangle Determina la posizione del triangolo t rispetto al piano subplane
 * @param subplane Piano di taglio
 * @param t Triangolo da verificare
 * @param eps Definisce l'eps da utilizzare per il determinante
 * @param prevalent Se true, restituisce la posizione prevalente (evitando POS_INTERSECT e POS_CENTER)
 * @return
 */
BSPTreeMesh::Position BSPTreeMesh::positionOfTriangle (Triangle subplane, Triangle t, double eps, bool prevalent)
{
    Position ps[3];
    unsigned left = 0, right = 0, center = 0;

    ps[0] = determinantToPosition(determinant (subplane, V()[t[0]], eps));
    ps[1] = determinantToPosition(determinant (subplane, V()[t[1]], eps));
    ps[2] = determinantToPosition(determinant (subplane, V()[t[2]], eps));

    for (int i=0; i<3; i++)
    {
        switch (ps[i])
        {
        case POS_LEFT:
            left++;
            break;
        case POS_RIGHT:
            right++;
            break;
        default:
            center++;
        }
    }

    if (prevalent)
    {
        if (left > right)
            return POS_LEFT;
        else
            return POS_RIGHT;
    }

    if (left > 0 && right > 0)
        return POS_INTERSECT;
    else if (center == 3)
        return POS_CENTER;
    else if (left > 0)
        return POS_LEFT;
    else
        return POS_RIGHT;
}



/**
 * @brief BSPTreeMesh::_createBSPTree Funzione ricorsiva di creazione del bsptree
 * @param s Triangoli della partizione
 * @return Nodo root del sottoalbero
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
            /* Calcola la posizione di un triangolo rispetto ad
             * un piano di taglio */
            Position pos = positionOfTriangle (subdivisionPlane, s[i]);

            /* A seconda della posizione del triangolo rispetto al piano di taglio, agisco di conseguenza */
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
            case POS_INTERSECT:
                /* Ritriangolo il triangolo corrente, considerando il piano di taglio */
                std::vector<Triangle> newTriangles = triangulate(s[i], subdivisionPlane);

                /* Elimino il vecchio triangolo */
                T().erase(T().begin() + i);

                /* Aggiungo i triangoli ottenuti */
                for (int j = 0; j < newTriangles.size(); j++)
                {
                    /* Aggiungo il nuovo triangolo alla lista totale dei triangoli */
                    T().push_back(newTriangles[j]);

                    /* Ottengo la posizione prevalente del nuovo triangolo rispetto
                     * al piano di taglio, e lo aggiungo alla lista apposita */
                    Position prevalent = positionOfTriangle(subdivisionPlane, newTriangles[j], 0.0, true);

                    switch (prevalent)
                    {
                    case POS_RIGHT:
                        rightTriangles.push_back(newTriangles[j]);
                        break;
                    case POS_LEFT:
                        leftTriangles.push_back(newTriangles[j]);
                        break;
                    /* Questo case non e' necessario */
                    #if 0
                    default:
                        centerTriangles.push_back(newTriangles[j]);
                    #endif
                    }
                }
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
 * @brief BSPTreeMesh::load_OFF Carica il file .off; ho fatto l'overload di questo metodo in modo
 *          da eseguire direttamente la creazione del BSPTree a seguito del caricamento
 * @param filename Path del file da caricare
 */
void BSPTreeMesh::load_OFF (const std::string &filename)
{
    Mesh::load_OFF (filename);

    /* Prova a caricare il bsptree da file */
    if (load (filename))
    {

    }
    /* Se non e' stato possibile caricare il bsptree, lo genero */
    else
    {
        createBSPTree ();
        save (filename);
    }
}


