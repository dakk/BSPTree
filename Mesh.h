#pragma once

#include <vector>
#include <fstream>
#include <GL/gl.h>

#include "Vec3D.h"

#define DEBUG_TRIANGULATION

class Mesh
{
        public:

                typedef struct {Vec3Df p; Vec3Df n;} Vertex;
                typedef Vec3D<unsigned int> Triangle;
                typedef struct
                {
                        Vec3Df min; Vec3Df max;
                        Vec3Df center() { return (min + max) * 0.5;    }
                        float  diag()   { return (min - max).length(); }
                } BBox;


                class Exception
                {
                        public:

                                inline Exception (const std::string & msg) : _msg ("Mesh Exception: " + msg) {}
                                inline const std::string & msg () const { return _msg; }

                        protected:

                                std::string _msg;
                };


                inline std::vector<Vertex>   & V () { return _V; }
                inline std::vector<Triangle> & T () { return _T; }

                inline BBox & bbox () { return _bbox; }

                inline const std::vector<Vertex>   & V () const { return _V; }
                inline const std::vector<Triangle> & T () const { return _T; }

                inline void clear () { _V.clear (); _T.clear (); }

                void recompute_normals();
                void recompute_bbox();

                void draw();

                void load_OFF(const std::string &filename);


        private:

                std::vector<Vertex>   _V;
                std::vector<Triangle> _T;

                BBox _bbox;
};
