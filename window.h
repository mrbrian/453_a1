/*
 * CPSC 453 - Introduction to Computer Graphics
 * Assignment 1
 *
 * Window - Fundamental GUI for interacting within the scene
 */

#ifndef WINDOW_H
#define WINDOW_H

#include "game.h"
#include <QMainWindow>
#include <QApplication>
#include <QMenuBar>
#include <QAction>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QActionGroup>
#include <QTimer>

class Renderer;

class Window : public QMainWindow
{
    // informs the qmake that a Qt moc_* file will need to be generated
    Q_OBJECT

public:
    // constructor
    Window(QWidget *parent = 0);

    // destructor
    ~Window();


private slots:
    // game updte function
    void gameUpdate();

    // restarts the game
    void newGame();
    // sets the draw mode of the renderer
    void setDrawMode(QAction * action);
    // pauses game
    void pause();
    // increases game speed
    void incSpeed();
    // decreases game speed
    void decSpeed();
    // increases the game speed slowly over time
    void toggleAutoSpeed();

protected:
    virtual void keyPressEvent(QKeyEvent * event);
    virtual void keyReleaseEvent(QKeyEvent * event);

private:
    // Main widget for drawing
    Renderer *renderer;

    // Menu items and actions
    QMenu * mFileMenu;
    QAction * mNewGameAction;
    QAction * mResetAction;
    QAction * mQuitAction;

    QActionGroup * mDrawGroup;
    QMenu * mDrawMenu;
    QAction * mWireAction;
    QAction * mFaceAction;
    QAction * mMultiAction;

    QMenu * mGameMenu;
    QAction * mPauseAction;
    QAction * mSpeedUpAction;
    QAction * mSlowDownAction;
    QAction * mAutoIncAction;

    // timer for calling game update function
    QTimer * gameTimer;

    // auto speed increasing flag
    bool autoSpeed;

    // current game tick speed
    int tickDelay;

    // Game reference
    Game * game;

    // Game score
    int score;
    // Score UI label
    QLabel * scoreLabel;

    // helper function for creating actions
    void createActions();
};

#endif // WINDOW_H
