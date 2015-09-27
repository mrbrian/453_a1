/*
 * CPSC 453 - Introduction to Computer Graphics
 * Assignment 1
 *
 * Renderer - OpenGL widget for drawing scene
 */

#ifndef RENDERER_H
#define RENDERER_H

#define _USE_MATH_DEFINES
#include "game.h"
#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QMouseEvent>

using namespace std;

class Renderer : public QOpenGLWidget, protected QOpenGLFunctions
{

    // informs the qmake that a Qt moc_* file will need to be generated
    Q_OBJECT

public:
    // constructor
    Renderer(QWidget *parent = 0);

    // destructor
    virtual ~Renderer();

    void setGame(Game *game);
    // public accessor
    void setIsScaling(bool val);
    void setDrawMode(int mode);

    virtual void update();


    static const int DRAW_WIRE  = 0;
    static const int DRAW_FACES = 1;
    static const int DRAW_MULTI = 2;

public slots:
    void resetView();

protected:

    // override fundamental drawing functions

    // Called when OpenGL is first initialized
    void initializeGL();

    // Called when the window is resized
    void resizeGL(int w, int h);

    // Called when the window needs to be redrawn
    void paintGL();

    // override mouse event functions

    // Called when a mouse button is pressed
    virtual void mousePressEvent(QMouseEvent * event);

    // Called when a mouse button is released
    virtual void mouseReleaseEvent(QMouseEvent * event);

    // Called when the mouse moves
    virtual void mouseMoveEvent(QMouseEvent * event);

private:

    // member variables for shader manipulation
    GLuint m_programID;
    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_norAttr;
    GLuint m_MMatrixUniform; // model matrix
    GLuint m_VMatrixUniform; // view matrix
    GLuint m_PMatrixUniform; // projection matrix

    // pointer to border triangles vbo
    GLuint m_triVbo;

    // pointer to box vbo
    GLuint m_boxVbo;

    QOpenGLShaderProgram *m_program;

    // for storing triangle vertices and colours
    vector<GLfloat> triVertices;
    vector<GLfloat> triColours;
    vector<GLfloat> triNormals;

    // helper function for loading shaders
    GLuint loadShader(GLenum type, const char *source);

    // helper functions for drawing/saving corner triangles to VBO
    void generateBorderTriangles();    
    void drawTriangles();

    // helper functions for drawing the game walls/game board
    void drawWalls(QVector3D offset);
    void drawGame(QVector3D offset);

    // helper functions for drawing/initializing a cube
    void setupBox();
    void drawBox(int cIdx);

    Game *game;

    // keep track of which renderering mode to draw
    // 0 = wireframe, 1 = face, 2 = multicolour
    int drawMode;

    bool isScaling;
    float scale;
    QPoint prevMousePos;
    QVector3D rotation;
    QVector3D rotationVel;
};

#endif // RENDERER_H
