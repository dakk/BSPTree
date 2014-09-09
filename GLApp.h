#pragma once

#include <QMainWindow>
#include <QFileDialog>
#include <QDir>

#include "window_gl.h"


class GLApp : public QMainWindow
{

        Q_OBJECT


        public:

                GLApp(const QString &app_name);

                inline Window_gl *viewer() { return _viewer; }


                public slots:

                        void open (const QString & filename) { _viewer->open(filename); }
                        void open () { open(QFileDialog::getOpenFileName((QWidget*)this, "Choose the mesh to load", QDir::currentPath(), "OFF (*.off)")); }

                private:

                        Window_gl *_viewer;
};
