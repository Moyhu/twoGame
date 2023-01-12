#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <random>
#include <termio.h>

using namespace std;


class ChessBoard;
class Cell;
class TwoRenderBase
{
public:
    virtual void OnRender(int maxRow, int maxCol, Cell* cells, int mark) = 0;
};


class Cell
{
public:
    Cell()
        : m_number(0)
    {
    }

    ~Cell() {

    }

    int Number() {
        return m_number;
    }

private:
    void NewNumber() {
        m_number = rand() % 2 ? 2 : 4;
    }

    bool MoveFrom(Cell& cell) {
        if (m_number != 0)
            return false;
        m_number = cell.m_number;
        cell.m_number = 0;
        return true;
    }

    int MergeCell(Cell& cell) {
        if (m_number == 0 || m_number != cell.m_number)
            return 0;
        m_number += cell.m_number;
        cell.m_number = 0;
        return m_number;
    }

    void Reset(bool isForce) {
        if (isForce || m_number == 0)
            NewNumber();
    }
    friend class ChessBoard;

private:
    int     m_number;
};

class ChessBoard
{
public:
    enum MoveDirection {
        e_UP,
        e_DOWN,
        e_LEFT,
        e_RIGHT,
    };

    ChessBoard(TwoRenderBase* render, int maxRow, int maxCol)
        : m_render(render)
        , m_maxRow(maxRow)
        , m_maxCol(maxCol)
    {
//        assert(render != NULL);
//        assert(maxRow > 0 && maxCol > 0);
//        assert(!(maxRow == 1 && maxRow == maxCol));
        m_cells = new Cell[maxRow * maxCol];
        Reset();
    }

    ~ChessBoard() {
        delete [] m_cells;
    }

    void Reset() {
        m_mark = 0;
        for (int i = 0; i < m_maxRow * m_maxCol; i++) {
            m_cells[i].Reset(true);
        }
        m_render->OnRender(m_maxRow, m_maxCol, m_cells, m_mark);
    }

    void Input(MoveDirection event) {
        switch (event) {
        case e_UP:
            for (int row = 0; row < m_maxRow - 1; row++) {
                for (int col = 0; col < m_maxCol; col++) {
                    Cell& portCell = m_cells[(row * m_maxCol) + col];
                    Cell& sinkCell = m_cells[((row + 1) * m_maxCol) + col];
                    portCell.MoveFrom(sinkCell);
                    m_mark += portCell.MergeCell(sinkCell);
                }
            }
            break;
        case e_DOWN:
            for (int row = m_maxRow - 1; row > 0; row--) {
                for (int col = 0; col < m_maxCol; col++) {
                    Cell& portCell = m_cells[(row * m_maxCol) + col];
                    Cell& sinkCell = m_cells[((row - 1) * m_maxCol) + col];
                    portCell.MoveFrom(sinkCell);
                    m_mark += portCell.MergeCell(sinkCell);
                }
            }
            break;
        case e_LEFT:
            for (int col = 0; col < m_maxCol - 1; col++){
                 for (int row = 0; row < m_maxRow; row++) {
                    Cell& portCell = m_cells[(row * m_maxCol) + col];
                    Cell& sinkCell = m_cells[(row * m_maxCol) + col + 1];
                    portCell.MoveFrom(sinkCell);
                    m_mark += portCell.MergeCell(sinkCell);
                }
            }
            break;
        case e_RIGHT:
            for (int col = m_maxCol - 1; col > 0; col--){
                 for (int row = 0; row < m_maxRow; row++) {
                    Cell& portCell = m_cells[(row * m_maxCol) + col];
                    Cell& sinkCell = m_cells[(row * m_maxCol) + col - 1];
                    portCell.MoveFrom(sinkCell);
                    m_mark += portCell.MergeCell(sinkCell);
                }
            }
            break;
        default:
            break;
        }

        for (int i = 0; i < m_maxRow * m_maxCol; i++) {
            m_cells[i].Reset(false);
        }

        m_render->OnRender(m_maxRow, m_maxCol, m_cells, m_mark);
    }

private:
    TwoRenderBase *m_render;
    int m_maxRow;
    int m_maxCol;
    int m_mark;
    Cell* m_cells;
};

class TwoRender : public TwoRenderBase
{
public:
    virtual void OnRender(int maxRow, int maxCol, Cell* cells, int mark) {
        static bool isFirst = true;
        if (isFirst) {
            isFirst = false;
        } else {
            ClearLastLine(maxCol + 2);
        }
        printf("\n");
        printf("mark:%d\n", mark);
        for (int row = 0; row < maxRow; row++) {
            for (int col = 0; col < maxCol; col++) {
                printf("%4d|", cells[(row * maxRow) + col].Number());
            }
            printf("\n");
        }
        fflush(NULL);
    }

private:
    void ClearLastLine(int lineCnt) {
        while (lineCnt--) {
            printf("\033[1A");
            printf("\033[K");
        }
    }
};

int scanKeyboard()
{
    int in;

    struct termios new_settings;
    struct termios stored_settings;
    tcgetattr(0,&stored_settings);
    new_settings = stored_settings;
    new_settings.c_lflag &= (~ICANON);
    new_settings.c_cc[VTIME] = 0;
    tcgetattr(0,&stored_settings);
    new_settings.c_cc[VMIN] = 1;
    tcsetattr(0,TCSANOW,&new_settings);
    in = getchar();
    tcsetattr(0,TCSANOW,&stored_settings);

    return in;
}

int main(int argc, char** args)
{
    if (argc < 2) {
        cout << "usage: [max row] [max col] " << endl;
        return -1;
    }
    cout << "enter [w s a d] to move, and enter [q] exit" << endl;
    int maxRow;
    int maxCol;
    maxRow = atoi(args[1]);
    maxCol = atoi(args[2]);
    TwoRender render;
    ChessBoard chessBoard(&render, maxRow, maxCol);
    while (1) {
        char inputKey = scanKeyboard();
        if (inputKey == 'w')
            chessBoard.Input(ChessBoard::e_UP);
        else if (inputKey == 's')
            chessBoard.Input(ChessBoard::e_DOWN);
        else if (inputKey == 'a')
            chessBoard.Input(ChessBoard::e_LEFT);
        else if (inputKey == 'd')
            chessBoard.Input(ChessBoard::e_RIGHT);
        else if (inputKey == 'q')
            break;
    }
    return 0;
}
