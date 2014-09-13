#include "GLApp.h"

#include <QMenuBar>

GLApp::GLApp(const QString &app_name) : QMainWindow ()
{
        QAction * fileOpenAction = new QAction ("Open", this);
        fileOpenAction->setShortcut(tr("Ctrl+O"));
        connect (fileOpenAction, SIGNAL (triggered()), this, SLOT (open()));

        QActionGroup * fileActionGroup = new QActionGroup (this);
        fileActionGroup->addAction (fileOpenAction);
        QMenu * fileMenu = menuBar ()->addMenu (tr ("File"));;
        fileMenu->addActions (fileActionGroup->actions());

        _viewer = new Window_gl();
        setCentralWidget (_viewer);
        setMinimumSize(800, 600);
        setWindowTitle(app_name);

        /* Senza settare esplicitamente il focus, non mi cattura gli eventi
         * keypress della tastiera */
        _viewer->setFocus();
}

