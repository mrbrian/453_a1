#include "window.h"
#include "renderer.h"

#define INIT_TICK_SPEED 500

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
    connect(timer, SIGNAL(timeout()), this, SLOT(gameUpdate()));
    tickSpeed = INIT_TICK_SPEED;
    timer->start(INIT_TICK_SPEED);


    // Setup the quit button
    scoreLabel = new QLabel(this);
    //connect(quitButton, SIGNAL(clicked()), qApp, SLOT(quit()));

    // Add quit button
    score = 0;
    layout->addWidget(scoreLabel);
    scoreLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    scoreLabel->setText("Score: 0");
    scoreLabel->setAlignment(Qt::AlignBottom | Qt::AlignRight);
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

    // Speed up the gameplay
    mSpeedUpAction = new QAction(tr("&Speed Up"), this);
    mSpeedUpAction->setShortcut(QKeySequence(Qt::Key_PageUp));
    mSpeedUpAction->setStatusTip(tr("Increase the speed of the game play"));
    connect(mSpeedUpAction, SIGNAL(triggered()), this, SLOT(incSpeed()));

    // Slow down the gameplay
    mSlowDownAction = new QAction(tr("&Speed Down"), this);
    mSlowDownAction->setShortcut(QKeySequence(Qt::Key_PageDown));
    mSlowDownAction->setStatusTip(tr("Decrease the speed of the game play"));
    connect(mSlowDownAction, SIGNAL(triggered()), this, SLOT(decSpeed()));

    // Auto increase difficulty
    mAutoIncAction = new QAction(tr("&Auto-Increase"), this);
    mAutoIncAction->setShortcut(QKeySequence(Qt::Key_A));
    mAutoIncAction->setStatusTip(tr("Automatically increase the speed"));
    connect(mAutoIncAction, SIGNAL(triggered()), this, SLOT(toggleAutoSpeed()));
}

// destructor
Window::~Window()
{
    delete renderer;
}

// Restarts the game
void Window::newGame()
{
    score = 0;
    tickSpeed = INIT_TICK_SPEED;
    game->reset();
}

// Game updating function
void Window::update()
{
    renderer->update();
    QMainWindow::update();
}

// Game updating function
void Window::gameUpdate()
{
    score += game->tick();
    if (autoSpeed)
    {
        tickSpeed = std::max(25, 500 - (score * 100));
        timer->setInterval(tickSpeed);
    }
    scoreLabel->setText("Score: " + QString::number(score));
}

void Window::incSpeed()
{
    tickSpeed -= 50;
    tickSpeed = std::max(25, tickSpeed - 50);
    timer->setInterval(tickSpeed);
}

void Window::toggleAutoSpeed()
{
    autoSpeed = true;
    tickSpeed = std::max(25, 500 - (score * 50));
    timer->setInterval(tickSpeed);
}

void Window::decSpeed()
{
    tickSpeed += 50;
    timer->setInterval(tickSpeed);
}

void Window::pause()
{
    if (!timer->isActive())
        timer->start(tickSpeed);
    else
        timer->stop();
}

void Window::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case int(Qt::Key_Shift):
            renderer->setIsScaling(true);
            break;
        case int(Qt::Key_Left):
            game->moveLeft();
            break;
        case int(Qt::Key_Right):
            game->moveRight();
            break;
        case int(Qt::Key_Up):
            game->rotateCCW();
            break;
        case int(Qt::Key_Down):
            game->rotateCW();
            break;
        case int(Qt::Key_Space):
            game->drop();
            break;
        default:
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
