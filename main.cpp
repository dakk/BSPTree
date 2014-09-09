#include <QApplication>
#include <GLApp.h>

int main (int argc, char **argv)
{
    QApplication app( argc, argv );

    GLApp GUI("Shader Test");
    GUI.show();

    return app.exec();
}

