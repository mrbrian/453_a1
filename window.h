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
    // game update function
    void update();
    // restarts the game
    void newGame();
    void setDrawMode(QAction * action);
    void pause();
protected:

    virtual void keyPressEvent(QKeyEvent * event);
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

    QTimer * timer;
    Game * game;
    // helper function for creating actions
    void createActions();
};

#endif // WINDOW_H
