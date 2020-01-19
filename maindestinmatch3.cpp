#include "maindestinmatch3.h"
#include "ui_maindestinmatch3.h"
#include <QDebug>
#include "vueplateau.h"
#include <QVBoxLayout>

MainDestinMatch3::MainDestinMatch3(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainDestinMatch3)
{
    ui->setupUi(this);

    Demarrer();
}

bool MainDestinMatch3::Demarrer()
{
    // try to initialize the app
    if (Initialiser() != true)
    {
        return false;
    }

    QTimer* bouclePrincipale = new QTimer();
    connect(bouclePrincipale, SIGNAL(timeout()), this, SLOT(BouclePrincipale()));
    bouclePrincipale->start(30);

    // free resources
    //CleanUp();

    return true;
}

MainDestinMatch3::~MainDestinMatch3()
{
    delete ui;
}

void MainDestinMatch3::CleanUp()
{
    /*delete m_PlayAgainLabel;
    delete m_GameOverLabel;
    delete m_StartGameLabel;
    delete m_WelcomeLabel;
    delete m_OverlayBackground;
    delete m_Background;
    delete m_BoardController;*/
    delete m_CompteARebours;
}

void MainDestinMatch3::Update()
{
    m_PlateauControleur->Update();
    //m_CompteARebours->Update(); // fait par signal normalement donc pas nécessaire
}

void MainDestinMatch3::OnCountdownFinished()
{
    m_HasGameFinished = true;
    // freeze board
    m_PlateauControleur->Geler(true);
    m_CompteARebours->Reset();
}


bool MainDestinMatch3::Initialiser()
{
    // create and setup a countdown timer
    m_CompteARebours = new CompteARebours(60, ui->centralWidget);

    m_CompteARebours->SetInterval(1000);
    // register callback
    m_CompteARebours->SetFinishedHandler(
                [this] {
        this->OnCountdownFinished();
    });

    m_PlateauControleur = new PlateauControleur(ui->centralWidget);

    // init to fill board with gems
    m_PlateauControleur->Initialiser();
    m_PlateauControleur->Geler(true);

    // setup overlay for start and game over screen
    /*m_OverlayBackground = new Sprite();
    m_OverlayBackground->Load(OVERLAY_PATH);
    m_OverlayBackground->SetWidth(m_WindowWidth);
    m_OverlayBackground->SetHeight(m_WindowHeight);

    // labels for start screen
    m_WelcomeLabel = new Label(40);
    m_WelcomeLabel->SetText("Welcome to Gem Match");
    m_WelcomeLabel->SetColor(0, 60, 80);
    m_WelcomeLabel->SetPosition(50, 100);

    m_StartGameLabel = new Label(30);
    m_StartGameLabel->SetText("Press <Space> to start");
    m_StartGameLabel->SetColor(10, 120, 10);
    m_StartGameLabel->SetPosition(100, 400);

    //labels for game over screen
    m_GameOverLabel = new Label(70);
    m_GameOverLabel->SetText("Game Over");
    m_GameOverLabel->SetColor(120, 0, 0);
    m_GameOverLabel->SetPosition(100, 100);

    m_PlayAgainLabel = new Label(30);
    m_PlayAgainLabel->SetText("Press <Space> to play again");
    m_PlayAgainLabel->SetColor(10, 120, 10);
    m_PlayAgainLabel->SetPosition(100, 400);*/

    // game has not started yet
    m_HasGameStarted = false;
    // game has not finished yet
    m_HasGameFinished = false;
    // game is now up and running
    m_IsRunning = true;

    return true;
}

void MainDestinMatch3::keyPressEvent(QKeyEvent *event)
{
    if( event->key() == Qt::Key_Space )
    {
        qDebug()<<"keyPressEvent space"<<endl;
        // game starts for the first time
        if (m_HasGameStarted == false )
        {
            // unfreeze board
            m_PlateauControleur->Geler(false);
            // start countdown
            m_CompteARebours->Start();

            m_HasGameStarted = true;
        }
        // game has not started yet
        if (m_HasGameFinished == true )
        {
            // init again to populate board with new gems
            m_PlateauControleur->Initialiser();

            // unfreeze board
            m_PlateauControleur->Geler(false);

            // reset & start countdown
            m_CompteARebours->Reset();
            m_CompteARebours->Start();

            m_HasGameFinished = false;
        }
    }
}

void MainDestinMatch3::Render()
{

    m_PlateauControleur->m_VuePlateau->Render();
    //m_CompteARebours->Render();

    // show a welcome screen, if the game is up for the first time
    /*if (m_HasGameStarted == false)
    {
        m_OverlayBackground->Render();
        m_WelcomeLabel->Render();
        m_StartGameLabel->Render();
    }

    // show game over screen when game is finished
    if (m_HasGameFinished == true)
    {
        m_OverlayBackground->Render();
        m_GameOverLabel->Render();
        m_PlayAgainLabel->Render();
    }*/
}

void MainDestinMatch3::BouclePrincipale()
{
    // main loop
    if (m_IsRunning)
    {
        // check if user closes application
        /* TODO MATHIEU : gérer la fin d'exécution
         * if (event.type == SDL_QUIT)
        {
            m_IsRunning = false;
        }*/

        // update logic
        Update();

        // render
        Render();
    }
}
