#ifndef PLATEAUCONTROLEUR_H
#define PLATEAUCONTROLEUR_H

#include <QWidget>
#include <QElapsedTimer>

class ModelePlateau;
class VuePlateau;

namespace Ui {
class PlateauControleur;
}

class PlateauControleur : public QWidget
{
    Q_OBJECT

public:
    explicit PlateauControleur(QWidget *parent = nullptr);
    ~PlateauControleur();

    VuePlateau* m_VuePlateau;

    /*! \brief The various states of the board
    *
    */
    enum class EtatPlateau { Uninitialized, Freeze, Ready, GemsSwapped, GemsSelected, SequenceDetected, SequencesRemoved, GemsDopped };

    void Initialiser();
    void Geler(const bool& estGele);

    /*! \brief Processes the game logic
    *
    */
    void Update();

    void FindSequences();
    void RemoveSequences();
    void DropGems();

    void OnGemsSelected(const int& sourceX, const int& sourceY, const int& targetX, const int& targetY);
    void SwapGems();
    void OnColorAdded(const int& Column, const int& Row);
    void OnColorRemoved(int const& column, int const& row);
    void OnColorDropped(const int& column, const int& row);
    void OnSequenceFound(const int& column, const int& row);

private:
    Ui::PlateauControleur *ui;

    EtatPlateau m_EtatPlateau;
    ModelePlateau* m_ModelePlateau;

    // amount of delay between dropping one row of gems in milliseconds
    int m_DropDelay;
    QElapsedTimer m_TimerDropGems;

    // amount of delay after removing a sequence of gems in milliseconds
    QElapsedTimer m_TimerRemoveSequence;
    int m_RemoveSequenceDelay;

    // store selected gems
    int m_SelectedSourceCellX;
    int m_SelectedSourceCellY;
    int m_SelectedTargetCellX;
    int m_SelectedTargetCellY;
};

#endif // PLATEAUCONTROLEUR_H
