#ifndef BOARD_H
#define BOARD_H
#include "types.h"
#include "evaluate.h"
#include "polyglot.h"
class Board {

private:
    uint64_t whitePawn;
    uint64_t whiteKing;
    uint64_t whiteQueen;
    uint64_t whiteBishop;
    uint64_t whiteKnight;
    uint64_t whiteRook;

    uint64_t blackPawn;
    uint64_t blackKing;
    uint64_t blackQueen;
    uint64_t blackBishop;
    uint64_t blackKnight;
    uint64_t blackRook;

    uint64_t knightMoveMask[64];
    uint64_t kingMoveMask[64];

    uint8_t castlingRights;
    const uint8_t WK = 1, WQ = 2, BK = 4, BQ = 8;
    uint8_t castlingMask[64];
    int enPassantSquare;
    bool sideToMove;
    int halfmoveClock;
    int fullmoveNumber;
    uint64_t hashKey;
    uint64_t historyStack[2048];
    int historyIndex;

    uint16_t killerMoves[256][2];
    int historyHeuristic[2][64][64];

    uint16_t pvTable[256][256];
    int pvLength[256];

    Evaluation::Score evaluatePiece(uint64_t bb, int val, const Evaluation::Score pst[64], bool white) const;

public:
    enum PieceType { NONE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };

    struct SearchLimits {
        int wtime = -1, btime = -1, winc = 0, binc = 0, movestogo = 30;
        int depth = 100, movetime = -1;
        bool infinite = false;
    };

    std::atomic<bool> stopSearch{false};
    chrono::time_point<chrono::steady_clock> startTime;
    long long timeLimit = 0;
    uint64_t nodes = 0;

    void checkTime() ;



    enum HashFlag { HASH_EXACT, HASH_ALPHA, HASH_BETA };
    struct TTEntry {
        uint64_t key;
        int score;
        int depth;
        int flag;
        uint16_t bestMove;
    };
    vector<TTEntry> tt;
    const int TT_SIZE = 1000000;

    void clearTT() ;
    void clearSearchState();

    int scoreToTT(int score, int ply) ;

    int scoreFromTT(int score, int ply) ;

    int probeTT(uint64_t key, int depth, int alpha, int beta, int ply, uint16_t& bestMove) ;

    void storeTT(uint64_t key, int depth, int score, int flag, uint16_t bestMove, int ply) ;

    uint64_t generateHashKey() ;

    uint64_t getPolyglotHash() const;

    int evaluate() const;

    struct UndoInfo {
        uint8_t castlingRights;
        int enPassantSquare;
        int halfmoveClock;
        int capturedPieceType;
        uint16_t move;
        uint64_t hashKey;
    };

    enum MoveFlags {
        QUIET = 0,
        DOUBLE_PUSH = 1,
        K_CASTLE = 2,
        Q_CASTLE = 3,
        CAPTURE = 4,
        EP_CAPTURE = 5,
        PROMO_KNIGHT = 8,
        PROMO_BISHOP = 9,
        PROMO_ROOK = 10,
        PROMO_QUEEN = 11,
        PROMO_KNIGHT_CAP = 12,
        PROMO_BISHOP_CAP = 13,
        PROMO_ROOK_CAP = 14,
        PROMO_QUEEN_CAP = 15
    };

    static inline int getFrom(uint16_t move) { return move & 0x3F; }
    static inline int getTo(uint16_t move) { return (move >> 6) & 0x3F; }
    static inline int getFlags(uint16_t move) { return (move >> 12) & 0xF; }

    bool getSideToMove() const;
    uint8_t getCastlingRights() const;
    int getEnPassantSquare() const;

    int getPieceAt(int square, bool side) const;

    bool inCheck(bool side) const;

    bool isRepetition() const;

    int see(uint16_t move) const;
    uint64_t getLeastValuableAttacker(int square, bool side, uint64_t& occupied, int& pieceType) const;

    int getMoveScore(uint16_t move, uint16_t hashMove, int ply) const;

    int quiescence(int alpha, int beta) ;

    int negamax(int depth, int alpha, int beta, int ply, uint16_t& bestMoveOut, uint16_t pvMove) ;

    uint16_t search(const SearchLimits& limits) ;

    void togglePiece(int pieceType, int square, bool side) ;

    bool makeMove(uint16_t move, UndoInfo& undo) ;

    void unmakeMove(uint16_t move, const UndoInfo& undo) ;

    void makeNullMove(UndoInfo& undo);

    void unmakeNullMove(const UndoInfo& undo);

    uint16_t package_move(int from, int to, int flags = 0) const;

    void resetBoard() ;

    void parseFEN(const string& fen) ;

    uint16_t parseMove(string moveStr) ;

    void init() ;

    void calculateLeapers() ;


    uint64_t getRookMoves(int square, uint64_t occupancy) const;

    uint64_t getBishopAttacks(int square, uint64_t occupancy) const;

    uint64_t attackedSquares(bool side) const;

    void generateRookMoves(uint64_t rookBoard, uint64_t occupied, uint64_t friendly, vector<uint16_t>& moveset) const;

    void generateBishopMoves(uint64_t bishopBoard, uint64_t occupied, uint64_t friendly, vector<uint16_t>& moveset) const;

    void generateQueenMoves(uint64_t queenBoard, uint64_t occupied, uint64_t friendly, vector<uint16_t>& moveset) const;

    void generateKnightMoves(uint64_t knightBoard, uint64_t occupied, uint64_t friendly, vector<uint16_t>& moveset) const;

    void generateKingMoves(uint64_t kingBoard, uint64_t occupied, uint64_t friendly, vector<uint16_t>& moveset) const;

    void generateCastlingMoves(bool side, uint64_t occupied, vector<uint16_t>& moveSet) const;

    void generateWhitePawnMoves(uint64_t whitePawns, uint64_t occupied, uint64_t friendly, vector<uint16_t>& moveSet) const;

    void generateBlackPawnMoves(uint64_t blackPawns, uint64_t occupied, uint64_t friendly, vector<uint16_t>& moveSet) const;

    vector<uint16_t> generateMoves(bool side) const;

    string squareToCoord(int sq) ;

    string moveToString(uint16_t move) ;

    uint64_t perft(int depth) ;

    void perftDivide(int depth) ;
};
#endif
