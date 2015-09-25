#include "renderer.h"
#include <QTextStream>
#include <QOpenGLBuffer>
#include <cmath>

// constructor
Renderer::Renderer(QWidget *parent)
    : QOpenGLWidget(parent)
{
    drawMode = GL_TRIANGLES;
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

}

// called by the Qt GUI system, to allow OpenGL drawing commands
void Renderer::paintGL()
{
    // Clear the screen buffers

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set the current shader program

    glUseProgram(m_programID);

    QMatrix4x4 cameraTransformation;
    cameraTransformation.rotate(rotation.x(), 1, 0, 0);
    cameraTransformation.rotate(rotation.y(), 0, 1, 0);
    cameraTransformation.rotate(rotation.z(), 0, 0, 1);

    QVector3D cameraPosition = cameraTransformation * QVector3D(0, 0, 40);
    QVector3D cameraUpDirection = cameraTransformation * QVector3D(0, 1, 0);

    // Modify the current projection matrix so that we move the
    // camera away from the origin.  We'll draw the game at the
    // origin, and we need to back up to see it.

    QMatrix4x4 view_matrix;
    //view_matrix.translate(0.0f, 0.0f, -40.0f);
    view_matrix.lookAt(cameraPosition, QVector3D(0, 0, 0), cameraUpDirection);
    glUniformMatrix4fv(m_VMatrixUniform, 1, false, view_matrix.data());

    // Not implemented: set up lighting (if necessary)

    int width = game->getWidth();  // includes the game walls in width
    int height = game->getHeight();

    // You'll be drawing unit cubes, so the game will have width
    // 10 and height 24 (game = 20, stripe = 4).  Let's translate
    // the game so that we can draw it starting at (0,0) but have
    // it appear centered in the window.
    QVector3D offset = QVector3D(-5.0f, -12.0f, 0.0f);

    generateCube(Qt::gray);

    drawWalls(offset);

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

        model_matrix.translate(offset + cubePos);
        glUniformMatrix4fv(m_MMatrixUniform, 1, false, model_matrix.data());

        //generateBorderTriangles();

        QColor colors[7] = {
            Qt::black,
            Qt::red,
            Qt::blue,
            Qt::yellow,
            Qt::green,
            Qt::magenta,
            Qt::cyan
        };

        generateCube(colors[cell]);

        drawTriangles();
    }
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

// computes the vertices and corresponding colours-per-vertex for a quadrilateral
// drawn from (x1, y1) to (x2, y2)
// Note: the magic numbers in the vector insert commands should be better documented
void Renderer::generateBorderTriangles()
{
    // make sure array lists are clear to start with
    triVertices.clear();
    triColours.clear();

    // add vertices to rectangle list
    float vectList [] = {
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
    triVertices.insert(triVertices.end(), vectList, vectList + 3*4*3); // 36 items in array

    // shader supports per-vertex colour; add colour for each vertex add colours to colour list - use current colour
    QColor borderColour = Qt::red;
    float colourList [] = { (float)borderColour.redF(), (float)borderColour.greenF(), (float)borderColour.blueF() };
    float normalList [] = { 0.0f, 0.0f, 1.0f }; // facing viewer
    for (int v = 0; v < 4 * 3; v++)
    {
        triColours.insert(triColours.end(), colourList, colourList + 3); // 3 coordinates per vertex
        triNormals.insert(triNormals.end(), normalList, normalList + 3); // 3 coordinates per vertex
    }

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

    QPoint deltaPos = (event->pos() - prevMousePos) * 0.5f;

    if (event->buttons() & Qt::LeftButton)
    {
        rotation.setX(rotation.x() - deltaPos.y());
    }
    if (event->buttons() & Qt::MiddleButton)
    {
        rotation.setY(rotation.y() - deltaPos.x());
    }
    if (event->buttons() & Qt::RightButton)
    {
        rotation.setZ(rotation.z() + deltaPos.x());
    }
    prevMousePos = event->pos();
    paintGL();
}

void Renderer::resetView()
{
    rotation = QVector3D(0, 0, 0);
}

void Renderer::generateCube(QColor colour)
{
    // make sure array lists are clear to start with
    triVertices.clear();
    triColours.clear();

    // add vertices to triangle list
    float vectList [] = {
        // front
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,

        -0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,

        // right
        0.5f, 0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, -0.5f,

        0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,

        // left
        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, 0.5f,

        -0.5f, -0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        // up
        -0.5f, 0.5f, -0.5f,
        -0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, -0.5f,

        -0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, -0.5f,

        // down
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, 0.5f,

        -0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, 0.5f,

        // back
        -0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        };

    int numTri = 12;    // number of triangles to draw
    int numTriVerts = 3;

    triVertices.insert(triVertices.end(), vectList, vectList + numTri * numTriVerts * 3); // 108 items in array

    // shader supports per-vertex colour; add colour for each vertex add colours to colour list - use current colour

    float colourList [] = { (float)colour.redF(), (float)colour.greenF(), (float)colour.blueF() };
    float normalList [] = {
        0.0f, 0.0f, 1.0f,       // facing viewer
        1.0f, 0.0f, 0.0f,       // facing right
        -1.0f, 0.0f, 0.0f,      // facing left
        0.0f, 1.0f, 0.0f,       // facing up
        0.0f, -1.0f, 0.0f,      // facing down
        0.0f, 0.0f, -1.0f };    // facing back

    for (int v = 0; v < numTri * numTriVerts; v++)
    {
        int offset = v / (numTriVerts * 2) * 3;
        triColours.insert(triColours.end(), colourList, colourList + 3); // 3 coordinates per vertex
        triNormals.insert(triNormals.end(), normalList + offset, normalList + offset + 3); // 3 coordinates per vertex
    }
}

void Renderer::drawTriangles()
{
    // draw border
    if (triVertices.size() > 0)
    {
        // pass in the list of vertices and their associated colours
        // 3 coordinates per vertex, or per colour
        glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, &triVertices[0]);
        glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, &triColours[0]);
        glVertexAttribPointer(m_norAttr, 3, GL_FLOAT, GL_FALSE, 0, &triNormals[0]);

        glEnableVertexAttribArray(m_posAttr);
        glEnableVertexAttribArray(m_colAttr);
        glEnableVertexAttribArray(m_norAttr);

        // draw triangles
        glDrawArrays(drawMode, 0, triVertices.size()/3); // 3 coordinates per vertex

        glDisableVertexAttribArray(m_norAttr);
        glDisableVertexAttribArray(m_colAttr);
        glDisableVertexAttribArray(m_posAttr);
    }
}

void Renderer::setGame(Game *game)
{
    this->game = game;
}

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
        model_matrix.translate(cubePos + offset);
        glUniformMatrix4fv(m_MMatrixUniform, 1, false, model_matrix.data());
        drawTriangles();

        // right wall
        cubePos = QVector3D(width, i, 0.0f);
        model_matrix.setToIdentity();
        model_matrix.translate(cubePos + offset);
        glUniformMatrix4fv(m_MMatrixUniform, 1, false, model_matrix.data());
        drawTriangles();
    }

    // draw the well bottom
    for (i = 0; i < width ; i++)
    {
        QMatrix4x4 model_matrix;

        QVector3D cubePos = QVector3D(i, -1, 0.0f);
        model_matrix.translate(cubePos + offset);
        glUniformMatrix4fv(m_MMatrixUniform, 1, false, model_matrix.data());
        //generateCube(Qt::gray);
        drawTriangles();
    }
}

void Renderer::setDrawMode(int mode)
{
    drawMode = mode;
}

// Define the box's geometry (as triangles), normals, and colour
const float box_coords[] = {
    0,1,0,  1,1,0,  0,1,1, 1,1,1,   // top
    0,0,0,  1,0,0,  0,0,1, 1,0,1,   // bottom
    0,1,0,  0,0,1,  0,1,1, 0,0,0,   // left
    1,1,0,  1,0,1,  1,1,1, 1,0,0,   // right
    1,1,1,  1,0,1,  0,1,1, 0,0,1,   // front
    1,1,0,  1,0,0,  0,1,0, 0,0,0,   // back
};

const float box_norms[] = {
    0,1,0,   0,1,0,   0,1,0,   0,1,0,   // top
    0,-1,0,  0,-1,0,  0,-1,0,  0,-1,0,  // bottom
    -1,0,0,  -1,0,0,  -1,0,0,  -1,0,0,  // left
    1,0,0,   1,0,0,   1,0,0,   1,0,0,   // right
    0,0,1,   0,0,1,   0,0,1,   0,0,1,   // front
    0,0,-1,  0,0,-1,  0,0,-1,  0,0,-1,  // back
};

const float box_cols_green[] = {
    0,1,0,  0,1,0,  0,1,0,  0,1,0,
    0,1,0,  0,1,0,  0,1,0,  0,1,0,
    0,1,0,  0,1,0,  0,1,0,  0,1,0,

    0,1,0,  0,1,0,  0,1,0,  0,1,0,
    0,1,0,  0,1,0,  0,1,0,  0,1,0,
    0,1,0,  0,1,0,  0,1,0,  0,1,0,
};

void Renderer::setupBox()
{
    long cBufferSize = sizeof(box_cols_green) * sizeof(float);
    long vBufferSize = sizeof(box_coords) * sizeof(float);
    long nBufferSize = sizeof(box_norms) * sizeof(float);

    glGenBuffers(1, &this->m_boxVbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->m_boxVbo);

    // Allocate buffer
    glBufferData(GL_ARRAY_BUFFER, vBufferSize + cBufferSize + nBufferSize, NULL, GL_STATIC_DRAW);

    // Upload the data to the GPU
    glBufferSubData(GL_ARRAY_BUFFER, 0, vBufferSize, &box_coords[0]);
    glBufferSubData(GL_ARRAY_BUFFER, vBufferSize, cBufferSize, &box_cols_green[0]);
    glBufferSubData(GL_ARRAY_BUFFER, vBufferSize + cBufferSize, nBufferSize, &box_norms[0]);
}

void Renderer::drawBox()
{
    long cBufferSize = sizeof(box_cols_green) * sizeof(float);
    long vBufferSize = sizeof(box_coords) * sizeof(float);
    long nBufferSize = sizeof(box_norms) * sizeof(float);

    // Bind to the correct context
    glBindBuffer(GL_ARRAY_BUFFER, this->m_boxVbo);

    // Enable the attribute arrays
    glEnableVertexAttribArray(this->m_posAttr);
    glEnableVertexAttribArray(this->m_colAttr);
    glEnableVertexAttribArray(this->m_norAttr);

    // Specifiy where these are in the VBO
    glVertexAttribPointer(this->m_posAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)0);

    glVertexAttribPointer(this->m_colAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)(vBufferSize));

    glVertexAttribPointer(this->m_norAttr, 3, GL_FLOAT, 0, GL_FALSE, (const GLvoid*)(vBufferSize + cBufferSize));

    // Draw the triangles
    glDrawArrays(GL_QUADS, 0, 24); // 24 vertices

    glDisableVertexAttribArray(m_norAttr);
    glDisableVertexAttribArray(m_colAttr);
    glDisableVertexAttribArray(m_posAttr);
}
