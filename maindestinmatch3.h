#ifndef MAINDESTINMATCH3_H
#define MAINDESTINMATCH3_H

#include <QMainWindow>
#include "plateaucontroleur.h"
#include "comptearebours.h"
#include <QKeyEvent>

namespace Ui {
class MainDestinMatch3;
}

class MainDestinMatch3 : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainDestinMatch3(QWidget *parent = nullptr);
    ~MainDestinMatch3();

private:
    Ui::MainDestinMatch3 *ui;

    PlateauControleur* m_PlateauControleur;
    CompteARebours* m_CompteARebours;

    // flags for determining the current game state
    bool m_IsRunning;
    bool m_HasGameStarted;
    bool m_HasGameFinished;

    // start screen
    /*Sprite* m_OverlayBackground;
    Label* m_WelcomeLabel;
    Label* m_StartGameLabel;

    // game over screen
    Label* m_GameOverLabel;
    Label* m_PlayAgainLabel;*/

public:

    void keyPressEvent(QKeyEvent *event);

    bool Initialiser();

    // logic loop
    void Update();

    void Render();

    // free all resources
    void CleanUp();

    // callback function that gets invokes when countdown has reached zero
    void OnCountdownFinished();

    bool Demarrer();

public slots:
    /*! \brief Executes the application
    *
    *	\return Returns 0 if everything is ok and 1 if there was an error
    */
    void BouclePrincipale();
};

#endif // MAINDESTINMATCH3_H
