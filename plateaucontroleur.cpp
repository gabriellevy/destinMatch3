#include "plateaucontroleur.h"
#include "ui_plateaucontroleur.h"
#include "modeleplateau.h"
#include "vueplateau.h"
#include "gemme.h"

PlateauControleur::PlateauControleur(QWidget *parent) :
    QWidget(parent),
    m_VuePlateau(new VuePlateau(8,8, this)),
    ui(new Ui::PlateauControleur),
    m_EtatPlateau(EtatPlateau::Uninitialized),
    m_ModelePlateau(new ModelePlateau(8,8)),
    m_DropDelay(100),
    m_RemoveSequenceDelay(250)
{
    ui->setupUi(this);

    // set view properties
    m_VuePlateau->SetPosition(330, 110);
    m_VuePlateau->SetWidth(340);
    m_VuePlateau->SetHeight(340);

    // register callbacks
    m_VuePlateau->SetGemsSelectedHandler(
        std::bind(&PlateauControleur::OnGemsSelected, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    m_ModelePlateau->SetColorAddedHandler(
                std::bind(&PlateauControleur::OnColorAdded, this, std::placeholders::_1, std::placeholders::_2));
    m_ModelePlateau->SetColorRemovedHandler(
                std::bind(&PlateauControleur::OnColorRemoved, this, std::placeholders::_1, std::placeholders::_2));
    m_ModelePlateau->SetColorDroppedHandler(
                std::bind(&PlateauControleur::OnColorDropped, this, std::placeholders::_1, std::placeholders::_2));
    m_ModelePlateau->SetSequenceFoundHandler(
                std::bind(&PlateauControleur::OnSequenceFound, this, std::placeholders::_1, std::placeholders::_2));

}

void PlateauControleur::Initialiser()
{
    // populate the board
    m_ModelePlateau->Populate();

    // board dimensions
    uint8_t columns = m_ModelePlateau->GetColumns();
    uint8_t rows = m_ModelePlateau->GetRows();

    // add a gem representation to the view
    for (uint8_t y = 0; y < rows; ++y)
    {
        for (uint8_t x = 0; x < columns; ++x)
        {
            uint8_t color = m_ModelePlateau->GetColor(x, y);
            m_VuePlateau->AddGem(x, y, Gemme::Color(color));
        }
    }

    // controller is ready for playing
    m_EtatPlateau = EtatPlateau::Ready;
}

void PlateauControleur::Geler(const bool& estGele)
{
    if (estGele == true)
        // controller is now freezed
        m_EtatPlateau = EtatPlateau::Freeze;
    else
        // enable play
        m_EtatPlateau = EtatPlateau::Ready;
}

PlateauControleur::~PlateauControleur()
{
    delete ui;
    delete m_ModelePlateau;
}


void PlateauControleur::Update()
{
    switch (m_EtatPlateau)
    {
    case EtatPlateau::Freeze:
        // make sure view is not accepting user input
        if (m_VuePlateau->IsInputEnabled() == true)
            m_VuePlateau->EnableInput(false);
        break;
    case EtatPlateau::Ready:
        // make sure view is accepting input
        if (m_VuePlateau->IsInputEnabled() == false)
            m_VuePlateau->EnableInput(true);
        break;
    case EtatPlateau::GemsSelected:
        // try to swap selected gems
        SwapGems();
        break;
    case EtatPlateau::GemsSwapped:
        // find sequences
        FindSequences();
        break;
    case EtatPlateau::SequenceDetected:
        // remove all sequences
        RemoveSequences();
        break;
    case EtatPlateau::SequencesRemoved:
        //drop gems one row at a time until no more holes are left
        DropGems();
        break;
    case EtatPlateau::GemsDopped:
        // find sequences
        FindSequences();
        break;
    default:
        break;
    }
    /* in every update cycle:
    1. Check if gems have been swapped
        yes ->
        - detect and remove sequences
    2. Check if sequences have been removed

    */
}

void PlateauControleur::OnGemsSelected(const int& sourceX, const int& sourceY, const int& targetX, const int& targetY)
{
    // store coordinates of source and target cell
    m_SelectedSourceCellX = sourceX;
    m_SelectedSourceCellY = sourceY;
    m_SelectedTargetCellX = targetX;
    m_SelectedTargetCellY = targetY;

    // gem have been selected
    m_EtatPlateau = EtatPlateau::GemsSelected;
}

void PlateauControleur::SwapGems()
{
    // check if the attempted swap leads to a sequence
    bool isSwapValid = m_ModelePlateau->SwapColors(m_SelectedSourceCellX, m_SelectedSourceCellY, m_SelectedTargetCellX, m_SelectedTargetCellY);

    // get the corresponding gems
    Gemme* firstGem = m_VuePlateau->GetGem(m_SelectedSourceCellX, m_SelectedSourceCellY);
    Gemme* secondGem = m_VuePlateau->GetGem(m_SelectedTargetCellX, m_SelectedTargetCellY);

    if (isSwapValid)
    {
        // swap gems in view
        m_VuePlateau->MoveGem(firstGem, m_SelectedTargetCellX, m_SelectedTargetCellY);
        m_VuePlateau->MoveGem(secondGem, m_SelectedSourceCellX, m_SelectedSourceCellY);

        // gems have been swapped successfully
        m_EtatPlateau = EtatPlateau::GemsSwapped;
    }
    else
    {
        // move was invalid and board is ready for a new selection
        m_EtatPlateau = EtatPlateau::Ready;
    }
}

void PlateauControleur::FindSequences()
{
    // find matching color sequences
    m_ModelePlateau->DetectColorSequences();

    if (m_ModelePlateau->ContainsSequences() == true)
        m_EtatPlateau = EtatPlateau::SequenceDetected;
    else
        m_EtatPlateau = EtatPlateau::Ready;
}

void PlateauControleur::RemoveSequences()
{
    // add some delay before gems are removed:
    // the user get a better understanding of the action as removed gems get highlighted in the view
    if (!m_TimerRemoveSequence.isValid())
    {
        //m_TimerRemoveSequence.start();
        m_TimerRemoveSequence.restart();
    }
    else
    {
        // if delay is over, indicate that all sequences have been removed
        if (m_TimerRemoveSequence.elapsed() > m_RemoveSequenceDelay )
        {
            // remove all sequences
            m_ModelePlateau->RemoveSequences();

            m_EtatPlateau = EtatPlateau::SequencesRemoved;

            // reset start time
            m_TimerRemoveSequence.invalidate();
        }
    }
}

void PlateauControleur::DropGems()
{
    // as long as there are any holes in the board, keep dropping colors
    if (m_ModelePlateau->GetHoles() > 0)
    {
        // add some delay each time gems are dropped
        if (!m_TimerDropGems.isValid())
        {
            m_TimerDropGems.restart();
        }
        else
        {
            if (m_TimerDropGems.elapsed() > m_DropDelay )
            {
                m_ModelePlateau->DropColors();

                // reset start time
                m_TimerDropGems.invalidate();
            }
        }

    }
    else
    {
        m_EtatPlateau = EtatPlateau::GemsDopped;
    }
}

void PlateauControleur::OnColorAdded(const int& Column, const int& Row)
{
    // retrieve color
    uint8_t color = m_ModelePlateau->GetColor(Column, Row);

    // add a new gem to the view
    m_VuePlateau->AddGem(Column, Row, Gemme::Color(color));
}

void PlateauControleur::OnColorRemoved(int const& column, int const& row)
{
    // gems that are going to be removed get hightlighted
    m_VuePlateau->RemovedGem(column, row);
}


void PlateauControleur::OnColorDropped(const int& column, const int& row)
{
    //retrieve gem at position
    Gemme* gem = m_VuePlateau->GetGem(column, row);
    // move down one row
    m_VuePlateau->MoveGem(gem, column, row + 1);
}

void PlateauControleur::OnSequenceFound(const int& column, const int& row)
{
    m_VuePlateau->MarkRemovedGem(column, row);
}
