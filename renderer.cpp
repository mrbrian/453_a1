#include "renderer.h"
#include <QTextStream>
#include <QOpenGLBuffer>
#include <cmath>

// color indexes
#define GRAY_IDX  7
#define BLACK_IDX 8
#define MULTI_IDX 9

// Define the box's geometry (as quads)
const float box_coords[] = {
    0,1,0,  0,1,1,  1,1,1, 1,1,0,   // top
    0,0,0,  1,0,0,  1,0,1, 0,0,1,   // bottom
    0,1,0,  0,0,0,  0,0,1, 0,1,1,   // left
    1,1,0,  1,1,1,  1,0,1, 1,0,0,   // right
    0,1,1,  0,0,1,  1,0,1, 1,1,1,   // front
    1,1,0,  1,0,0,  0,0,0, 0,1,0,   // back
};

// box normals
const float box_norms[] = {
    0,1,0,   0,1,0,   0,1,0,   0,1,0,   // top
    0,-1,0,  0,-1,0,  0,-1,0,  0,-1,0,  // bottom
    -1,0,0,  -1,0,0,  -1,0,0,  -1,0,0,  // left
    1,0,0,   1,0,0,   1,0,0,   1,0,0,   // right
    0,0,1,   0,0,1,   0,0,1,   0,0,1,   // front
    0,0,-1,  0,0,-1,  0,0,-1,  0,0,-1,  // back
};

// all box colours
const float box_cols[] = {
    1,0,0,  1,0,0,  1,0,0,  1,0,0,  // red
    1,0,0,  1,0,0,  1,0,0,  1,0,0,
    1,0,0,  1,0,0,  1,0,0,  1,0,0,
    1,0,0,  1,0,0,  1,0,0,  1,0,0,
    1,0,0,  1,0,0,  1,0,0,  1,0,0,
    1,0,0,  1,0,0,  1,0,0,  1,0,0,

    0,0,1,  0,0,1,  0,0,1,  0,0,1,  // blue
    0,0,1,  0,0,1,  0,0,1,  0,0,1,
    0,0,1,  0,0,1,  0,0,1,  0,0,1,
    0,0,1,  0,0,1,  0,0,1,  0,0,1,
    0,0,1,  0,0,1,  0,0,1,  0,0,1,
    0,0,1,  0,0,1,  0,0,1,  0,0,1,

    0,1,0,  0,1,0,  0,1,0,  0,1,0,  // green
    0,1,0,  0,1,0,  0,1,0,  0,1,0,
    0,1,0,  0,1,0,  0,1,0,  0,1,0,
    0,1,0,  0,1,0,  0,1,0,  0,1,0,
    0,1,0,  0,1,0,  0,1,0,  0,1,0,
    0,1,0,  0,1,0,  0,1,0,  0,1,0,

    1,1,0,  1,1,0,  1,1,0,  1,1,0,  // yellow
    1,1,0,  1,1,0,  1,1,0,  1,1,0,
    1,1,0,  1,1,0,  1,1,0,  1,1,0,
    1,1,0,  1,1,0,  1,1,0,  1,1,0,
    1,1,0,  1,1,0,  1,1,0,  1,1,0,
    1,1,0,  1,1,0,  1,1,0,  1,1,0,

    0,1,1,  0,1,1,  0,1,1,  0,1,1,  // cyan
    0,1,1,  0,1,1,  0,1,1,  0,1,1,
    0,1,1,  0,1,1,  0,1,1,  0,1,1,
    0,1,1,  0,1,1,  0,1,1,  0,1,1,
    0,1,1,  0,1,1,  0,1,1,  0,1,1,
    0,1,1,  0,1,1,  0,1,1,  0,1,1,

    1,0,1,  1,0,1,  1,0,1,  1,0,1,  // magenta
    1,0,1,  1,0,1,  1,0,1,  1,0,1,
    1,0,1,  1,0,1,  1,0,1,  1,0,1,
    1,0,1,  1,0,1,  1,0,1,  1,0,1,
    1,0,1,  1,0,1,  1,0,1,  1,0,1,
    1,0,1,  1,0,1,  1,0,1,  1,0,1,

    1,.5,0,  1,.5,0,  1,.5,0,  1,.5,0,  // orange
    1,.5,0,  1,.5,0,  1,.5,0,  1,.5,0,
    1,.5,0,  1,.5,0,  1,.5,0,  1,.5,0,
    1,.5,0,  1,.5,0,  1,.5,0,  1,.5,0,
    1,.5,0,  1,.5,0,  1,.5,0,  1,.5,0,
    1,.5,0,  1,.5,0,  1,.5,0,  1,.5,0,

    .5,.5,.5,  .5,.5,.5,  .5,.5,.5,  .5,.5,.5,  // gray
    .5,.5,.5,  .5,.5,.5,  .5,.5,.5,  .5,.5,.5,
    .5,.5,.5,  .5,.5,.5,  .5,.5,.5,  .5,.5,.5,
    .5,.5,.5,  .5,.5,.5,  .5,.5,.5,  .5,.5,.5,
    .5,.5,.5,  .5,.5,.5,  .5,.5,.5,  .5,.5,.5,
    .5,.5,.5,  .5,.5,.5,  .5,.5,.5,  .5,.5,.5,

    0,0,0,  0,0,0,  0,0,0,  0,0,0,    // black
    0,0,0,  0,0,0,  0,0,0,  0,0,0,
    0,0,0,  0,0,0,  0,0,0,  0,0,0,
    0,0,0,  0,0,0,  0,0,0,  0,0,0,
    0,0,0,  0,0,0,  0,0,0,  0,0,0,
    0,0,0,  0,0,0,  0,0,0,  0,0,0,

    1,0,0,	1,0,0,	1,0,0,	1,0,0,      // multicolour (each face a dif colour)
    1,.3,0,	1,.3,0,	1,.3,0,	1,.3,0,
    1,1,0,	1,1,0,	1,1,0,	1,1,0,
    0,1,0,	0,1,0,	0,1,0,	0,1,0,
    0,.3,1,	0,.3,1,	0,.3,1,	0,.3,1,
    .5,.3,1,	.5,.3,1,	.5,.3,1, 	.5,.3,1,
    1,0,1,	1,0,1,	1,0,1, 	1,0,1,
    1,0,0,	1,0,0,	1,0,0,	1,0,0,
    1,.3,0,	1,.3,0,	1,.3,0,	1,.3,0,
    1,1,0,	1,1,0,	1,1,0,	1,1,0,
    0,1,0,	0,1,0,	0,1,0,	0,1,0,
    0,.3,1,	0,.3,1,	0,.3,1,	0,.3,1,
    .5,.3,1,	.5,.3,1,	.5,.3,1, 	.5,.3,1,
    1,0,1,	1,0,1,	1,0,1, 	1,0,1,
};

// constructor
Renderer::Renderer(QWidget *parent)
    : QOpenGLWidget(parent)
{
    drawMode = DRAW_FACES;
    scale = 1;
}

// constructor
Renderer::~Renderer()
{

}

// called once by Qt GUI system, to allow initialization for OpenGL requirements
void Renderer::initializeGL()
{
    // Qt support for inline GL function calls
	initializeOpenGLFunctions();

    // enable depth and face culling
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // sets the background clour
    glClearColor(0.7f, 0.7f, 1.0f, 1.0f);

    // links to and compiles the shaders, used for drawing simple objects
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, "per-fragment-phong.vs.glsl");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, "per-fragment-phong.fs.glsl");
    m_program->link();
    m_posAttr = m_program->attributeLocation("position_attr");
    m_colAttr = m_program->attributeLocation("colour_attr");
    m_norAttr = m_program->attributeLocation("normal_attr");
    m_PMatrixUniform = m_program->uniformLocation("proj_matrix");
    m_VMatrixUniform = m_program->uniformLocation("view_matrix");
    m_MMatrixUniform = m_program->uniformLocation("model_matrix");
    m_programID = m_program->programId();

    // add corner triangles to VBO
    generateBorderTriangles();

    // add unit cube to VBO
    setupBox();
}

// called by the Qt GUI system, to allow OpenGL drawing commands
void Renderer::paintGL()
{
    // Clear the screen buffers

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set the current shader program
    glUseProgram(m_programID);

    // Modify the current projection matrix so that we move the
    // camera away from the origin.  We'll draw the game at the
    // origin, and we need to back up to see it.

    QMatrix4x4 view_matrix;
    view_matrix.translate(0.0f, 0.0f, -40.0f);

    view_matrix.rotate(rotation.x(), 1, 0, 0);
    view_matrix.rotate(rotation.y(), 0, 1, 0);
    view_matrix.rotate(rotation.z(), 0, 0, 1);

    glUniformMatrix4fv(m_VMatrixUniform, 1, false, view_matrix.data());

    // Not implemented: set up lighting (if necessary)

    // You'll be drawing unit cubes, so the game will have width
    // 10 and height 24 (game = 20, stripe = 4).  Let's translate
    // the game so that we can draw it starting at (0,0) but have
    // it appear centered in the window.
    QVector3D offset = QVector3D(-5.0f, -12.0f, 0.0f);

    // draw the game board + walls + border triangles
    drawWalls(offset);
    drawGame(offset);
    drawTriangles(offset);

    // deactivate the program
    m_program->release();
}

// called by the Qt GUI system, to allow OpenGL to respond to widget resizing
void Renderer::resizeGL(int w, int h)
{
    // width and height are better variables to use
    Q_UNUSED(w); Q_UNUSED(h);

    // update viewing projections
    glUseProgram(m_programID);

    // Set up perspective projection, using current size and aspect
    // ratio of display
    QMatrix4x4 projection_matrix;
    projection_matrix.perspective(40.0f, (GLfloat)width() / (GLfloat)height(),
                                  0.1f, 1000.0f);
    glUniformMatrix4fv(m_PMatrixUniform, 1, false, projection_matrix.data());

    glViewport(0, 0, width(), height());
}

// add vertices to rectangle list
const float tri_vertList [] = {
    0.0, 0.0, 0.0,  // bottom left triangle
    1.0, 0.0, 0.0,
    0.0, 1.0, 0.0,

    9.0, 0.0, 0.0,  // bottom right triangle
    10.0, 0.0, 0.0,
    10.0, 1.0, 0.0,

    0.0, 19.0, 0.0, // top left triangle
    1.0, 20.0, 0.0,
    0.0, 20.0, 0.0,

    10.0, 19.0, 0.0,    // top right triangle
    10.0, 20.0, 0.0,
    9.0, 20.0, 0.0 };

float tri_colourList [] = {
    1, 0, 0,
    1, 0, 0,    // all red verts
    1, 0, 0,

    1, 0, 0,
    1, 0, 0,
    1, 0, 0,

    1, 0, 0,
    1, 0, 0,
    1, 0, 0,

    1, 0, 0,
    1, 0, 0,
    1, 0, 0,
};

float tri_normalList [] = {
    0.0f, 0.0f, 1.0f,    // facing viewer
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,

    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,

    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,

    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
};

// computes the vertices and corresponding colours-per-vertex for a quadrilateral
// drawn from (x1, y1) to (x2, y2)
// Note: the magic numbers in the vector insert commands should be better documented
void Renderer::generateBorderTriangles()
{
    long cBufferSize = sizeof(tri_colourList) * sizeof(float);
    long vBufferSize = sizeof(tri_vertList) * sizeof(float);
    long nBufferSize = sizeof(tri_normalList) * sizeof(float);

    glGenBuffers(1, &this->m_triVbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->m_triVbo);

    // Allocate buffer
    glBufferData(GL_ARRAY_BUFFER, vBufferSize + cBufferSize + nBufferSize, NULL, GL_STATIC_DRAW);

    // Upload the data to the GPU
    glBufferSubData(GL_ARRAY_BUFFER, 0, vBufferSize, &tri_vertList[0]);
    glBufferSubData(GL_ARRAY_BUFFER, vBufferSize, cBufferSize, &tri_colourList[0]);
    glBufferSubData(GL_ARRAY_BUFFER, vBufferSize + cBufferSize, nBufferSize, &tri_normalList[0]);
}


// override mouse press event
void Renderer::mousePressEvent(QMouseEvent * event)
{
    QTextStream cout(stdout);
    cout << "Stub: Button " << event->button() << " pressed.\n";

    prevMousePos = event->pos();
}

// override mouse release event
void Renderer::mouseReleaseEvent(QMouseEvent * event)
{
    QTextStream cout(stdout);
    cout << "Stub: Button " << event->button() << " pressed.\n";
}

// override mouse move event
void Renderer::mouseMoveEvent(QMouseEvent * event)
{
    QTextStream cout(stdout);
    cout << "Stub: Motion at " << event->x() << ", " << event->y() << ".\n";

    rotationVel *= 0;   // initialize rotation velocity

    QPoint deltaPos = (event->pos() - prevMousePos);    // calculate movement

    if (isScaling)
    {
        scale -= ((float)deltaPos.x()) / 50.0;
        if (scale < 0)      // prevent negative scaling
            scale = 0;
    }
    else
        if (event->buttons() & Qt::LeftButton)      // LB rotates along x-axis
        {
            rotation.setX(rotation.x() + deltaPos.x());
            rotationVel.setX(deltaPos.x());
        }
    else
        if (event->buttons() & Qt::MiddleButton)    // MB rotates along y-axis
        {
            rotation.setY(rotation.y() + deltaPos.x());
            rotationVel.setY(deltaPos.x());
        }
    else
        if (event->buttons() & Qt::RightButton)     // RB rotates along z-axis
        {
            rotation.setZ(rotation.z() + deltaPos.x());
            rotationVel.setZ(deltaPos.x());
        }

    prevMousePos = event->pos();

    // repaint scene
    update();
}

// resets the renderer current view
void Renderer::resetView()
{
    rotation = QVector3D(0, 0, 0);
    rotationVel = QVector3D(0, 0, 0);
}

// helper function, draw corner triangles
void Renderer::drawTriangles(QVector3D offset)
{
    QMatrix4x4 model_matrix;
    model_matrix.scale(scale);
    model_matrix.translate(offset);
    glUniformMatrix4fv(m_MMatrixUniform, 1, false, model_matrix.data());

    long cBufferSize = sizeof(tri_colourList) * sizeof(float);
    long vBufferSize = sizeof(tri_vertList) * sizeof(float);
    long nBufferSize = sizeof(tri_normalList) * sizeof(float);

    // Bind to the correct context
    glBindBuffer(GL_ARRAY_BUFFER, this->m_triVbo);

    // Enable the attribute arrays
    glEnableVertexAttribArray(this->m_posAttr);
    glEnableVertexAttribArray(this->m_colAttr);
    glEnableVertexAttribArray(this->m_norAttr);

    // Specifiy where these are in the VBO
    glVertexAttribPointer(this->m_posAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)0);
    glVertexAttribPointer(this->m_colAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)(vBufferSize));
    glVertexAttribPointer(this->m_norAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)(vBufferSize + cBufferSize));

    // Draw the triangles
    glDrawArrays(GL_TRIANGLES, 0, 12); // 12 vertices

    glDisableVertexAttribArray(m_norAttr);
    glDisableVertexAttribArray(m_colAttr);
    glDisableVertexAttribArray(m_posAttr);
}

// public set method for game
void Renderer::setGame(Game *game)
{
    this->game = game;
}

// public set method for isScaling flag
void Renderer::setIsScaling(bool val)
{
    this->isScaling = val;
}

// draws all cubes for the "well"
void Renderer::drawWalls(QVector3D offset)
{
    int width = game->getWidth();
    int height = game->getHeight();

    int i = 0;
    // draw the well sides
    for (i = -1; i < height; i++)
    {
        QMatrix4x4 model_matrix;

        // left wall
        QVector3D cubePos = QVector3D(-1, i, 0.0f);
        model_matrix.scale(scale);
        model_matrix.translate(cubePos + offset);
        glUniformMatrix4fv(m_MMatrixUniform, 1, false, model_matrix.data());
        drawBox(GRAY_IDX);

        // right wall
        cubePos = QVector3D(width, i, 0.0f);
        model_matrix.setToIdentity();
        model_matrix.scale(scale);
        model_matrix.translate(cubePos + offset);
        glUniformMatrix4fv(m_MMatrixUniform, 1, false, model_matrix.data());
        drawBox(GRAY_IDX);
    }

    // draw the well bottom
    for (i = 0; i < width ; i++)
    {
        QMatrix4x4 model_matrix;

        QVector3D cubePos = QVector3D(i, -1, 0.0f);
        model_matrix.scale(scale);
        model_matrix.translate(cubePos + offset);
        glUniformMatrix4fv(m_MMatrixUniform, 1, false, model_matrix.data());
        drawBox(GRAY_IDX);
    }
}

void Renderer::drawGame(QVector3D offset)
{
    int width = game->getWidth();
    int height = game->getHeight();

    int i = 0;
    // draw block in each spot
    for (i = 0; i < width * (height + 4); i++)
    {
        int r = i / width;
        int c = i % width;
        QMatrix4x4 model_matrix;

        int cell = game->get(r, c);

        if (cell == -1)
            continue;

        QVector3D cubePos = QVector3D(c, r, 0.0f);

        model_matrix.scale(scale);
        model_matrix.translate(offset + cubePos);
        glUniformMatrix4fv(m_MMatrixUniform, 1, false, model_matrix.data());

        drawBox(cell);
    }
}

// Change the draw mode (Wire, Face, Multicolor)
void Renderer::setDrawMode(int mode)
{
    drawMode = mode;
}

// Saves all the cube info to the VBO
void Renderer::setupBox()
{
    long cBufferSize = sizeof(box_cols) * sizeof(float);
    long vBufferSize = sizeof(box_coords) * sizeof(float);
    long nBufferSize = sizeof(box_norms) * sizeof(float);

    glGenBuffers(1, &this->m_boxVbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->m_boxVbo);

    // Allocate buffer
    glBufferData(GL_ARRAY_BUFFER, vBufferSize + cBufferSize + nBufferSize, NULL, GL_STATIC_DRAW);

    // Upload the data to the GPU
    glBufferSubData(GL_ARRAY_BUFFER, 0, vBufferSize, &box_coords[0]);
    glBufferSubData(GL_ARRAY_BUFFER, vBufferSize, cBufferSize, &box_cols[0]);
    glBufferSubData(GL_ARRAY_BUFFER, vBufferSize + cBufferSize, nBufferSize, &box_norms[0]);
}

// Draw a unit cube and use colors stored at position cIdx
void Renderer::drawBox(int cIdx)
{
    int glDrawMode = 0;
    int floats = 3; // 3 floats per vert
    int verts = 4;  // 4 verts per quad
    int quads = 6;  // 6 quads per box

    long cBufferSize = sizeof(box_cols) * sizeof(float);
    long vBufferSize = sizeof(box_coords) * sizeof(float);
    long nBufferSize = sizeof(box_norms) * sizeof(float);

    long cBufferOffset = 0;

    // Bind to the correct context
    glBindBuffer(GL_ARRAY_BUFFER, this->m_boxVbo);

    // Enable the attribute arrays
    glEnableVertexAttribArray(this->m_posAttr);
    glEnableVertexAttribArray(this->m_colAttr);
    glEnableVertexAttribArray(this->m_norAttr);

    // Specifiy where these are in the VBO
    glVertexAttribPointer(this->m_posAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)0);

    switch (drawMode)
    {
        case DRAW_WIRE:     // wireframe
            cBufferOffset = sizeof(float) * floats * verts * quads * BLACK_IDX; // draw lines in black
            glDrawMode = GL_LINES;
            break;
        case DRAW_FACES:
            cBufferOffset = sizeof(float) * floats * verts * quads * cIdx;
            glDrawMode = GL_QUADS;
            break;
        case DRAW_MULTI:     // multicolor
            cBufferOffset = sizeof(float) * floats * verts * (quads * MULTI_IDX + cIdx);
            glDrawMode = GL_QUADS;
            break;
    }
    glVertexAttribPointer(this->m_colAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)(vBufferSize + cBufferOffset));
    glVertexAttribPointer(this->m_norAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)(vBufferSize + cBufferSize));

    // Draw the faces
    glDrawArrays(glDrawMode, 0, 24); // 24 vertices

    glDisableVertexAttribArray(m_norAttr);
    glDisableVertexAttribArray(m_colAttr);
    glDisableVertexAttribArray(m_posAttr);
}

// updates the continuous spin, and repaints widget
void Renderer::update()
{
    rotation += rotationVel;
    QOpenGLWidget::update();
}
