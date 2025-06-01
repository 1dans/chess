#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace std;

enum class PieceColor { White, Black };
enum class PieceType { King, Queen, Rook, Bishop, Knight, Pawn, None };

struct Position {
    int row;
    int col;

    bool isValid() const {
        return row >= 0 && row < 8 && col >= 0 && col < 8;
    }
};

class Piece {
public:
    PieceColor color;
    PieceType type;

    Piece(PieceColor color, PieceType type) : color(color), type(type) {}
    virtual ~Piece() = default;

    virtual bool isMoveValid(Position from, Position to, const vector<vector<shared_ptr<Piece>>>& board) = 0;
    virtual char getSymbol() const = 0;
    virtual shared_ptr<Piece> clone() const = 0;
};

class Empty : public Piece {
public:
    Empty() : Piece(PieceColor::White, PieceType::None) {}
    bool isMoveValid(Position, Position, const vector<vector<shared_ptr<Piece>>>&) override { return false; }
    char getSymbol() const override { return '.'; }
    shared_ptr<Piece> clone() const override { return make_shared<Empty>(); }
};

class King : public Piece {
public:
    King(PieceColor color) : Piece(color, PieceType::King) {}
    bool isMoveValid(Position from, Position to, const vector<vector<shared_ptr<Piece>>>& board) override {
        int dx = abs(from.row - to.row);
        int dy = abs(from.col - to.col);
        auto dest = board[to.row][to.col];
        if (dest->type != PieceType::None && dest->color == color) return false;
        return dx <= 1 && dy <= 1;
    }
    char getSymbol() const override { return color == PieceColor::White ? 'K' : 'k'; }
    shared_ptr<Piece> clone() const override { return make_shared<King>(*this); }
};

class Queen : public Piece {
public:
    Queen(PieceColor color) : Piece(color, PieceType::Queen) {}
    bool isMoveValid(Position from, Position to, const vector<vector<shared_ptr<Piece>>>& board) override {
        int dx = abs(from.row - to.row);
        int dy = abs(from.col - to.col);
        auto dest = board[to.row][to.col];
        if (dest->type != PieceType::None && dest->color == color) return false;
        return dx == dy || from.row == to.row || from.col == to.col;
    }
    char getSymbol() const override { return color == PieceColor::White ? 'Q' : 'q'; }
    shared_ptr<Piece> clone() const override { return make_shared<Queen>(*this); }
};

class Rook : public Piece {
public:
    Rook(PieceColor color) : Piece(color, PieceType::Rook) {}
    bool isMoveValid(Position from, Position to, const vector<vector<shared_ptr<Piece>>>& board) override {
        if (from.row != to.row && from.col != to.col) return false;
        auto dest = board[to.row][to.col];
        if (dest->type != PieceType::None && dest->color == color) return false;
        return true;
    }
    char getSymbol() const override { return color == PieceColor::White ? 'R' : 'r'; }
    shared_ptr<Piece> clone() const override { return make_shared<Rook>(*this); }
};

class Bishop : public Piece {
public:
    Bishop(PieceColor color) : Piece(color, PieceType::Bishop) {}
    bool isMoveValid(Position from, Position to, const vector<vector<shared_ptr<Piece>>>& board) override {
        int dx = abs(from.row - to.row);
        int dy = abs(from.col - to.col);
        auto dest = board[to.row][to.col];
        if (dest->type != PieceType::None && dest->color == color) return false;
        return dx == dy;
    }
    char getSymbol() const override { return color == PieceColor::White ? 'B' : 'b'; }
    shared_ptr<Piece> clone() const override { return make_shared<Bishop>(*this); }
};

class Knight : public Piece {
public:
    Knight(PieceColor color) : Piece(color, PieceType::Knight) {}
    bool isMoveValid(Position from, Position to, const vector<vector<shared_ptr<Piece>>>& board) override {
        int dx = abs(from.row - to.row);
        int dy = abs(from.col - to.col);
        auto dest = board[to.row][to.col];
        if (dest->type != PieceType::None && dest->color == color) return false;
        return (dx == 2 && dy == 1) || (dx == 1 && dy == 2);
    }
    char getSymbol() const override { return color == PieceColor::White ? 'N' : 'n'; }
    shared_ptr<Piece> clone() const override { return make_shared<Knight>(*this); }
};

class Pawn : public Piece {
public:
    Pawn(PieceColor color) : Piece(color, PieceType::Pawn) {}
    bool isMoveValid(Position from, Position to, const vector<vector<shared_ptr<Piece>>>& board) override {
        int dir = (color == PieceColor::White) ? -1 : 1;
        int startRow = (color == PieceColor::White) ? 6 : 1;
        auto dest = board[to.row][to.col];
        if (from.col == to.col && dest->type == PieceType::None) {
            if (to.row == from.row + dir) return true;
            if (from.row == startRow && to.row == from.row + 2 * dir && board[from.row + dir][from.col]->type == PieceType::None)
                return true;
        }
        else if (abs(to.col - from.col) == 1 && to.row == from.row + dir && dest->type != PieceType::None && dest->color != color) {
            return true;
        }
        return false;
    }
    char getSymbol() const override { return color == PieceColor::White ? 'P' : 'p'; }
    shared_ptr<Piece> clone() const override { return make_shared<Pawn>(*this); }
};

class Board {
private:
    vector<vector<shared_ptr<Piece>>> board;
public:
    Board();
    void setup();
    void draw();
    bool move(Position from, Position to);
    shared_ptr<Piece> getPiece(Position pos);
    const vector<vector<shared_ptr<Piece>>>& getState() const { return board; }
    Position findKing(PieceColor color);
    Board clone() const;
};

Board::Board() {
    board.resize(8, vector<shared_ptr<Piece>>(8));
    setup();
}

Board Board::clone() const {
    Board newBoard;
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
            newBoard.board[i][j] = board[i][j]->clone();
    return newBoard;
}

void Board::setup() {
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            board[i][j] = make_shared<Empty>();

    board[0][0] = make_shared<Rook>(PieceColor::Black);
    board[0][1] = make_shared<Knight>(PieceColor::Black);
    board[0][2] = make_shared<Bishop>(PieceColor::Black);
    board[0][3] = make_shared<Queen>(PieceColor::Black);
    board[0][4] = make_shared<King>(PieceColor::Black);
    board[0][5] = make_shared<Bishop>(PieceColor::Black);
    board[0][6] = make_shared<Knight>(PieceColor::Black);
    board[0][7] = make_shared<Rook>(PieceColor::Black);
    for (int j = 0; j < 8; ++j)
        board[1][j] = make_shared<Pawn>(PieceColor::Black);

    for (int j = 0; j < 8; ++j)
        board[6][j] = make_shared<Pawn>(PieceColor::White);
    board[7][0] = make_shared<Rook>(PieceColor::White);
    board[7][1] = make_shared<Knight>(PieceColor::White);
    board[7][2] = make_shared<Bishop>(PieceColor::White);
    board[7][3] = make_shared<Queen>(PieceColor::White);
    board[7][4] = make_shared<King>(PieceColor::White);
    board[7][5] = make_shared<Bishop>(PieceColor::White);
    board[7][6] = make_shared<Knight>(PieceColor::White);
    board[7][7] = make_shared<Rook>(PieceColor::White);
}

void Board::draw() {
    for (int i = 0; i < 8; i++) {
        cout << 8 - i << " ";
        for (int j = 0; j < 8; j++) {
            cout << board[i][j]->getSymbol() << " ";
        }
        cout << endl;
    }
    cout << "  a b c d e f g h" << endl;
}

bool Board::move(Position from, Position to) {
    auto piece = getPiece(from);
    if (!piece->isMoveValid(from, to, board)) {
        return false;
    }
    if (piece->type == PieceType::Pawn && (to.row == 0 || to.row == 7)) {
        board[to.row][to.col] = make_shared<Queen>(piece->color);
    }
    else {
        board[to.row][to.col] = piece;
    }
    board[from.row][from.col] = make_shared<Empty>();
    return true;
}

shared_ptr<Piece> Board::getPiece(Position pos) {
    return board[pos.row][pos.col];
}

Position Board::findKing(PieceColor color) {
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
            if (board[i][j]->type == PieceType::King && board[i][j]->color == color)
                return { i, j };
    return { -1, -1 };
}

class ChessGame {
private:
    Board board;
    PieceColor currentTurn = PieceColor::White;
    vector<string> moveHistory;
public:
    void start();
    void nextTurn();
    bool handleMove(Position from, Position to, const string& fromStr = "", const string& toStr = "");
    bool isCheckmate(PieceColor color);
};

void ChessGame::start() {
    srand(time(nullptr));
    while (true) {
        board.draw();
        if (isCheckmate(currentTurn)) {
            cout << (currentTurn == PieceColor::White ? "Чорні перемогли!\n" : "Білі перемогли!\n");
            break;
        }
        if (currentTurn == PieceColor::White) {
            cout << "Хід білих\nВведіть хід (наприклад, e2 e4): ";
            string fromStr, toStr;
            cin >> fromStr >> toStr;
            Position from = { 8 - (fromStr[1] - '0'), fromStr[0] - 'a' };
            Position to = { 8 - (toStr[1] - '0'), toStr[0] - 'a' };
            if (handleMove(from, to, fromStr, toStr)) nextTurn();
        }
        else {
            vector<pair<Position, Position>> moves;
            for (int i = 0; i < 8; ++i) {
                for (int j = 0; j < 8; ++j) {
                    Position from = { i, j };
                    auto piece = board.getPiece(from);
                    if (piece->color != PieceColor::Black || piece->type == PieceType::None) continue;
                    for (int x = 0; x < 8; ++x) {
                        for (int y = 0; y < 8; ++y) {
                            Position to = { x, y };
                            if (piece->isMoveValid(from, to, board.getState())) {
                                Board testBoard = board.clone();
                                testBoard.move(from, to);
                                if (testBoard.findKing(PieceColor::Black).row != -1) {
                                    moves.push_back({ from, to });
                                }
                            }
                        }
                    }
                }
            }
            if (!moves.empty()) {
                auto move = moves[rand() % moves.size()];
                handleMove(move.first, move.second);
                nextTurn();
            }
        }
    }
}

void ChessGame::nextTurn() {
    currentTurn = (currentTurn == PieceColor::White) ? PieceColor::Black : PieceColor::White;
}

bool ChessGame::handleMove(Position from, Position to, const string& fromStr, const string& toStr) {
    auto piece = board.getPiece(from);
    if (!piece || piece->type == PieceType::None || piece->color != currentTurn)
        return false;
    if (board.move(from, to)) {
        if (!fromStr.empty() && !toStr.empty())
            moveHistory.push_back(fromStr + "-" + toStr);
        return true;
    }
    return false;
}

bool ChessGame::isCheckmate(PieceColor color) {
    Position kingPos = board.findKing(color);
    if (kingPos.row == -1) return true;
    return false;
}

int main() {
	system("chcp 1251>null");
    ChessGame game;
    game.start();
    return 0;
}
