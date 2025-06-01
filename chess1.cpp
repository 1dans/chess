#include <iostream>
#include <vector>
#include <memory>
#include <cctype>

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
};

class King : public Piece {
public:
    King(PieceColor color) : Piece(color, PieceType::King) {}

    bool isMoveValid(Position from, Position to, const vector<vector<shared_ptr<Piece>>>& board) override {
        int dx = abs(from.row - to.row);
        int dy = abs(from.col - to.col);

        if (!to.isValid()) return false;

        auto destPiece = board[to.row][to.col];
        if (destPiece->type != PieceType::None && destPiece->color == color)
            return false;

        return (dx <= 1 && dy <= 1 && (dx != 0 || dy != 0));
    }

    char getSymbol() const override {
        return color == PieceColor::White ? 'K' : 'k';
    }
};

class Empty : public Piece {
public:
    Empty() : Piece(PieceColor::White, PieceType::None) {}

    bool isMoveValid(Position, Position, const vector<vector<shared_ptr<Piece>>>&) override {
        return false;
    }

    char getSymbol() const override {
        return '.';
    }
};

class Board {
private:
    vector<vector<shared_ptr<Piece>>> board;

public:
    Board();
    void setup();
    void draw() const;
    bool move(Position from, Position to);
    shared_ptr<Piece> getPiece(Position pos) const;
    const vector<vector<shared_ptr<Piece>>>& getBoard() const { return board; }
};

class ChessGame {
private:
    Board board;
    PieceColor currentTurn = PieceColor::White;

public:
    void start();
    void nextTurn();
    bool handleMove(Position from, Position to);
};

Board::Board() {
    board.resize(8, vector<shared_ptr<Piece>>(8));
    setup();
}

void Board::setup() {
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            board[i][j] = make_shared<Empty>();

    board[0][4] = make_shared<King>(PieceColor::Black);
    board[7][4] = make_shared<King>(PieceColor::White);
}

void Board::draw() const {
    for (int i = 0; i < 8; i++) {
        cout << 8 - i << " ";
        for (int j = 0; j < 8; j++) {
            cout << board[i][j]->getSymbol() << " ";
        }
        cout << endl;
    }
    cout << "  a b c d e f g h\n";
}

shared_ptr<Piece> Board::getPiece(Position pos) const {
    if (pos.isValid())
        return board[pos.row][pos.col];
    return nullptr;
}

bool Board::move(Position from, Position to) {
    if (!from.isValid() || !to.isValid()) {
        cout << "Координати поза межами дошки!\n";
        return false;
    }

    auto piece = getPiece(from);
    if (!piece || piece->type == PieceType::None) {
        cout << "На вибраній клітинці немає фігури!\n";
        return false;
    }

    if (!piece->isMoveValid(from, to, board)) {
        cout << "Неправильний хід!\n";
        return false;
    }

    board[to.row][to.col] = piece;
    board[from.row][from.col] = make_shared<Empty>();
    return true;
}

void ChessGame::start() {
    while (true) {
        board.draw();
        cout << (currentTurn == PieceColor::White ? "Хід білих\n" : "Хід чорних\n");

        string fromStr, toStr;
        cout << "Введіть хід (напр., e2 e4): ";
        cin >> fromStr >> toStr;

        if (fromStr.size() != 2 || toStr.size() != 2) {
            cout << "Невірний формат вводу!\n";
            continue;
        }

        Position from = { 8 - (fromStr[1] - '0'), fromStr[0] - 'a' };
        Position to = { 8 - (toStr[1] - '0'), toStr[0] - 'a' };

        if (handleMove(from, to)) {
            nextTurn();
        }
    }
}

void ChessGame::nextTurn() {
    currentTurn = (currentTurn == PieceColor::White) ? PieceColor::Black : PieceColor::White;
}

bool ChessGame::handleMove(Position from, Position to) {
    auto piece = board.getPiece(from);
    if (!piece || piece->type == PieceType::None) {
        cout << "Немає фігури на початковій позиції!\n";
        return false;
    }

    if (piece->color != currentTurn) {
        cout << "Це не ваша фігура!\n";
        return false;
    }

    return board.move(from, to);
}

int main() {
    system("chcp 1251>null");
    ChessGame game;
    game.start();
    return 0;
}
