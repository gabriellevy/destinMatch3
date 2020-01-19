#include "modeleplateau.h"
#include "gemme.h"
#include "modeleplateau.h"
#include <QDebug>
#include "../destinLib/aleatoire.h"

ModelePlateau::ModelePlateau(const uint8_t& colonnes, const uint8_t& lignes) :
    m_Colonnes(colonnes),
    m_Lignes(lignes),
    m_Troues(0)
{
    // create array of colors
    m_Colors = new uint8_t[m_Colonnes * m_Lignes];

    // allocate enough memory for the vector that holds valid color sequences
    m_ColorSequences.reserve(m_Colonnes * m_Lignes);

    // set valid color sequences to false
    for (int i = 0; i < m_Colonnes * m_Lignes; ++i)
    {
        bool isValidSequence = false;
        m_ColorSequences.push_back(isValidSequence);
    }
}

ModelePlateau::~ModelePlateau()
{
    m_ColorSequences.clear();

    // delete m_Colors array
    delete[] m_Colors;
}



bool ModelePlateau::SwapColors(uint8_t const& sourceX, uint8_t const& sourceY, uint8_t const& targetX, uint8_t const& targetY)
{
    // check if source color is valid
    if (sourceX < 0 || sourceX > m_Colonnes -1 || sourceY < 0 || sourceY > m_Lignes - 1 )
        return false;

    // check if target color is valid
    if (targetX < 0 || targetX > m_Colonnes -1  || targetY < 0 || targetY > m_Lignes - 1 )
        return false;

    int sourcePosition = sourceY * m_Colonnes + sourceX;
    int targetPosition = targetY * m_Colonnes + targetX;

    // attempt to swap colors - check if the swap leads to a sequence
    uint8_t sourceColor = m_Colors[sourceY * m_Colonnes + sourceX];
    uint8_t targetColor = m_Colors[targetY * m_Colonnes + targetX];

    // swap color
    m_Colors[sourcePosition] = targetColor;
    m_Colors[targetPosition] = sourceColor;

    // check if one of them is now part of a sequence
    if (IsPartOfSequence(sourceX, sourceY) || IsPartOfSequence(targetX, targetY))
    {
        return true;
    }

    else
    {
        // reverse swap
        m_Colors[sourcePosition] = sourceColor;
        m_Colors[targetPosition] = targetColor;

        return false;
    }
}

void ModelePlateau::Populate()
{
    // traverse the board rows to find horizontal color sequences
    for (int y = 0; y < m_Lignes; ++y)
    {
        for (int x = 0; x < m_Colonnes; ++x)
        {
            int position = y * m_Colonnes + x;

            // generate a random color for this field
            m_Colors[position] = GetRandomColor();

            // check if a horizontal sequence has been created and remove it
            if (x > 1)
            {
                if (m_Colors[position] == m_Colors[y * m_Colonnes + x - 1] && m_Colors[y * m_Colonnes + x - 2])
                {
                    uint8_t newColor = m_Colors[position];

                    // get a new random color that is different and does not create a sequence
                    while (newColor == m_Colors[position])
                    {
                        newColor = GetRandomColor();
                    }

                    m_Colors[position] = newColor;
                }
            }

            if (y > 1)
            {
                if (m_Colors[position] == m_Colors[(y - 1) * m_Colonnes + x] && m_Colors[(y - 2) * m_Colonnes + x])
                {
                    uint8_t newColor = m_Colors[position];

                    // get a new random color that is different and does not create a sequence
                    while (newColor == m_Colors[position] || newColor == m_Colors[(y - 1) * m_Colonnes + x-1])
                    {
                        newColor = GetRandomColor();
                    }

                    m_Colors[position] = newColor;
                }
            }
        }
    }

    m_Troues = 0;
}

void ModelePlateau::DetectColorSequences()
{
    // traverse the board rows to find horizontal color sequences
    for (int y = 0; y < m_Lignes; ++y)
    {
        for (int x = 0; x < m_Colonnes; ++x)
        {
            int position = y * m_Colonnes + x;
            int color = m_Colors[position];

            // look in this row for a sequence

            int sequenceCountX = 1;
            int sequenceCountY = 1;

            // look horizontally - there is no need to look up the last two columns
            if (x < m_Colonnes - 2)
            {
                int matchingColor = color;

                while (true)
                {
                    int searchPosition = position + sequenceCountX;

                    // make sure to look for a sequence in the same row
                    if (position / m_Lignes != searchPosition / m_Lignes)
                        break;

                    // get color of search position
                    if (searchPosition < m_Colonnes * m_Lignes)
                        matchingColor = m_Colors[searchPosition];

                    // stop looking if the colors don't match
                    if (matchingColor != color)
                        break;
                    else
                        ++sequenceCountX;
                }
            }

            // look vertically - there is no need to look up the last two rows
            if (y < m_Lignes - 2)
            {
                int matchingColor = color;

                while (true)
                {
                    int searchPosition = position + sequenceCountY * m_Colonnes;

                    // get color of search position
                    if (searchPosition < m_Colonnes * m_Lignes)
                        matchingColor = m_Colors[searchPosition];
                    else
                        break;
                    // stop looking if the colors don't match
                    if (matchingColor != color)
                        break;
                    else
                        ++sequenceCountY;
                }
            }

            // check if a sequence of at least 3 horizontal matching color has been found
            if (sequenceCountX >= 3)
            {
                for (int sequence = 0; sequence < sequenceCountX; ++sequence)
                {
                    int sequencePosition = position + sequence;
                    m_ColorSequences[sequencePosition] = true;

                    // call handler
                    SequenceFound(x + sequence, y);
                }
            }
            // check if a sequence of at least 3 vertical matching color has been found
            if (sequenceCountY >= 3)
            {
                for (int sequence = 0; sequence < sequenceCountY; ++sequence)
                {
                    int sequencePosition = position + sequence * m_Colonnes;
                    m_ColorSequences[sequencePosition] = true;

                    // call handler
                    SequenceFound(x, y + sequence);
                }
            }
        }
    }
}

bool const ModelePlateau::ContainsSequences()
{
    // iterate through color sequences
    for (QVector<bool>::iterator it = m_ColorSequences.begin(); it != m_ColorSequences.end(); ++it)
    {
        // found the beginning of a color sequence
        if (*it == true)
            return true;
    }

    return false;
}

void ModelePlateau::RemoveSequences()
{
    for (int i = 0; i < m_Lignes * m_Colonnes; ++i)
    {
        if (m_ColorSequences.at(i) == true)
        {
            m_Colors[i] = -1;

            // invoke removal event
            ColorRemoved(i % m_Colonnes, i / m_Lignes);

            //track how many holes there are after removal
            ++m_Troues;
        }
    }

    //reset valid color sequences
    for (QVector<bool>::iterator it = m_ColorSequences.begin(); it != m_ColorSequences.end(); ++it)
    {
        *it = false;
    }
}

uint8_t ModelePlateau::GetColor(const uint8_t& column, const uint8_t& row)
{
    if (row >= 0 && row < m_Lignes && column >= 0 && column < m_Colonnes)
    {
        return m_Colors[row * m_Colonnes + column];
    }
    else
    {
        return 0;
    }
}

void ModelePlateau::PrintValidSequences()
{

    for (int i = 0; i < m_Lignes * m_Colonnes; ++i)
    {
        if ((i % m_Colonnes) == 0)
            qDebug() << endl;

        qDebug() << m_ColorSequences.at(i) << " ";
    }
    qDebug() << endl;
}

void ModelePlateau::DropColors()
{
    // iterate through the board from behind and look for holes
    for (int y = m_Lignes - 1; y >= 0; --y)
    {
        for (int x = m_Colonnes - 1; x >= 0; --x)
        {
            int position = y * m_Colonnes + x;

            // check if there is a hole here
            if (m_Colors[position] == -1)
            {
                // get position of color above
                int swapPosition = position - m_Colonnes;

                // if the position is is within the field, move the color down one cell
                if (swapPosition >= 0)
                {
                    m_Colors[position] = m_Colors[swapPosition];
                    m_Colors[swapPosition] = -1;

                    // check if a hole has been closed
                    if (m_Colors[position] != -1)
                    {
                        // invoke color dropped event
                        ColorDropped(x, y - 1);
                    }
                }
                else
                // the hole is at the top row and it gets closed with a new random color
                {
                    m_Colors[position] = GetRandomColor();

                    // invoke color added event
                    ColorAdded(x, y);

                    // one hole has bubbled up all the way to the top and has been removed
                    --m_Troues;
                }

            }
        }
    }
}

void ModelePlateau::Print()
{
    // new line
    qDebug() << endl;

    for (int y = 0; y < m_Lignes; ++y)
    {
        for (int x = 0; x < m_Colonnes; ++x)
        {
            // print color ID of each cell
            qDebug() << QString::number(m_Colors[y * m_Colonnes + x]) << " ";
        }
        // new line
        qDebug() << endl;
    }
}

uint8_t ModelePlateau::GetRandomColor()
{
    uint8_t randomColor = static_cast<uint8_t>
            (Aleatoire::GetAl()->EntierInferieurA( static_cast<int>(Gemme::Color::COUNT)));
    return randomColor;
}

bool ModelePlateau::IsPartOfSequence(uint8_t const& cellX, uint8_t const& cellY)
{
    // look in all directions and check

    uint8_t sourceColor = m_Colors[cellY * m_Colonnes + cellX];

    // look right
    if (cellX < (m_Colonnes - 2))
    {
        if (sourceColor == m_Colors[cellY * m_Colonnes + cellX + 1] && sourceColor == m_Colors[cellY * m_Colonnes + cellX + 2])
            return true;
    }

    // look left
    if (cellX > 1)
    {
        if (sourceColor == m_Colors[cellY * m_Colonnes + cellX - 1] && sourceColor == m_Colors[cellY * m_Colonnes + cellX - 2])
            return true;
    }

    // look left and right 1 field
    if (cellX > 0 && cellX < (m_Colonnes - 1))
    {
        if (sourceColor == m_Colors[cellY * m_Colonnes + cellX - 1] && sourceColor == m_Colors[cellY * m_Colonnes + cellX + 1])
            return true;
    }

    // look down
    if (cellY < (m_Lignes - 2))
    {
        if (sourceColor == m_Colors[(cellY + 1) * m_Colonnes + cellX] && sourceColor == m_Colors[(cellY + 2) * m_Colonnes + cellX])
            return true;
    }

    // look up
    if (cellY > 1)
    {
        if (sourceColor == m_Colors[(cellY - 1) * m_Colonnes + cellX] && sourceColor == m_Colors[(cellY - 2) * m_Colonnes + cellX])
            return true;
    }

    // look up and down 1 field
    if (cellY > 0 && cellY < (m_Lignes - 1))
    {
        if (sourceColor == m_Colors[(cellY - 1) * m_Colonnes + cellX] && sourceColor == m_Colors[(cellY + 1) * m_Colonnes + cellX])
            return true;
    }

    return false;
}


void ModelePlateau::SetColorAddedHandler(std::function<void(const uint8_t&, const uint8_t&)> handlerFunction)
{
    ColorAdded = handlerFunction;
}

void ModelePlateau::SetColorRemovedHandler(std::function<void(const uint8_t&, const uint8_t&)> handlerFunction)
{
    ColorRemoved = handlerFunction;
}

void ModelePlateau::SetColorDroppedHandler(std::function<void(const uint8_t&, const uint8_t&)> handlerFunction)
{
    ColorDropped = handlerFunction;
}

void ModelePlateau::SetSequenceFoundHandler(std::function<void(const uint8_t&, const uint8_t&)> handlerFunction)
{
    SequenceFound = handlerFunction;
}

uint8_t const& ModelePlateau::GetColumns()
{
    return m_Colonnes;
}

uint8_t const& ModelePlateau::GetRows()
{
    return m_Lignes;
}

uint8_t const& ModelePlateau::GetHoles()
{
    return m_Troues;
}
