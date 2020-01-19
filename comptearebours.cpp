#include "comptearebours.h"
#include "ui_comptearebours.h"
#include <string>
#include <iostream>

CompteARebours::CompteARebours(uint32_t startTime, QWidget *parent) :
    QWidget(parent),
    m_Time(startTime),
    m_StartTime(startTime),
    m_Interval(1000),
    m_HasStarted(false),
    ui(new Ui::CompteARebours)
{
    ui->setupUi(this);
    // adjust countdown label position
    //ui->label->move(80, 30);

    ui->label->setText(QString::number(startTime));
}

CompteARebours::~CompteARebours()
{
    delete ui;
}


void CompteARebours::SetInterval(int const& interval)
{
    m_Interval = interval;
}

void CompteARebours::SetFinishedHandler(std::function<void()> handlerFunction)
{
    m_CountdownFinishEvent = handlerFunction;
}

void CompteARebours::Start()
{
    m_Timer = new QTimer(this);
    connect(m_Timer, SIGNAL(timeout()), this, SLOT(Update()));
    m_HasStarted = true;
    m_Timer->start(m_Interval);
}


void CompteARebours::Stop()
{
    m_HasStarted = false;
}

void CompteARebours::Reset()
{
    m_Time = m_StartTime;
}

void CompteARebours::Update()
{
    if (m_HasStarted)
    {
        if (m_Time == 0)
        {
            Stop();

            // callback handler
            if (m_CountdownFinishEvent != nullptr)
            {
                m_CountdownFinishEvent();
            }
        }
        else
        {
            // subtract interval
            m_Time -= m_Interval / 1000;

            // update label
            QString labelText = QString::number(m_Time);
            ui->label->setText(labelText);
        }
    }
}
