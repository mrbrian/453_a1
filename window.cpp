#include "window.h"
#include "renderer.h"

Window::Window(QWidget *parent) :
    QMainWindow(parent)
{
    setWindowTitle("CPSC453: Tetromino Apocalypse");

    // Create the main drawing object
    renderer = new Renderer();

    // Create the actions to be used by the menus
    createActions();

    // Create the menus
    // Setup the file menu
    mFileMenu = menuBar()->addMenu(tr("&File"));
    mFileMenu->addAction(mNewGameAction);  // add new game
    mFileMenu->addAction(mResetAction);  // add reset
    mFileMenu->addAction(mQuitAction);  // add quitting

    // Setup the draw menu
    mDrawMenu = menuBar()->addMenu(tr("&Draw"));
    mDrawMenu->addAction(mWireAction);  // add wire
    mDrawMenu->addAction(mFaceAction);  // add wire
    mDrawMenu->addAction(mMultiAction);  // add wire

    // Setup the game menu
    mGameMenu = menuBar()->addMenu(tr("&Game"));
    mGameMenu->addAction(mPauseAction);  // add pausing
    mGameMenu->addAction(mSpeedUpAction);  // add pausing
    mGameMenu->addAction(mSlowDownAction);  // add pausing
    mGameMenu->addAction(mAutoIncAction);  // add pausing

    // Setup the application's widget collection
    QVBoxLayout * layout = new QVBoxLayout();

    // Add renderer
    layout->addWidget(renderer);
    renderer->setMinimumSize(300, 600);

    QWidget * mainWidget = new QWidget();
    mainWidget->setLayout(layout);
    setCentralWidget(mainWidget);

    // Create game object
    game = new Game(10, 20);
    renderer->setGame(game);

    // Setup the game timer
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    tickSpeed = 500;
    timer->start(tickSpeed);
}

// helper function for creating actions
void Window::createActions()
{
    // Quits the application
    mQuitAction = new QAction(tr("&Quit"), this);
    mQuitAction->setShortcut(QKeySequence(Qt::Key_Q));
    mQuitAction->setStatusTip(tr("Quits the application"));
    connect(mQuitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    // Starts a new game
    mNewGameAction = new QAction(tr("&New Game"), this);
    mNewGameAction->setShortcut(QKeySequence(Qt::Key_N));
    mNewGameAction->setStatusTip(tr("Starts a new game"));
    connect(mNewGameAction, SIGNAL(triggered()), this, SLOT(newGame()));

    // Resets the application
    mResetAction = new QAction(tr("&Reset"), this);
    mResetAction->setShortcut(QKeySequence(Qt::Key_R));
    mResetAction->setStatusTip(tr("Reset the view of the game"));
    connect(mResetAction, SIGNAL(triggered()), renderer, SLOT(resetView()));

    // Toggles to line drawing
    mDrawGroup = new QActionGroup(this);
    connect(mDrawGroup, SIGNAL(triggered(QAction *)), this, SLOT(setDrawMode(QAction *)));

    // Sets drawing mode to wireframe
    mWireAction = new QAction(tr("&Wireframe"), this);
    mWireAction->setShortcut(QKeySequence(Qt::Key_W));
    mWireAction->setStatusTip(tr("Wireframe mode"));
    mDrawGroup->addAction(mWireAction);

    mFaceAction = new QAction(tr("&Face"), this);
    mFaceAction->setShortcut(QKeySequence(Qt::Key_F));
    mFaceAction->setStatusTip(tr("Face mode"));
    mDrawGroup->addAction(mFaceAction);

    mMultiAction = new QAction(tr("&Multicoloured"), this);
    mMultiAction->setShortcut(QKeySequence(Qt::Key_M));
    mMultiAction->setStatusTip(tr("Multicoloured mode"));
    mDrawGroup->addAction(mMultiAction);

    // Pauses the game
    mPauseAction = new QAction(tr("&Pause"), this);
    mPauseAction->setShortcut(QKeySequence(Qt::Key_P));
    mPauseAction->setStatusTip(tr("Pause game"));
    connect(mPauseAction, SIGNAL(triggered()), this, SLOT(pause()));

    mSpeedUpAction = new QAction(tr("&Speed Up"), this);
    mSpeedUpAction->setShortcut(QKeySequence(Qt::Key_PageUp));
    mSpeedUpAction->setStatusTip(tr("Increase the speed of the game play"));
    connect(mSpeedUpAction, SIGNAL(triggered()), this, SLOT(incSpeed()));

    mSlowDownAction = new QAction(tr("&Speed Down"), this);
    mSlowDownAction->setShortcut(QKeySequence(Qt::Key_PageDown));
    mSlowDownAction->setStatusTip(tr("Decrease the speed of the game play"));
    connect(mSlowDownAction, SIGNAL(triggered()), this, SLOT(decSpeed()));

    mAutoIncAction = new QAction(tr("&Auto-Increase"), this);
    mAutoIncAction->setShortcut(QKeySequence(Qt::Key_A));
    mAutoIncAction->setStatusTip(tr("Automatically increase the speed"));
    connect(mAutoIncAction, SIGNAL(triggered()), this, SLOT(decSpeed()));
}

// destructor
Window::~Window()
{
    delete renderer;
}

// Restarts the game
void Window::newGame()
{
    game->reset();
}

// Game updating function
void Window::update()
{
    game->tick();
    renderer->update();
}

void Window::incSpeed()
{
    tickSpeed += 100;
}

void Window::decSpeed()
{
    tickSpeed -= 100;
}

void Window::pause()
{
    if (!timer->isActive())
        timer->start(500);
    else
        timer->stop();
}

void Window::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == int(Qt::Key_Shift))
        renderer->setIsScaling(true);
    else if (event->key() == int(Qt::Key_Left))
        game->moveLeft();
    else if (event->key() == int(Qt::Key_Right))
        game->moveRight();
    else if (event->key() == int(Qt::Key_Up))
        game->rotateCCW();
    else if (event->key() == int(Qt::Key_Down))
        game->rotateCW();
    else if (event->key() == int(Qt::Key_Space))
        game->drop();
    else
    {
        QMainWindow::keyPressEvent(event);
        return;
    }
    renderer->update();
}

void Window::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == int(Qt::Key_Shift))
        renderer->setIsScaling(false);
}

// Sets the given colour
void Window::setDrawMode(QAction * action)
{
    if (action == mWireAction)
        renderer->setDrawMode(Renderer::DRAW_WIRE);
    else if (action == mFaceAction)
        renderer->setDrawMode(Renderer::DRAW_FACES);
    else
        renderer->setDrawMode(Renderer::DRAW_MULTI);

}
