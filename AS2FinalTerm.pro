TEMPLATE = app
TARGET   = ShaderTest

QT      += opengl
CONFIG  += qt core opengl warn_on thread

LIBS += -lGLEW -lGLU

HEADERS =               \
        Vec3D.h         \
        Mesh.h          \
        GLApp.h         \
        Shader.h        \
        window_gl.h \
    BSPTreeMesh.h

SOURCES =               \
        GLApp.cpp       \
        Shader.cpp      \
        main.cpp        \
        Mesh.cpp        \
        window_gl.cpp \
    BSPTreeMesh.cpp

OTHER_FILES =                   \
        shaders/phong.frag      \
        shaders/phong.vert
