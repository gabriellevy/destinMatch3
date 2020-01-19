#ifndef VUEPLATEAU_H
#define VUEPLATEAU_H

#include <QWidget>
#include <functional>
#include "gemme.h"
#include <QMouseEvent>

class GemmeFactory;

namespace Ui {
class VuePlateau;
}

/*! Displays the state of the BoardModel to the user and processes user input
*
*/
class VuePlateau : public QWidget
{
    Q_OBJECT

public:
    explicit VuePlateau(const uint8_t& colonnes, const uint8_t& lignes, QWidget *parent = nullptr);
    ~VuePlateau();

    static VuePlateau* ME;

    enum class State { Freeze, SelectFirstGem, SelectSecondGem, RemoveGems };

    /*! \brief Sets the position of the board
    *
    *	\param x The x position
    *	\param y The y postion
    */
    void SetPosition(const uint32_t& x, const uint32_t& y);

    /*! \brief Sets the width of the board
    *
    *	\param width The desired width
    */
    void SetWidth(const uint32_t& width);

    /*! \brief Sets the height of the board
    *
    *	\param height The desired height
    */
    void SetHeight(const uint32_t& height);

    /*! \brief Retrieves the gem of the specified cell
    *
    *	\param column The column of the requested cell
    *	\param row The row of the requested cell
    */
    Gemme* GetGem(const int& column, const int& row) const;

    /*! \brief Adds a gem to a cell
    *
    *	\param column The column of the requested cell
    *	\param row The row of the requested cell
    *	\param color The color of the gem to be added
    */
    void AddGem(const int& column, const int& row, const Gemme::Color& color);

    /*! \brief Marks a gems that will be removed
    *
    *	\param targetColumn Horizontal postion of target cell
    *	\param targetRow Vertical postion of target cell
    */
    void MarkRemovedGem(const int& column, const int& row);

    /*! \brief Removes a gems
    *
    *	\param targetColumn Horizontal postion of target cell
    *	\param targetRow Vertical postion of target cell
    */
    void RemovedGem(const int& column, const int& row);

    /*! \brief Moves a gem from the source cell to the target cell
    *
    *	\param gem Pointer to the gem to be moved
    *	\param targetColumn Horizontal postion of target cell
    *	\param targetRow Vertical postion of target cell
    */
    void MoveGem(Gemme* gem, const int& targetColumn, const int& targetRow);

    /*! \brief Renders the board view
    *
    */
    void Render();

    /*! \brief Enables or disables user input
    *
    *	\param isInputEnabled Flag to enable or disable input
    */
    void EnableInput(const bool& isInputEnabled);

    /*! \brief Checks if the board view is accepting user input
    *
    *	\return true if enabled and false if disabled
    */
    bool IsInputEnabled();

    // registration method for handlers that want to know which gems have been selected
    void SetGemsSelectedHandler(std::function<void(const uint8_t&, const uint8_t&, const uint8_t&, const uint8_t&)> handlerFunction);

private:
    // process mouse events
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    // highlight a cell
    void HighlightCell(const uint8_t& column, const uint8_t& row);

    // resets the state of a gem
    void ResetCell(const uint8_t& column, const uint8_t& row);

    void SwapCells(const int& firstCell, const int& secondCell);

    // detects if mouse is within board bounds
    bool IsMouseOverBoard();

    // amount of cells
    const uint8_t m_Columns;
    const uint8_t m_Rows;

    // position and dimensions of the board
    uint32_t m_PosX;
    uint32_t m_PosY;
    uint32_t m_Width;
    uint32_t m_Height;

    // dimensions of the cells
    uint32_t m_CellWidth;
    uint32_t m_CellHeight;

    // mouse position
    uint32_t m_MousePosX;
    uint32_t m_MousePosY;

    // tracks if user presses the left mouse button
    bool m_IsLeftMouseButtonDown;

    // array that holds all gems
    Gemme** m_Gems;

    // the factory that produces gems
    GemmeFactory* m_GemFactory;

    // holds the current board view state
    State m_State;

    // track highlighted cell
    uint32_t m_HighlightedCell;

    // track selection
    int m_FirstCell;
    int m_SecondCell;

    Gemme* m_HoveredGem = nullptr;

    // a cursor that indicates the selected gem
    //Sprite* m_SelectionCursor = nullptr;
    // sprite for highlighting removed gems
    //Sprite* m_RemoveHighlight = nullptr;

    // handler functions
    std::function <void(const uint8_t&, const uint8_t&, const uint8_t&, const uint8_t&)> GemsSelected;

    // file paths to sprites
    static const std::string SELECTION_CURSOR_PATH;
    static const std::string REMOVE_SPRITE_PATH;
private:
    Ui::VuePlateau *ui;
};

#endif // VUEPLATEAU_H
