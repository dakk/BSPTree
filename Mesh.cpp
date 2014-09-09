#include "Mesh.h"

#include <float.h>

void Mesh::recompute_normals()
{
        for (unsigned int i = 0; i < _T.size (); i++)
        {
                Vec3Df e01 (_V[_T[i][1]].p - _V[_T[i][0]].p);
                Vec3Df e02 (_V[_T[i][2]].p - _V[_T[i][0]].p);

                Vec3Df nt = Vec3Df::cross_product(e01, e02);
                nt.normalize();

                for (unsigned int j = 0; j < 3; j++) _V[_T[i][j]].n += nt;
        }

        for (unsigned int i = 0; i < _V.size (); i++) _V[i].n.normalize();
}


void Mesh::recompute_bbox()
{
        _bbox.min = Vec3Df( FLT_MAX,  FLT_MAX,  FLT_MAX);
        _bbox.max = Vec3Df(-FLT_MAX, -FLT_MAX, -FLT_MAX);

        for (unsigned int i = 0; i < _V.size (); i++)
        {

                for(int coord=0; coord<3; ++coord)
                {
                        _bbox.min[coord] = std::min(_bbox.min[coord], _V[coord].p[i]);
                        _bbox.max[coord] = std::max(_bbox.max[coord], _V[coord].p[i]);
                }
        }
}


void Mesh::draw()
{
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof (Vertex), (GLvoid*)(&_V[0].p));
        glNormalPointer(GL_FLOAT, sizeof (Vertex), (GLvoid*)(((float*)&_V[0].p) + 3));
        glDrawElements(GL_TRIANGLES, 3 * _T.size(), GL_UNSIGNED_INT, (GLvoid*)(&_T[0]));
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
}


void Mesh::load_OFF(const std::string & filename)
{
        clear();

        std::ifstream in(filename.c_str());
        if (!in) throw Exception ("Error loading OFF file: " + filename);

        std::string offString;
        unsigned int numV, numF, tmp;
        in >> offString >> numV >> numF >> tmp;

        _V.resize(numV);
        for (unsigned int i = 0; i < numV; i++) in >> _V[i].p;

        unsigned int s;
        for (unsigned int i = 0; i < numF; i++)
        {
                in >> s;
                std::vector<unsigned int> v(s);
                for (unsigned int j = 0; j < s; j++) in >> v[j];
                for (unsigned int j = 2; j < s; j++) _T.push_back(Triangle (v[j-2], v[j-1], v[j]));
        }
        in.close ();

        recompute_bbox();
        recompute_normals();
}
