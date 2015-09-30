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
#include <QOpenGLFunctions_4_2_Core>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QMouseEvent>
#include <QTimer>

using namespace std;

class Renderer : public QOpenGLWidget, protected QOpenGLFunctions_4_2_Core
{

    // informs the qmake that a Qt moc_* file will need to be generated
    Q_OBJECT

public:
    // constructor
    Renderer(QWidget *parent = 0);

    // destructor
    virtual ~Renderer();

    // draw mode types
    enum DrawMode {WIRE, FACES, MULTI};

    // public accessors
    void setGame(Game *game);
    void setIsScaling(bool val);
    void setDrawMode(DrawMode mode);

public slots:
    // updates the transformations and calls widget update
    void update();

    // resets the model transformations
    void resetView();

protected:
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
    void drawTriangles(QMatrix4x4 * transform);

    // drawing the game walls
    void drawWalls(QMatrix4x4 * transform);
    // draw the game board
    void drawGame(QMatrix4x4 * transform);
    // initializing a cube
    void setupBox();
    // draw a cube with specific color index
    void drawBox(int cIdx);

    // tetris game reference
    Game *game;

    // keep track of which renderering mode to draw
    // 0 = wireframe, 1 = face, 2 = multicolour
    DrawMode drawMode;

    // model scale factor
    float scale;
    // mouse buttons that are currently pressed
    int mouseButtons;
    // model scaling toggle
    bool isScaling;   

    // track previous mouse position for finding rotation velocity
    QPoint prevMousePos;
    // model rotation info
    QVector3D rotation;
    // model rotation velocities
    QVector3D rotationVel;

    // timer for calling renderer updates
    QTimer * renderTimer;
};

#endif // RENDERER_H
