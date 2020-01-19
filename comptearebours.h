#ifndef COMPTEAREBOURS_H
#define COMPTEAREBOURS_H

#include <QWidget>
#include <functional>
#include <QTimer>

namespace Ui {
class CompteARebours;
}

class CompteARebours : public QWidget
{
    Q_OBJECT

public:
    explicit CompteARebours(uint32_t startTime, QWidget *parent);

    virtual ~CompteARebours();

    /*! \brief Set the countdown interval
    *
    * \param interval The interval in milliseconds
    */
    void SetInterval(int const& interval);

    /*! Starts  timer */
    void Start();

    /*! Stops  timer	*/
    void Stop();

    /*! Resets timer to start time	*/
    void Reset();

    /*! Renders the countdown label */
    //void Render();

    /*! Registers a handler that gets called when time is over */
    void SetFinishedHandler(std::function<void()> handlerFunction);

private:
    uint32_t m_Time;
    uint32_t m_StartTime;
    int m_Interval;
    QTimer* m_Timer;

    bool m_HasStarted;

    std::function<void()> m_CountdownFinishEvent;
    Ui::CompteARebours *ui;

private slots:
    /*! Updates timer logic	*/
    void Update();

};

#endif // COMPTEAREBOURS_H
