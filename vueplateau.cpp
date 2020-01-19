#include "vueplateau.h"
#include "ui_vueplateau.h"
#include "gemmefactory.h"

VuePlateau::VuePlateau(const uint8_t& colonnes, const uint8_t& lignes, QWidget *parent) :
    QWidget(parent),
    m_Columns(colonnes),
    m_Rows(lignes),
    m_PosX(0),
    m_PosY(0),
    m_Width(100),
    m_Height(100),
    m_CellWidth(0),
    m_CellHeight(0),
    m_MousePosX(0),
    m_MousePosY(0),
    m_IsLeftMouseButtonDown(false),
    m_GemFactory(new GemmeFactory()),
    //m_SelectionCursor(new Sprite),
    //m_RemoveHighlight(new Sprite),
    m_State(State::SelectFirstGem),
    ui(new Ui::VuePlateau)
{
    ui->setupUi(this);

    ME = this;

    // create array of gems
    m_Gems = new Gemme*[m_Columns * m_Rows];

    // calculate cell dimensions
    m_CellWidth = m_Width / m_Columns;
    m_CellHeight = m_Height / m_Rows;
    //m_SelectionCursor->Load(SELECTION_CURSOR_PATH);
    //m_RemoveHighlight->Load(REMOVE_SPRITE_PATH);
}

VuePlateau* VuePlateau::ME = nullptr;

VuePlateau::~VuePlateau()
{
    delete ui;
    // delete gem array
    delete[] m_Gems;
    //delete m_RemoveHighlight;
    //delete m_SelectionCursor;
    delete m_GemFactory;
}


void VuePlateau::SetPosition(const uint32_t& x, const uint32_t& y)
{
    m_PosX = x;
    m_PosY = y;
}

void VuePlateau::SetWidth(const uint32_t& width)
{
    m_Width = width;

    // update cell width
    m_CellWidth = m_Width / m_Columns;
}

void VuePlateau::SetHeight(const uint32_t& height)
{
    m_Height = height;

    // update cell height
    m_CellHeight = m_Height / m_Rows;
}

Gemme* VuePlateau::GetGem(int const& column, int const& row) const
{
    if (row >= 0 && row < m_Rows && column >= 0 && column < m_Columns)
    {
        return m_Gems[row * m_Columns + column];
    }

    return nullptr;
}

void VuePlateau::AddGem(const int& column, const int& row, const Gemme::Color& color)
{
    if (row >= 0 && row < m_Rows && column >= 0 && column < m_Columns)
    {
        // get a gem from the factory
        Gemme* addedGem = m_GemFactory->GetGem(color);
        m_Gems[row * m_Columns + column] = addedGem;
    }
}

void VuePlateau::MarkRemovedGem(const int& column, const int& row)
{
    if (row >= 0 && row < m_Rows && column >= 0 && column < m_Columns)
    {
        Gemme* gem = m_Gems[row * m_Columns + column];
        gem->SetState(Gemme::State::Removed);
    }
}

void VuePlateau::RemovedGem(const int& column, const int& row)
{
    if (row >= 0 && row < m_Rows && column >= 0 && column < m_Columns)
    {
        Gemme* gem = m_Gems[row * m_Columns + column];

        // gem is no longer used and can be recycled and reused by the gem factory
        if (gem != nullptr)
            gem->SetUsed(false);

        // remove gem
        m_Gems[row * m_Columns + column] = nullptr;
    }
}

void VuePlateau::MoveGem(Gemme* gem, const int& targetColumn, const int& targetRow)
{
    // cell exists, move the gem there
    if (targetRow >= 0 && targetRow < m_Rows && targetColumn >= 0 && targetColumn < m_Columns)
    {
        m_Gems[targetRow * m_Columns + targetColumn] = gem;

    }
}

void VuePlateau::EnableInput(const bool& isInputEnabled)
{
    if (isInputEnabled == false)
        m_State = State::Freeze;
    else
        m_State = State::SelectFirstGem;
}

bool VuePlateau::IsInputEnabled()
{
    if (m_State == State::Freeze)
        return false;
    else
        return true;
}

void VuePlateau::SetGemsSelectedHandler(std::function<void(const uint8_t&, const uint8_t&, const uint8_t&, const uint8_t&)> handlerFunction)
{
    GemsSelected = handlerFunction;
}

void VuePlateau::mouseMoveEvent(QMouseEvent *event)
{
    const uint32_t& mouseX = static_cast<uint32_t>(event->x());

    const uint32_t& mouseY = static_cast<uint32_t>(event->y());

    // do nothing if input is disabled
    if (m_State == State::Freeze)
        return;

    m_MousePosX = mouseX;
    m_MousePosY = mouseY;

    // check if mouse is placed over board
    if (IsMouseOverBoard() == true)
    {
        // determine the cell the mouse is hovering
        uint8_t xCell = (m_MousePosX - m_PosX) / m_CellWidth;
        uint8_t yCell = (m_MousePosY - m_PosY) / m_CellHeight;

        if (xCell > m_Columns - 1) xCell = m_Columns - 1;
        if (yCell > m_Rows - 1) yCell = m_Rows - 1;

        //highlight new cell if mouse hovers over a different cell
        uint8_t hoveredCell = yCell * m_Columns + xCell;

        if (m_State == State::SelectFirstGem)
        {
            Gemme* highlightedGem = m_Gems[m_HighlightedCell];

            // if there is a gem at the position, highlight it
            if (highlightedGem != nullptr)
            {
                if (hoveredCell != m_HighlightedCell || highlightedGem->GetState() == Gemme::State::Normal)
                {
                    ResetCell(m_HighlightedCell % m_Columns, m_HighlightedCell / m_Rows);
                    HighlightCell(xCell, yCell);
                }
            }
            m_HighlightedCell = hoveredCell;
        }
    }
    // remove hightlighting when outside of board
    else
    {
        Gemme* highlightedGem = m_Gems[m_HighlightedCell];

        // if there is a gem at the position, highlight it
        if (highlightedGem != nullptr)
        {
            if (highlightedGem->GetState() == Gemme::State::Highlighted)
            {
                // switch off highlighting if the mouse is not over the board
                ResetCell(m_HighlightedCell % m_Columns, m_HighlightedCell / m_Rows);
            }
        }
    }
}

void VuePlateau::mousePressEvent(QMouseEvent *event)
{
    // do nothing if input is disabled
    if (m_State == State::Freeze)
        return;

    // check if mouse is placed over board
    if (event->button() == Qt::LeftButton)
    {
        m_IsLeftMouseButtonDown = true;

        // determine the cell where the user clicked
        uint8_t xCell = (m_MousePosX - m_PosX) / m_CellWidth;
        uint8_t yCell = (m_MousePosY - m_PosY) / m_CellHeight;

        if (xCell > m_Columns - 1) xCell = m_Columns - 1;
        if (yCell > m_Rows - 1) yCell = m_Rows - 1;

        if (m_State == State::SelectFirstGem)
        {
            m_FirstCell = yCell * m_Columns + xCell;

            m_Gems[m_FirstCell]->SetState(Gemme::State::Selected);
            m_State = State::SelectSecondGem;
        }
    }
}

void VuePlateau::mouseReleaseEvent(QMouseEvent *event)
{
    // do nothing if input is disabled
    if (m_State == State::Freeze)
        return;

    if (event->button() == Qt::LeftButton)
    {
        m_IsLeftMouseButtonDown = false;

        // check if mouse button was released inside the board
        if (IsMouseOverBoard() == true)
        {
            // determine the cell where the mouse button was released
            uint8_t xCell = (m_MousePosX - m_PosX) / m_CellWidth;
            uint8_t yCell = (m_MousePosY - m_PosY) / m_CellHeight;

            if (xCell > m_Columns - 1) xCell = m_Columns - 1;
            if (yCell > m_Rows - 1) yCell = m_Rows - 1;

            // store cell where mouse button was released
            int releasedCell = yCell * m_Columns + xCell;

            // user selected the second gem
            if (m_State == State::SelectSecondGem && m_FirstCell != releasedCell)
            {
                // invoke gems selected event
                //GemsSelected(m_FirstCell % m_Columns, m_FirstCell / m_Rows, releasedCell % m_Columns, releasedCell / m_Rows);
                SwapCells(m_FirstCell, releasedCell);

                m_Gems[m_FirstCell]->SetState(Gemme::State::Normal);
                m_State = State::SelectFirstGem;

            }

            // user performed a click and release the button over the same cell
            if (m_State == State::SelectSecondGem && m_FirstCell == releasedCell)
            {
                m_Gems[releasedCell]->SetState(Gemme::State::Selected);
            }
        }
    }
}

void VuePlateau::HighlightCell(const uint8_t& column, const uint8_t& row)
{
    uint32_t cellID = row * m_Columns + column;

    if (m_Gems[cellID] != nullptr)
    {
        m_Gems[cellID]->SetState(Gemme::State::Highlighted);
    }
}

void VuePlateau::ResetCell(const uint8_t& column, const uint8_t& row)
{
    uint32_t cellID = row * m_Columns + column;

    if (m_Gems[cellID] != nullptr)
    {
        m_Gems[cellID]->SetState(Gemme::State::Normal);
    }
}

void VuePlateau::SwapCells(const int& firstCell, const int& secondCell)
{

    // swap if cells are adjacent
    if (((firstCell / m_Rows) == (secondCell / m_Rows) && abs(firstCell - secondCell) == 1)
        || abs(firstCell - secondCell) == m_Columns)
    {
        // invoke gems selected event
        GemsSelected(m_FirstCell % m_Columns, m_FirstCell / m_Rows, secondCell % m_Columns, secondCell / m_Rows);

    }
}


bool VuePlateau::IsMouseOverBoard()
{
    return (m_MousePosX > m_PosX && m_MousePosX < m_PosX + m_Width
        && m_MousePosY > m_PosY && m_MousePosY < m_PosY + m_Height);
}

void VuePlateau::Render()
{

    //Draw the tiles by calculating their positions
    for (int i = 0; i < m_Rows * m_Columns; ++i)
    {
        int x = i % m_Columns;
        int y = i / m_Rows;

        Gemme* gem = m_Gems[i];
        if (gem != nullptr)
        {
            //Sprite* currentSprite = gem->GetSprite();

            // draw normal state
            if (gem->GetState() == Gemme::State::Normal || gem->GetState() == Gemme::State::Highlighted)
            {
                gem->move(m_PosX + x * m_CellWidth, m_PosY + y * m_CellHeight);
                gem->Render();
            }

            // draw remove highlight
            if (gem->GetState() == Gemme::State::Removed)
            {
                gem->move(m_PosX + x * m_CellWidth, m_PosY + y * m_CellHeight);
                //m_RemoveHighlight->SetPosition(m_PosX + x * m_CellWidth, m_PosY + y * m_CellHeight);
                gem->Render();
                //m_RemoveHighlight->Render();
            }
        }
    }

    if (m_State == State::SelectSecondGem)
    {
        Gemme* gem = m_Gems[m_FirstCell];

        int x = m_FirstCell % m_Columns;
        int y = m_FirstCell / m_Rows;

        if (m_IsLeftMouseButtonDown)
        {
            gem->move(m_MousePosX - gem->width()/2, m_MousePosY - gem->height()/2);
            m_Gems[m_FirstCell]->Render();
        }
        else
        {
            gem->move(m_PosX + x * m_CellWidth, m_PosY + y * m_CellHeight);
            //m_SelectionCursor->SetPosition(m_PosX + x * m_CellWidth, m_PosY + y * m_CellHeight);

            // render gem & the selection cursor
            m_Gems[m_FirstCell]->Render();
            //m_SelectionCursor->Render();
        }
    }
}
