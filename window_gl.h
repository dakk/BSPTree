#pragma once

#include <QtOpenGL/QGLWidget>
#include <QKeyEvent>
#include <QtOpenGL>

#include "Shader.h"
#include "BSPTreeMesh.h"

class Window_gl : public QGLWidget
{

        Q_OBJECT


        public:

                Window_gl(QWidget *parent = 0);


        public
        slots:

                void open(const QString &filename);


        protected:

                void initializeGL();
                void resizeGL(int w, int h);
                void paintGL();


                // event handlers...
                //
                void keyPressEvent(QKeyEvent *event);
                void mousePressEvent(QMouseEvent *event);
                void mouseMoveEvent(QMouseEvent *event);
                void wheelEvent(QWheelEvent *event);


                // define a new orthogonal projection
                // (to be called after any diagonal update)
                //
                void updateProjectionMatrix();
                void translate(Vec3Df trans);
                void rotate(Vec3Df axis, float angle);
                void centerScene();
                void mapToSphere(QPoint v2D, Vec3Df& v3D);




        private:


                // pointers to mesh and skeleton
                //
                BSPTreeMesh    mesh;
                Shader  *phongShader;


                GLdouble projection_matrix[16],
                         modelview_matrix[16];


                // Trackball handling
                //
                float   radius;
                Vec3Df  center;
                Vec3Df  lastPoint3D;
                QPoint  lastPoint2D;
};
