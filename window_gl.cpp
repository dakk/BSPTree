#include "window_gl.h"
#include <QMatrix3x3>

#define TRACKBALL_RADIUS    0.6



using namespace std;

Window_gl::Window_gl(QWidget *parent) : QGLWidget(parent)
{
    radius = 1.f;
}


void Window_gl::open(const QString &filename)
{
    try
    {
        mesh.load_OFF(string(qPrintable(filename)));

        radius = mesh.bbox().diag() * 0.4;
        center = mesh.bbox().center();

        centerScene();
        updateProjectionMatrix();
    }
    catch (Mesh::Exception e)
    {
        cerr << e.msg () << endl;
    }
    updateGL();
}


void Window_gl::updateProjectionMatrix()
{
    makeCurrent();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat) width() / (GLfloat) height(), 0.01*radius, 100.0*radius);
    glGetDoublev(GL_PROJECTION_MATRIX, projection_matrix);
    glMatrixMode(GL_MODELVIEW);
}


void Window_gl::translate(Vec3Df trans)
{
    makeCurrent();
    glLoadIdentity();
    glTranslated(trans[0], trans[1], trans[2]);
    glMultMatrixd(modelview_matrix);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview_matrix);

    updateCameraPosition();
}


void Window_gl::rotate(Vec3Df axis, float angle)
{
    Vec3Df t( modelview_matrix[0]*center[0] +
            modelview_matrix[4]*center[1] +
            modelview_matrix[8]*center[2] +
            modelview_matrix[12],
            modelview_matrix[1]*center[0] +
            modelview_matrix[5]*center[1] +
            modelview_matrix[9]*center[2] +
            modelview_matrix[13],
            modelview_matrix[2]*center[0] +
            modelview_matrix[6]*center[1] +
            modelview_matrix[10]*center[2] +
            modelview_matrix[14] );

    makeCurrent();
    glLoadIdentity();
    glTranslatef(t[0], t[1], t[2]);
    glRotated(angle, axis[0], axis[1], axis[2]);
    glTranslatef(-t[0], -t[1], -t[2]);
    glMultMatrixd(modelview_matrix);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview_matrix);

    updateCameraPosition();
}

void Window_gl::centerScene()
{
    translate( Vec3Df( -(modelview_matrix[0]   * center[0] +
               modelview_matrix[4]  * center[1] +
            modelview_matrix[8]  * center[2] +
            modelview_matrix[12]),
            -(modelview_matrix[1]  * center[0] +
            modelview_matrix[5]  * center[1] +
            modelview_matrix[9]  * center[2] +
            modelview_matrix[13]),
            -(modelview_matrix[2]  * center[0] +
            modelview_matrix[6]  * center[1] +
            modelview_matrix[10] * center[2] +
            modelview_matrix[14] +
            3.0*radius) ) );

    updateCameraPosition();
}


/**
 * @brief Window_gl::updateCameraPosition Aggiorna la posizione della camera,
 *          ricavandola dal modelview_matrix
 * @note La richiamo da ogni funzione che modifica il modelview_matrix (rotate, translate, centerscene)
 */
void Window_gl::updateCameraPosition()
{
    cameraPosition.init (modelview_matrix[12], modelview_matrix[13], modelview_matrix[14]);
}


void Window_gl::paintGL(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(projection_matrix);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd(modelview_matrix);

#ifdef DEBUG_TRIANGULATION
    glEnable(GL_DEPTH_TEST);
#endif
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    mesh.draw(cameraPosition);
}




// define a new viewport
//
void Window_gl::resizeGL(int w, int h)
{
    glViewport(0,0,w,h);	// define the new viewport
    updateProjectionMatrix();
}



// initialize openGl environment
//
void Window_gl::initializeGL()
{
    qglClearColor(QColor(255,255,255));

    glDisable(GL_DITHER);
    glEnable(GL_NORMALIZE);     // forces glNormal() vectors normalization

    GLfloat light_position1[4] = {   42,  374,  161, 0 };
    GLfloat light_position2[4] = {  473, -351, -259, 0 };
    GLfloat light_position3[4] = { -438,  167,  -48, 0 };

    GLfloat direction1[3] = {  -42, -374, -161 };
    GLfloat direction2[3] = { -473,  351,  259 };
    GLfloat direction3[3] = {  438, -167,   48 };

    GLfloat color1[4] = { 1.00, 1.00, 1.00, 1 };
    GLfloat color2[4] = { 0.28, 0.39, 1.00, 1 };
    GLfloat color3[4] = { 1.00, 0.69, 0.23, 1 };

    GLfloat specularColor1[4] = { 0.8, 0.8, 0.8, 1 };
    GLfloat specularColor2[4] = { 0.8, 0.8, 0.8, 1 };
    GLfloat specularColor3[4] = { 0.8, 0.8, 0.8, 1 };

    GLfloat ambient[4] = { 0.3f, 0.3f, 0.3f, 0.5f };

    glLightfv (GL_LIGHT0, GL_POSITION, light_position1);
    glLightfv (GL_LIGHT0, GL_SPOT_DIRECTION, direction1);
    glLightfv (GL_LIGHT0, GL_DIFFUSE, color1);
    glLightfv (GL_LIGHT0, GL_SPECULAR, specularColor1);

    glLightfv (GL_LIGHT1, GL_POSITION, light_position2);
    glLightfv (GL_LIGHT1, GL_SPOT_DIRECTION, direction2);
    glLightfv (GL_LIGHT1, GL_DIFFUSE, color2);
    glLightfv (GL_LIGHT1, GL_SPECULAR, specularColor2);

    glLightfv (GL_LIGHT2, GL_POSITION, light_position3);
    glLightfv (GL_LIGHT2, GL_SPOT_DIRECTION, direction3);
    glLightfv (GL_LIGHT2, GL_DIFFUSE, color3);
    glLightfv (GL_LIGHT2, GL_SPECULAR, specularColor3);

    glLightModelfv (GL_LIGHT_MODEL_AMBIENT, ambient);

    glEnable (GL_LIGHTING);
    glEnable (GL_LIGHT0);
    glEnable (GL_LIGHT1);
    glEnable (GL_LIGHT2);
    glEnable (GL_LIGHT3);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview_matrix);
    updateProjectionMatrix();
    centerScene();

    //        try
    //        {
    //                phongShader = new Shader;
    //                phongShader->loadFromFile("shaders/phong.vert", "shaders/phong.frag");
    //                phongShader->bind();
    //        }
    //        catch (Shader::Exception e)
    //        {
    //                cerr << e.msg () << endl;
    //                exit (EXIT_FAILURE);
    //        }
}



/****************************************************
 ****************************************************
 **                                                **
 **                  EVENT HANLDERS                **
 **                                                **
 ****************************************************
 ****************************************************/

void Window_gl::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Left:
    {
        if (event->modifiers() & Qt::ShiftModifier)
            translate(Vec3Df(-radius / 100.0, 0.0, 0.0));
        else
            rotate(Vec3Df(0,1,0), -TRACKBALL_RADIUS);

        break;
    }

    case Qt::Key_Right:
    {
        if (event->modifiers() & Qt::ShiftModifier)
            translate(Vec3Df(radius / 100.0, 0.0, 0.0));
        else
            rotate(Vec3Df(0,1,0),  TRACKBALL_RADIUS);

        break;
    }

    case Qt::Key_Up:
    {
        if (event->modifiers() & Qt::ShiftModifier)
            translate(Vec3Df(0.0, radius / 100.0, 0.0));
        else
            rotate(Vec3Df(1,0,0), -TRACKBALL_RADIUS);

        break;
    }

    case Qt::Key_Down:
    {
        if (event->modifiers() & Qt::ShiftModifier)
            translate(Vec3Df(0.0, -radius / 100.0, 0.0));
        else
            rotate(Vec3Df(1,0,0),  TRACKBALL_RADIUS);

        break;
    }

    case Qt::Key_Plus:  translate(Vec3Df(0.0, 0.0, radius / 10.0));  break;

    case Qt::Key_Minus: translate(Vec3Df(0.0, 0.0, -radius / 10.0)); break;
    }

    updateGL();
}



void Window_gl::mousePressEvent( QMouseEvent *event )
{
    lastPoint2D = event->pos();
    mapToSphere(lastPoint2D, lastPoint3D);
}


void Window_gl::wheelEvent(QWheelEvent *event)
{
    float d = -(float) event->delta() / 120.0 * 0.2 * radius;

    translate(Vec3Df(0.0, 0.0, d));

    updateGL();

    event->accept();
}


void Window_gl::mouseMoveEvent(QMouseEvent *event)
{
    if (event->modifiers() & Qt::ShiftModifier)
    {
        QPoint newPoint2D = event->pos();

        float dx = newPoint2D.x() - lastPoint2D.x();
        float dy = newPoint2D.y() - lastPoint2D.y();

        float z = - (modelview_matrix[ 2]*center[0] +
                modelview_matrix[ 6]*center[1] +
                modelview_matrix[10]*center[2] +
                modelview_matrix[14]) /
                (modelview_matrix[ 3]*center[0] +
                modelview_matrix[ 7]*center[1] +
                modelview_matrix[11]*center[2] +
                modelview_matrix[15]);

        float w          = width();
        float h          = height();
        float aspect     = w / h;
        float near_plane = 0.01 * radius;
        float top        = tan (45.f / 2.0f * M_PI / 180.0f) * near_plane;
        float right      = aspect * top;

        translate(Vec3Df( 2.0 * dx / w * right / near_plane * z, -2.0 * dy / h * top / near_plane * z, 0.0f));

        lastPoint2D = newPoint2D;

        updateGL();

        return;
    }


    // ROTATION EVENT
    //
    if (event->buttons() == Qt::LeftButton)
    {
        QPoint newPoint2D = event->pos();
        Vec3Df newPoint3D;

        mapToSphere(newPoint2D, newPoint3D);

        makeCurrent();

        Vec3Df axis = Vec3Df::cross_product(lastPoint3D, newPoint3D);

        if (axis.squared_length() < 1e-7)
            axis = Vec3Df(1,0,0);
        else
            axis.normalize();

        // find the amount of rotation
        Vec3Df d = lastPoint3D - newPoint3D;

        float t = 0.5 * d.length() / TRACKBALL_RADIUS;

        if ( t < -1.0 )
        {
            t = -1.0;
        }
        else
        {
            if ( t > 1.0 )
                t = 1.0;
        }

        float    phi = 2.0 * asin(t);
        float  angle = phi * 180.0 / M_PI;

        rotate(axis, angle);

        lastPoint2D = newPoint2D;
        lastPoint3D = newPoint3D;

        updateGL();

        return;
    }
}


// This is actually doing the Sphere/Hyperbolic sheet hybrid thing,
// based on Ken Shoemake's ArcBall in Graphics Gems IV, 1993.
//
void Window_gl::mapToSphere(QPoint v2D, Vec3Df& v3D)
{
    double x =  (2.0 * v2D.x() - width()) / width();
    double y = -(2.0 * v2D.y() - height()) / height();
    double xval = x;
    double yval = y;
    double x2y2 = xval * xval + yval * yval;

    double rsqr = TRACKBALL_RADIUS * TRACKBALL_RADIUS;

    v3D[0] = xval;
    v3D[1] = yval;

    if (x2y2 < 0.5 * rsqr)
        v3D[2] = sqrt(rsqr - x2y2);
    else
        v3D[2] = 0.5 * rsqr/sqrt(x2y2);
}
