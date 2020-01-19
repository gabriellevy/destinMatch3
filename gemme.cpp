#include "gemme.h"
#include "ui_gemme.h"

Gemme::Gemme(Color color, QWidget *parent) :
    QWidget(parent),
    m_Color(color),
    m_State(State::Normal),
    m_IsInUse(false),
    ui(new Ui::Gemme)
{
    ui->setupUi(this);
}

Gemme::~Gemme()
{
    delete ui;
}

void Gemme::SetState(State newState)
{
    m_State = newState;
}

Gemme::State const& Gemme::GetState()
{
    return m_State;
}

void Gemme::SetQPixmap(QPixmap const qpixmap)
{
    ui->gemmeLabel->setPixmap(qpixmap);
}

void Gemme::Render()
{
    /*if (m_State == State::Highlighted)
    {
        m_Sprite->SetColor(200,200,200);
    }
    else
    {
        m_Sprite->SetColor(255, 255, 255);
    }*/

    //m_Sprite->Render();
}

