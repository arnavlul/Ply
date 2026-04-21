#include <iostream>
#include <string>
#include <cstdint>
#include <bit>
#include <vector>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <thread>
#include <atomic>

using namespace std;

namespace Evaluation {
    const int PAWN_VAL = 100;
    const int KNIGHT_VAL = 320;
    const int BISHOP_VAL = 330;
    const int ROOK_VAL = 500;
    const int QUEEN_VAL = 900;
    const int KING_VAL = 20000;

    const int pawn_pst[64] = {
         0,  0,  0,  0,  0,  0,  0,  0,
         5, 10, 10,-20,-20, 10, 10,  5,
         5, -5,-10,  0,  0,-10, -5,  5,
         0,  0,  0, 20, 20,  0,  0,  0,
         5,  5, 10, 25, 25, 10,  5,  5,
        10, 10, 20, 30, 30, 20, 10, 10,
        50, 50, 50, 50, 50, 50, 50, 50,
         0,  0,  0,  0,  0,  0,  0,  0
    };

    const int knight_pst[64] = {
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50
    };

    const int bishop_pst[64] = {
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -20,-10,-10,-10,-10,-10,-10,-20
    };

    const int rook_pst[64] = {
          0,  0,  0,  5,  5,  0,  0,  0,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
          5, 10, 10, 10, 10, 10, 10,  5,
          0,  0,  0,  0,  0,  0,  0,  0
    };

    const int queen_pst[64] = {
        -20,-10,-10, -5, -5,-10,-10,-20,
        -10,  0,  5,  0,  0,  0,  0,-10,
        -10,  5,  5,  5,  5,  5,  0,-10,
          0,  0,  5,  5,  5,  5,  0, -5,
         -5,  0,  5,  5,  5,  5,  0, -5,
        -10,  0,  5,  5,  5,  5,  0,-10,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20
    };

    const int king_pst[64] = {
         20, 30, 10,  0,  0, 10, 30, 20,
         20, 20,  0,  0,  0,  0, 20, 20,
        -10,-20,-20,-20,-20,-20,-20,-10,
        -20,-30,-30,-40,-40,-30,-30,-20,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30
    };

    uint64_t pieceKeys[12][64];
    uint64_t castleKeys[16];
    uint64_t enPassantKeys[8];
    uint64_t sideKey;

    uint64_t random64() {
        static uint64_t seed = 1070372ULL;
        seed ^= seed >> 12;
        seed ^= seed << 25;
        seed ^= seed >> 27;
        return seed * 0x2545F4914F6CDD1DULL;
    }

    void initZobrist() {
        for (int i = 0; i < 12; i++)
            for (int j = 0; j < 64; j++)
                pieceKeys[i][j] = random64();
        for (int i = 0; i < 16; i++) castleKeys[i] = random64();
        for (int i = 0; i < 8; i++) enPassantKeys[i] = random64();
        sideKey = random64();
    }
}

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

    int evaluatePiece(uint64_t bb, int val, const int pst[64], bool white) const {
        int s = 0;
        while (bb) {
            int sq = __builtin_ctzll(bb);
            s += val + pst[white ? sq : (sq ^ 56)];
            bb &= (bb - 1);
        }
        return s;
    }

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

    void checkTime() {
        if (timeLimit != -1) {
            auto now = chrono::steady_clock::now();
            auto elapsed = chrono::duration_cast<chrono::milliseconds>(now - startTime).count();
            if (elapsed >= timeLimit) stopSearch = true;
        }
    }



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

    void clearTT() {
        for (int i = 0; i < TT_SIZE; i++) tt[i].key = 0;
    }

    int scoreToTT(int score, int ply) {
        if (score > 49000) return score + ply;
        if (score < -49000) return score - ply;
        return score;
    }

    int scoreFromTT(int score, int ply) {
        if (score > 49000) return score - ply;
        if (score < -49000) return score + ply;
        return score;
    }

    int probeTT(uint64_t key, int depth, int alpha, int beta, int ply, uint16_t& bestMove) {
        TTEntry& entry = tt[key % TT_SIZE];
        if (entry.key == key) {
            bestMove = entry.bestMove;
            if (entry.depth >= depth) {
                int score = scoreFromTT(entry.score, ply);
                if (entry.flag == HASH_EXACT) return score;
                if (entry.flag == HASH_ALPHA && score <= alpha) return alpha;
                if (entry.flag == HASH_BETA && score >= beta) return beta;
            }
        }
        return -1000001;
    }

    void storeTT(uint64_t key, int depth, int score, int flag, uint16_t bestMove, int ply) {
        TTEntry& entry = tt[key % TT_SIZE];
        entry.key = key;
        entry.score = scoreToTT(score, ply);
        entry.depth = depth;
        entry.flag = flag;
        entry.bestMove = bestMove;
    }

    uint64_t generateHashKey() {
        uint64_t h = 0;
        uint64_t occupancy = (whitePawn | whiteKing | whiteQueen | whiteBishop | whiteKnight | whiteRook) |
                             (blackPawn | blackKing | blackQueen | blackBishop | blackKnight | blackRook);
        uint64_t temp = occupancy;
        while (temp) {
            int sq = __builtin_ctzll(temp);
            int pt = getPieceAt(sq, true);
            if (pt != NONE) h ^= Evaluation::pieceKeys[pt - 1][sq];
            else {
                pt = getPieceAt(sq, false);
                if (pt != NONE) h ^= Evaluation::pieceKeys[pt + 5][sq];
            }
            temp &= (temp - 1);
        }
        if (!sideToMove) h ^= Evaluation::sideKey;
        h ^= Evaluation::castleKeys[castlingRights];
        if (enPassantSquare != -1) h ^= Evaluation::enPassantKeys[enPassantSquare % 8];
        return h;
    }

    int evaluate() const {
        int score = 0;
        score += evaluatePiece(whitePawn, Evaluation::PAWN_VAL, Evaluation::pawn_pst, true);
        score += evaluatePiece(whiteKnight, Evaluation::KNIGHT_VAL, Evaluation::knight_pst, true);
        score += evaluatePiece(whiteBishop, Evaluation::BISHOP_VAL, Evaluation::bishop_pst, true);
        score += evaluatePiece(whiteRook, Evaluation::ROOK_VAL, Evaluation::rook_pst, true);
        score += evaluatePiece(whiteQueen, Evaluation::QUEEN_VAL, Evaluation::queen_pst, true);
        score += evaluatePiece(whiteKing, Evaluation::KING_VAL, Evaluation::king_pst, true);

        score -= evaluatePiece(blackPawn, Evaluation::PAWN_VAL, Evaluation::pawn_pst, false);
        score -= evaluatePiece(blackKnight, Evaluation::KNIGHT_VAL, Evaluation::knight_pst, false);
        score -= evaluatePiece(blackBishop, Evaluation::BISHOP_VAL, Evaluation::bishop_pst, false);
        score -= evaluatePiece(blackRook, Evaluation::ROOK_VAL, Evaluation::rook_pst, false);
        score -= evaluatePiece(blackQueen, Evaluation::QUEEN_VAL, Evaluation::queen_pst, false);
        score -= evaluatePiece(blackKing, Evaluation::KING_VAL, Evaluation::king_pst, false);

        return sideToMove ? score : -score;
    }

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

    bool getSideToMove() const { return sideToMove; }

    int getPieceAt(int square, bool side) const {
        uint64_t bit = (1ULL << square);
        if (side) { // White
            if (whitePawn & bit) return PAWN;
            if (whiteKnight & bit) return KNIGHT;
            if (whiteBishop & bit) return BISHOP;
            if (whiteRook & bit) return ROOK;
            if (whiteQueen & bit) return QUEEN;
            if (whiteKing & bit) return KING;
        } else { // Black
            if (blackPawn & bit) return PAWN;
            if (blackKnight & bit) return KNIGHT;
            if (blackBishop & bit) return BISHOP;
            if (blackRook & bit) return ROOK;
            if (blackQueen & bit) return QUEEN;
            if (blackKing & bit) return KING;
        }
        return NONE;
    }

    bool inCheck(bool side) const {
        uint64_t kingBB = side ? whiteKing : blackKing;
        return (attackedSquares(!side) & kingBB) != 0;
    }

    int getMoveScore(uint16_t move, uint16_t hashMove) const {
        if (move == hashMove) return 2000000;
        int flags = getFlags(move);
        if (flags & CAPTURE) {
            int attacker = getPieceAt(getFrom(move), sideToMove);
            int victim = getPieceAt(getTo(move), !sideToMove);
            if (flags == EP_CAPTURE) victim = PAWN;
            
            auto getVal = [](int t) {
                switch(t) {
                    case PAWN: return 1;
                    case KNIGHT: return 2;
                    case BISHOP: return 3;
                    case ROOK: return 4;
                    case QUEEN: return 5;
                    case KING: return 6;
                    default: return 0;
                }
            };
            return 1000 + (getVal(victim) * 10) - getVal(attacker);
        }
        return 0;
    }

    int quiescence(int alpha, int beta) {
        nodes++;
        if (nodes % 2048 == 0) checkTime();
        if (stopSearch) return 0;

        int standPat = evaluate();
        if (standPat >= beta) return beta;
        if (alpha < standPat) alpha = standPat;

        vector<uint16_t> moves = generateMoves(sideToMove);
        struct ScoredMove { uint16_t move; int score; };
        vector<ScoredMove> scoredMoves;
        for (uint16_t m : moves) {
            if (getFlags(m) & CAPTURE) scoredMoves.push_back({m, getMoveScore(m, 0)});
        }
        sort(scoredMoves.begin(), scoredMoves.end(), [](const ScoredMove& a, const ScoredMove& b) {
            return a.score > b.score;
        });

        for (auto& sm : scoredMoves) {
            UndoInfo undo;
            if (makeMove(sm.move, undo)) {
                int score = -quiescence(-beta, -alpha);
                unmakeMove(sm.move, undo);
                if (stopSearch) return 0;
                if (score >= beta) return beta;
                if (score > alpha) alpha = score;
            }
        }
        return alpha;
    }

    int negamax(int depth, int alpha, int beta, int ply, uint16_t& bestMoveOut, uint16_t pvMove) {
        nodes++;
        if (nodes % 2048 == 0) checkTime();
        if (stopSearch) return 0;

        int oldAlpha = alpha;
        uint16_t ttMove = 0;
        int ttScore = probeTT(hashKey, depth, alpha, beta, ply, ttMove);
        if (ttScore != -1000001) {
            if (ply == 0) bestMoveOut = ttMove;
            return ttScore;
        }

        if (depth <= 0) return quiescence(alpha, beta);

        vector<uint16_t> moves = generateMoves(sideToMove);
        struct ScoredMove { uint16_t move; int score; };
        vector<ScoredMove> scoredMoves;
        uint16_t bestMoveThisNode = ttMove ? ttMove : pvMove;
        for (uint16_t m : moves) scoredMoves.push_back({m, getMoveScore(m, bestMoveThisNode)});
        
        sort(scoredMoves.begin(), scoredMoves.end(), [](const ScoredMove& a, const ScoredMove& b) {
            return a.score > b.score;
        });

        int legalMoves = 0;
        int bestScore = -1000000;
        uint16_t localBestMove = 0;

        for (auto& sm : scoredMoves) {
            UndoInfo undo;
            if (makeMove(sm.move, undo)) {
                legalMoves++;
                
                if (ply == 0 && moveToString(sm.move) == "d4c2") {
                    // cout << "info string Found d4c2 legal at ply 0. Score will be calculated." << endl;
                }

                uint16_t dummy;
                int score = -negamax(depth - 1, -beta, -alpha, ply + 1, dummy, 0);
                unmakeMove(sm.move, undo);

                if (stopSearch) return 0;

                if (score > bestScore) {
                    bestScore = score;
                    localBestMove = sm.move;
                }
                if (score > alpha) {
                    alpha = score;
                }
                if (alpha >= beta) break;
            }
        }

        if (legalMoves == 0) {
            if (inCheck(sideToMove)) return -50000 + ply;
            return 0;
        }

        int flag = HASH_EXACT;
        if (bestScore <= oldAlpha) flag = HASH_ALPHA;
        else if (bestScore >= beta) flag = HASH_BETA;
        storeTT(hashKey, depth, bestScore, flag, localBestMove, ply);

        if (ply == 0) bestMoveOut = localBestMove;
        return bestScore;
    }

    uint16_t search(const SearchLimits& limits) {
        stopSearch = false;
        nodes = 0;
        startTime = chrono::steady_clock::now();
        
        if (limits.movetime != -1) {
            timeLimit = limits.movetime - 50;
        } else if (sideToMove ? (limits.wtime != -1) : (limits.btime != -1)) {
            int time = sideToMove ? limits.wtime : limits.btime;
            int inc = sideToMove ? limits.winc : limits.binc;
            int mtg = (limits.movestogo > 0) ? limits.movestogo : 30;
            timeLimit = time / mtg + inc - 50;
        } else {
            timeLimit = -1;
        }
        if (timeLimit < 20 && timeLimit != -1) timeLimit = 20;

        uint16_t bestMove = 0;
        // Fallback: get the first legal move
        vector<uint16_t> allMoves = generateMoves(sideToMove);
        for (uint16_t m : allMoves) {
            UndoInfo undo;
            if (makeMove(m, undo)) {
                unmakeMove(m, undo);
                bestMove = m;
                break;
            }
        }

        for (int d = 1; d <= limits.depth; d++) {
            uint16_t currentBest = 0;
            int score = negamax(d, -1000000, 1000000, 0, currentBest, bestMove);
            
            if (stopSearch) {
                if (currentBest != 0) bestMove = currentBest;
                break;
            }
            
            if (currentBest != 0) bestMove = currentBest;
            
            auto now = chrono::steady_clock::now();
            auto elapsed = chrono::duration_cast<chrono::milliseconds>(now - startTime).count();
            if (elapsed == 0) elapsed = 1;
            cout << "info depth " << d << " score cp " << score << " nodes " << nodes << " time " << elapsed << " nps " << (nodes * 1000 / elapsed) << " pv " << moveToString(bestMove) << endl;
            
            if (limits.infinite == false && timeLimit != -1 && elapsed > timeLimit / 2) break; 
        }
        return bestMove;
    }

    void togglePiece(int pieceType, int square, bool side) {
        uint64_t bit = (1ULL << square);
        if (side) {
            hashKey ^= Evaluation::pieceKeys[pieceType - 1][square];
            switch (pieceType) {
                case PAWN: whitePawn ^= bit; break;
                case KNIGHT: whiteKnight ^= bit; break;
                case BISHOP: whiteBishop ^= bit; break;
                case ROOK: whiteRook ^= bit; break;
                case QUEEN: whiteQueen ^= bit; break;
                case KING: whiteKing ^= bit; break;
            }
        } else {
            hashKey ^= Evaluation::pieceKeys[pieceType + 5][square];
            switch (pieceType) {
                case PAWN: blackPawn ^= bit; break;
                case KNIGHT: blackKnight ^= bit; break;
                case BISHOP: blackBishop ^= bit; break;
                case ROOK: blackRook ^= bit; break;
                case QUEEN: blackQueen ^= bit; break;
                case KING: blackKing ^= bit; break;
            }
        }
    }

    bool makeMove(uint16_t move, UndoInfo& undo) {
        int from = getFrom(move);
        int to = getTo(move);
        int flags = getFlags(move);
        bool side = sideToMove;

        undo.castlingRights = castlingRights;
        undo.enPassantSquare = enPassantSquare;
        undo.halfmoveClock = halfmoveClock;
        undo.capturedPieceType = NONE;
        undo.move = move;
        undo.hashKey = hashKey;

        int piece = getPieceAt(from, side);
        hashKey ^= Evaluation::castleKeys[castlingRights];
        if (enPassantSquare != -1) hashKey ^= Evaluation::enPassantKeys[enPassantSquare % 8];

        togglePiece(piece, from, side);

        if (flags & CAPTURE) {
            if (flags == EP_CAPTURE) {
                int epSq = side ? to - 8 : to + 8;
                undo.capturedPieceType = PAWN;
                togglePiece(PAWN, epSq, !side);
            } else {
                undo.capturedPieceType = getPieceAt(to, !side);
                if (undo.capturedPieceType != NONE) {
                    togglePiece(undo.capturedPieceType, to, !side);
                }
            }
        }

        if (flags == K_CASTLE) {
            if (side) { togglePiece(ROOK, 7, 1); togglePiece(ROOK, 5, 1); }
            else { togglePiece(ROOK, 63, 0); togglePiece(ROOK, 61, 0); }
        } else if (flags == Q_CASTLE) {
            if (side) { togglePiece(ROOK, 0, 1); togglePiece(ROOK, 3, 1); }
            else { togglePiece(ROOK, 56, 0); togglePiece(ROOK, 59, 0); }
        }

        if (flags >= PROMO_KNIGHT) {
            int promoPiece = KNIGHT;
            int promoType = flags & 3;
            if (promoType == 1) promoPiece = BISHOP;
            else if (promoType == 2) promoPiece = ROOK;
            else if (promoType == 3) promoPiece = QUEEN;
            togglePiece(promoPiece, to, side);
        } else {
            togglePiece(piece, to, side);
        }

        castlingRights &= (castlingMask[from] & castlingMask[to]);
        enPassantSquare = (flags == DOUBLE_PUSH) ? (side ? from + 8 : from - 8) : -1;

        hashKey ^= Evaluation::castleKeys[castlingRights];
        if (enPassantSquare != -1) hashKey ^= Evaluation::enPassantKeys[enPassantSquare % 8];

        halfmoveClock++;
        if (piece == PAWN || (flags & CAPTURE)) halfmoveClock = 0;
        if (!side) fullmoveNumber++;

        sideToMove = !sideToMove;
        hashKey ^= Evaluation::sideKey;

        uint64_t kingBB = side ? whiteKing : blackKing;
        uint64_t attacks = attackedSquares(!side);
        if (attacks & kingBB) {
            unmakeMove(move, undo);
            return false;
        }
        return true;
    }

    void unmakeMove(uint16_t move, const UndoInfo& undo) {
        int from = getFrom(move);
        int to = getTo(move);
        int flags = getFlags(move);
        sideToMove = !sideToMove;
        bool side = sideToMove;

        int piece = getPieceAt(to, side);

        if (flags >= PROMO_KNIGHT) {
            togglePiece(piece, to, side);
            togglePiece(PAWN, to, side);
            piece = PAWN;
        }

        togglePiece(piece, to, side);
        togglePiece(piece, from, side);

        if (flags & CAPTURE) {
            if (flags == EP_CAPTURE) {
                int epSq = side ? to - 8 : to + 8;
                togglePiece(PAWN, epSq, !side);
            } else if (undo.capturedPieceType != NONE) {
                togglePiece(undo.capturedPieceType, to, !side);
            }
        }

        if (flags == K_CASTLE) {
            if (side) { togglePiece(ROOK, 7, 1); togglePiece(ROOK, 5, 1); }
            else { togglePiece(ROOK, 63, 0); togglePiece(ROOK, 61, 0); }
        } else if (flags == Q_CASTLE) {
            if (side) { togglePiece(ROOK, 0, 1); togglePiece(ROOK, 3, 1); }
            else { togglePiece(ROOK, 56, 0); togglePiece(ROOK, 59, 0); }
        }

        castlingRights = undo.castlingRights;
        enPassantSquare = undo.enPassantSquare;
        halfmoveClock = undo.halfmoveClock;
        if (!side) fullmoveNumber--;
        hashKey = undo.hashKey;
    }

    uint16_t package_move(int from, int to, int flags = 0) {
        return (uint16_t)(from | (to << 6) | (flags << 12));
    }

    void resetBoard() {
        whitePawn = 0x000000000000FF00ULL;
        whiteKing = 0x0000000000000010ULL;
        whiteQueen = 0x0000000000000008ULL;
        whiteBishop = 0x0000000000000024ULL;
        whiteKnight = 0x0000000000000042ULL;
        whiteRook = 0x0000000000000081ULL;

        blackPawn = 0x00FF000000000000ULL;
        blackKing = 0x1000000000000000ULL;
        blackQueen = 0x0800000000000000ULL;
        blackBishop = 0x2400000000000000ULL;
        blackKnight = 0x4200000000000000ULL;
        blackRook = 0x8100000000000000ULL;

        castlingRights = 15;
        enPassantSquare = -1;
        sideToMove = 1;
        halfmoveClock = 0;
        fullmoveNumber = 1;
        hashKey = generateHashKey();
    }

    void parseFEN(const string& fen) {
        whitePawn = whiteKnight = whiteBishop = whiteRook = whiteQueen = whiteKing = 0;
        blackPawn = blackKnight = blackBishop = blackRook = blackQueen = blackKing = 0;
        castlingRights = 0;
        enPassantSquare = -1;
        halfmoveClock = 0;
        fullmoveNumber = 1;
        sideToMove = true;

        istringstream ss(fen);
        string pieces, side, castling, ep, halfmove, fullmove;
        if (!(ss >> pieces)) return;
        if (!(ss >> side)) side = "w";
        if (!(ss >> castling)) castling = "-";
        if (!(ss >> ep)) ep = "-";
        if (!(ss >> halfmove)) halfmove = "0";
        if (!(ss >> fullmove)) fullmove = "1";

        int rank = 7, file = 0;
        for (char c : pieces) {
            if (c == '/') { rank--; file = 0; }
            else if (isdigit(c)) { file += (c - '0'); }
            else {
                int square = rank * 8 + file;
                if (c == 'P') whitePawn |= (1ULL << square);
                else if (c == 'N') whiteKnight |= (1ULL << square);
                else if (c == 'B') whiteBishop |= (1ULL << square);
                else if (c == 'R') whiteRook |= (1ULL << square);
                else if (c == 'Q') whiteQueen |= (1ULL << square);
                else if (c == 'K') whiteKing |= (1ULL << square);
                else if (c == 'p') blackPawn |= (1ULL << square);
                else if (c == 'n') blackKnight |= (1ULL << square);
                else if (c == 'b') blackBishop |= (1ULL << square);
                else if (c == 'r') blackRook |= (1ULL << square);
                else if (c == 'q') blackQueen |= (1ULL << square);
                else if (c == 'k') blackKing |= (1ULL << square);
                file++;
            }
        }
        sideToMove = (side == "w");
        for (char c : castling) {
            if (c == 'K') castlingRights |= WK;
            else if (c == 'Q') castlingRights |= WQ;
            else if (c == 'k') castlingRights |= BK;
            else if (c == 'q') castlingRights |= BQ;
        }
        if (ep != "-" && ep.length() >= 2) {
            enPassantSquare = (ep[1] - '1') * 8 + (ep[0] - 'a');
        }
        try {
            halfmoveClock = stoi(halfmove);
            fullmoveNumber = stoi(fullmove);
        } catch (...) {}
        hashKey = generateHashKey();
    }

    uint16_t parseMove(string moveStr) {
        vector<uint16_t> moves = generateMoves(sideToMove);
        for (uint16_t move : moves) {
            if (moveToString(move) == moveStr) {
                UndoInfo undo;
                if (makeMove(move, undo)) {
                    unmakeMove(move, undo);
                    return move;
                }
            }
        }
        return 0;
    }

    void init() {
        for (int i = 0; i < 64; i++) castlingMask[i] = 15;
        castlingMask[0] = 13; castlingMask[7] = 14; castlingMask[4] = 12;
        castlingMask[56] = 7; castlingMask[63] = 11; castlingMask[60] = 3;
        tt.resize(TT_SIZE);
        clearTT();
        resetBoard();
        calculateLeapers();
    }

    void calculateLeapers() {
        const uint64_t notAFile = 0xFEFEFEFEFEFEFEFEULL;
        const uint64_t notHFile = 0x7F7F7F7F7F7F7F7FULL;
        const uint64_t notABFile = 0xFCFCFCFCFCFCFCFCULL;
        const uint64_t notGHFile = 0x3F3F3F3F3F3F3F3FULL;
        for (int square = 0; square < 64; square++) {
            uint64_t startingSquare = 1ULL << square;
            uint64_t kingMoves = 0;
            kingMoves |= (startingSquare << 8) | (startingSquare >> 8);
            kingMoves |= ((startingSquare & notHFile) << 1) | ((startingSquare & notAFile) >> 1);
            kingMoves |= ((startingSquare & notHFile) << 9) | ((startingSquare & notAFile) << 7);
            kingMoves |= ((startingSquare & notHFile) >> 7) | ((startingSquare & notAFile) >> 9);
            kingMoveMask[square] = kingMoves;
            uint64_t knightMoves = 0;
            knightMoves |= ((startingSquare & notHFile) << 17) | ((startingSquare & notGHFile) << 10) | ((startingSquare & notGHFile) >> 6) | ((startingSquare & notHFile) >> 15);
            knightMoves |= ((startingSquare & notAFile) << 15) | ((startingSquare & notABFile) << 6) | ((startingSquare & notABFile) >> 10) | ((startingSquare & notAFile) >> 17);
            knightMoveMask[square] = knightMoves;
            }
            }


    uint64_t getRookMoves(int square, uint64_t occupancy) const {
        uint64_t attacks = 0;
        uint64_t fileA = 0x0101010101010101ULL, fileH = 0x8080808080808080ULL;
        uint64_t rank1 = 0x00000000000000FFULL, rank8 = 0xFF00000000000000ULL;
        int copy = square;
        while (!((1ULL << copy) & rank8)) { copy += 8; attacks |= (1ULL << copy); if ((1ULL << copy) & occupancy) break; }
        copy = square;
        while (!((1ULL << copy) & rank1)) { copy -= 8; attacks |= (1ULL << copy); if ((1ULL << copy) & occupancy) break; }
        copy = square;
        while (!((1ULL << copy) & fileH)) { copy += 1; attacks |= (1ULL << copy); if ((1ULL << copy) & occupancy) break; }
        copy = square;
        while (!((1ULL << copy) & fileA)) { copy -= 1; attacks |= (1ULL << copy); if ((1ULL << copy) & occupancy) break; }
        return attacks;
    }

    uint64_t getBishopAttacks(int square, uint64_t occupancy) const {
        uint64_t attacks = 0;
        uint64_t fileA = 0x0101010101010101ULL, fileH = 0x8080808080808080ULL;
        uint64_t rank1 = 0x00000000000000FFULL, rank8 = 0xFF00000000000000ULL;
        int copy = square;
        while (!((1ULL << copy) & rank8) && !((1ULL << copy) & fileH)) { copy += 9; attacks |= (1ULL << copy); if ((1ULL << copy) & occupancy) break; }
        copy = square;
        while (!((1ULL << copy) & rank8) && !((1ULL << copy) & fileA)) { copy += 7; attacks |= (1ULL << copy); if ((1ULL << copy) & occupancy) break; }
        copy = square;
        while (!((1ULL << copy) & rank1) && !((1ULL << copy) & fileH)) { copy -= 7; attacks |= (1ULL << copy); if ((1ULL << copy) & occupancy) break; }
        copy = square;
        while (!((1ULL << copy) & rank1) && !((1ULL << copy) & fileA)) { copy -= 9; attacks |= (1ULL << copy); if ((1ULL << copy) & occupancy) break; }
        return attacks;
    }

    uint64_t attackedSquares(bool side) const {
        uint64_t attacks = 0;
        uint64_t occupancy = (whitePawn | whiteKing | whiteQueen | whiteBishop | whiteKnight | whiteRook) |
                             (blackPawn | blackKing | blackQueen | blackBishop | blackKnight | blackRook);
        const uint64_t notAFile = 0xFEFEFEFEFEFEFEFEULL, notHFile = 0x7F7F7F7F7F7F7F7FULL;
        if (side) {
            attacks |= (whitePawn << 7) & notHFile;
            attacks |= (whitePawn << 9) & notAFile;
            uint64_t knights = whiteKnight;
            while (knights) { int sq = __builtin_ctzll(knights); attacks |= knightMoveMask[sq]; knights &= (knights - 1); }
            uint64_t diagSliders = whiteBishop | whiteQueen;
            while (diagSliders) { int sq = __builtin_ctzll(diagSliders); attacks |= getBishopAttacks(sq, occupancy); diagSliders &= (diagSliders - 1); }
            uint64_t orthSliders = whiteRook | whiteQueen;
            while (orthSliders) { int sq = __builtin_ctzll(orthSliders); attacks |= getRookMoves(sq, occupancy); orthSliders &= (orthSliders - 1); }
            if (whiteKing) attacks |= kingMoveMask[__builtin_ctzll(whiteKing)];
        } else {
            attacks |= (blackPawn >> 7) & notAFile;
            attacks |= (blackPawn >> 9) & notHFile;
            uint64_t knights = blackKnight;
            while (knights) { int sq = __builtin_ctzll(knights); attacks |= knightMoveMask[sq]; knights &= (knights - 1); }
            uint64_t diagSliders = blackBishop | blackQueen;
            while (diagSliders) { int sq = __builtin_ctzll(diagSliders); attacks |= getBishopAttacks(sq, occupancy); diagSliders &= (diagSliders - 1); }
            uint64_t orthSliders = blackRook | blackQueen;
            while (orthSliders) { int sq = __builtin_ctzll(orthSliders); attacks |= getRookMoves(sq, occupancy); orthSliders &= (orthSliders - 1); }
            if (blackKing) attacks |= kingMoveMask[__builtin_ctzll(blackKing)];
        }
        return attacks;
    }

    void generateRookMoves(uint64_t rookBoard, uint64_t occupied, uint64_t friendly, vector<uint16_t>& moveset) {
        uint64_t rank8 = 0xFF00000000000000ULL, rank1 = 0x00000000000000FFULL;
        uint64_t fileH = 0x8080808080808080ULL, fileA = 0x0101010101010101ULL;
        while (rookBoard > 0) {
            int from = __builtin_ctzll(rookBoard);
            int copy = from;
            while (!((1ULL << copy) & rank8)) { copy += 8; if ((1ULL << copy) & friendly) break; int flag = ((1ULL << copy) & occupied) ? CAPTURE : QUIET; moveset.push_back(package_move(from, copy, flag)); if ((1ULL << copy) & occupied) break; }
            copy = from;
            while (!((1ULL << copy) & rank1)) { copy -= 8; if ((1ULL << copy) & friendly) break; int flag = ((1ULL << copy) & occupied) ? CAPTURE : QUIET; moveset.push_back(package_move(from, copy, flag)); if ((1ULL << copy) & occupied) break; }
            copy = from;
            while (!((1ULL << copy) & fileH)) { copy += 1; if ((1ULL << copy) & friendly) break; int flag = ((1ULL << copy) & occupied) ? CAPTURE : QUIET; moveset.push_back(package_move(from, copy, flag)); if ((1ULL << copy) & occupied) break; }
            copy = from;
            while (!((1ULL << copy) & fileA)) { copy -= 1; if ((1ULL << copy) & friendly) break; int flag = ((1ULL << copy) & occupied) ? CAPTURE : QUIET; moveset.push_back(package_move(from, copy, flag)); if ((1ULL << copy) & occupied) break; }
            rookBoard &= (rookBoard - 1);
        }
    }

    void generateBishopMoves(uint64_t bishopBoard, uint64_t occupied, uint64_t friendly, vector<uint16_t>& moveset) {
        uint64_t rank8 = 0xFF00000000000000ULL, rank1 = 0x00000000000000FFULL;
        uint64_t fileH = 0x8080808080808080ULL, fileA = 0x0101010101010101ULL;
        while (bishopBoard > 0) {
            int from = __builtin_ctzll(bishopBoard);
            int copy = from;
            while (!((1ULL << copy) & rank8) && !((1ULL << copy) & fileH)) { copy += 9; if ((1ULL << copy) & friendly) break; int flag = ((1ULL << copy) & occupied) ? CAPTURE : QUIET; moveset.push_back(package_move(from, copy, flag)); if ((1ULL << copy) & occupied) break; }
            copy = from;
            while (!((1ULL << copy) & fileA) && !((1ULL << copy) & rank8)) { copy += 7; if ((1ULL << copy) & friendly) break; int flag = ((1ULL << copy) & occupied) ? CAPTURE : QUIET; moveset.push_back(package_move(from, copy, flag)); if ((1ULL << copy) & occupied) break; }
            copy = from;
            while (!((1ULL << copy) & fileH) && !((1ULL << copy) & rank1)) { copy -= 7; if ((1ULL << copy) & friendly) break; int flag = ((1ULL << copy) & occupied) ? CAPTURE : QUIET; moveset.push_back(package_move(from, copy, flag)); if ((1ULL << copy) & occupied) break; }
            copy = from;
            while (!((1ULL << copy) & fileA) && !((1ULL << copy) & rank1)) { copy -= 9; if ((1ULL << copy) & friendly) break; int flag = ((1ULL << copy) & occupied) ? CAPTURE : QUIET; moveset.push_back(package_move(from, copy, flag)); if ((1ULL << copy) & occupied) break; }
            bishopBoard &= (bishopBoard - 1);
        }
    }

    void generateQueenMoves(uint64_t queenBoard, uint64_t occupied, uint64_t friendly, vector<uint16_t>& moveset) {
        generateRookMoves(queenBoard, occupied, friendly, moveset);
        generateBishopMoves(queenBoard, occupied, friendly, moveset);
    }

    void generateKnightMoves(uint64_t knightBoard, uint64_t occupied, uint64_t friendly, vector<uint16_t>& moveset) {
        while (knightBoard > 0) {
            int from = __builtin_ctzll(knightBoard);
            uint64_t attacks = knightMoveMask[from] & ~friendly;
            while (attacks) {
                int to = __builtin_ctzll(attacks);
                int flag = ((1ULL << to) & occupied) ? CAPTURE : QUIET;
                moveset.push_back(package_move(from, to, flag));
                attacks &= (attacks - 1);
            }
            knightBoard &= (knightBoard - 1);
        }
    }

    void generateKingMoves(uint64_t kingBoard, uint64_t occupied, uint64_t friendly, vector<uint16_t>& moveset) {
        if (kingBoard == 0) return;
        int from = __builtin_ctzll(kingBoard);
        uint64_t attacks = kingMoveMask[from] & ~friendly;
        while (attacks) {
            int to = __builtin_ctzll(attacks);
            int flag = ((1ULL << to) & occupied) ? CAPTURE : QUIET;
            moveset.push_back(package_move(from, to, flag));
            attacks &= (attacks - 1);
        }
    }

    void generateCastlingMoves(bool side, uint64_t occupied, vector<uint16_t>& moveSet) {
        uint64_t attacked = attackedSquares(!side);
        if (side) {
            if ((castlingRights & WK) && !(occupied & ((1ULL << 5) | (1ULL << 6))))
                if (!(attacked & ((1ULL << 4) | (1ULL << 5) | (1ULL << 6)))) moveSet.push_back(package_move(4, 6, K_CASTLE));
            if ((castlingRights & WQ) && !(occupied & ((1ULL << 3) | (1ULL << 2) | (1ULL << 1))))
                if (!(attacked & ((1ULL << 4) | (1ULL << 3) | (1ULL << 2)))) moveSet.push_back(package_move(4, 2, Q_CASTLE));
        } else {
            if ((castlingRights & BK) && !(occupied & ((1ULL << 61) | (1ULL << 62))))
                if (!(attacked & ((1ULL << 60) | (1ULL << 61) | (1ULL << 62)))) moveSet.push_back(package_move(60, 62, K_CASTLE));
            if ((castlingRights & BQ) && !(occupied & ((1ULL << 59) | (1ULL << 58) | (1ULL << 57))))
                if (!(attacked & ((1ULL << 60) | (1ULL << 59) | (1ULL << 58)))) moveSet.push_back(package_move(60, 58, Q_CASTLE));
        }
    }

    void generateWhitePawnMoves(uint64_t whitePawns, uint64_t occupied, uint64_t friendly, vector<uint16_t>& moveSet) {
        uint64_t rank2 = 0x000000000000FF00ULL, fileH = 0x8080808080808080ULL, fileA = 0x0101010101010101ULL;
        while (whitePawns > 0) {
            int from = __builtin_ctzll(whitePawns);
            int to = from + 8;
            if (!((1ULL << to) & occupied)) {
                if (to >= 56) { moveSet.push_back(package_move(from, to, PROMO_KNIGHT)); moveSet.push_back(package_move(from, to, PROMO_BISHOP)); moveSet.push_back(package_move(from, to, PROMO_ROOK)); moveSet.push_back(package_move(from, to, PROMO_QUEEN)); }
                else { moveSet.push_back(package_move(from, to, QUIET)); if (((1ULL << from) & rank2) && !((1ULL << (from + 16)) & occupied)) moveSet.push_back(package_move(from, from + 16, DOUBLE_PUSH)); }
            }
            if (!((1ULL << from) & fileA)) { int tl = from + 7; if (((1ULL << tl) & occupied) && !((1ULL << tl) & friendly)) { if (tl >= 56) { moveSet.push_back(package_move(from, tl, PROMO_KNIGHT_CAP)); moveSet.push_back(package_move(from, tl, PROMO_BISHOP_CAP)); moveSet.push_back(package_move(from, tl, PROMO_ROOK_CAP)); moveSet.push_back(package_move(from, tl, PROMO_QUEEN_CAP)); } else moveSet.push_back(package_move(from, tl, CAPTURE)); } else if (tl == enPassantSquare) moveSet.push_back(package_move(from, tl, EP_CAPTURE)); }
            if (!((1ULL << from) & fileH)) { int tr = from + 9; if (((1ULL << tr) & occupied) && !((1ULL << tr) & friendly)) { if (tr >= 56) { moveSet.push_back(package_move(from, tr, PROMO_KNIGHT_CAP)); moveSet.push_back(package_move(from, tr, PROMO_BISHOP_CAP)); moveSet.push_back(package_move(from, tr, PROMO_ROOK_CAP)); moveSet.push_back(package_move(from, tr, PROMO_QUEEN_CAP)); } else moveSet.push_back(package_move(from, tr, CAPTURE)); } else if (tr == enPassantSquare) moveSet.push_back(package_move(from, tr, EP_CAPTURE)); }
            whitePawns &= (whitePawns - 1);
        }
    }

    void generateBlackPawnMoves(uint64_t blackPawns, uint64_t occupied, uint64_t friendly, vector<uint16_t>& moveSet) {
        uint64_t rank7 = 0x00FF000000000000ULL, fileH = 0x8080808080808080ULL, fileA = 0x0101010101010101ULL;
        while (blackPawns > 0) {
            int from = __builtin_ctzll(blackPawns);
            int to = from - 8;
            if (!((1ULL << to) & occupied)) {
                if (to <= 7) { moveSet.push_back(package_move(from, to, PROMO_KNIGHT)); moveSet.push_back(package_move(from, to, PROMO_BISHOP)); moveSet.push_back(package_move(from, to, PROMO_ROOK)); moveSet.push_back(package_move(from, to, PROMO_QUEEN)); }
                else { moveSet.push_back(package_move(from, to, QUIET)); if (((1ULL << from) & rank7) && !((1ULL << (from - 16)) & occupied)) moveSet.push_back(package_move(from, from - 16, DOUBLE_PUSH)); }
            }
            if (!((1ULL << from) & fileA)) { int tl = from - 9; if (((1ULL << tl) & occupied) && !((1ULL << tl) & friendly)) { if (tl <= 7) { moveSet.push_back(package_move(from, tl, PROMO_KNIGHT_CAP)); moveSet.push_back(package_move(from, tl, PROMO_BISHOP_CAP)); moveSet.push_back(package_move(from, tl, PROMO_ROOK_CAP)); moveSet.push_back(package_move(from, tl, PROMO_QUEEN_CAP)); } else moveSet.push_back(package_move(from, tl, CAPTURE)); } else if (tl == enPassantSquare) moveSet.push_back(package_move(from, tl, EP_CAPTURE)); }
            if (!((1ULL << from) & fileH)) { int tr = from - 7; if (((1ULL << tr) & occupied) && !((1ULL << tr) & friendly)) { if (tr <= 7) { moveSet.push_back(package_move(from, tr, PROMO_KNIGHT_CAP)); moveSet.push_back(package_move(from, tr, PROMO_BISHOP_CAP)); moveSet.push_back(package_move(from, tr, PROMO_ROOK_CAP)); moveSet.push_back(package_move(from, tr, PROMO_QUEEN_CAP)); } else moveSet.push_back(package_move(from, tr, CAPTURE)); } else if (tr == enPassantSquare) moveSet.push_back(package_move(from, tr, EP_CAPTURE)); }
            blackPawns &= (blackPawns - 1);
        }
    }

    vector<uint16_t> generateMoves(bool side) {
        uint64_t allWhite = whitePawn | whiteKing | whiteQueen | whiteBishop | whiteKnight | whiteRook;
        uint64_t allBlack = blackPawn | blackKing | blackQueen | blackBishop | blackKnight | blackRook;
        uint64_t occupied = allWhite | allBlack;
        vector<uint16_t> moveSet;
        if (side) {
            generateRookMoves(whiteRook, occupied, allWhite, moveSet);
            generateBishopMoves(whiteBishop, occupied, allWhite, moveSet);
            generateQueenMoves(whiteQueen, occupied, allWhite, moveSet);
            generateKnightMoves(whiteKnight, occupied, allWhite, moveSet);
            generateKingMoves(whiteKing, occupied, allWhite, moveSet);
            generateWhitePawnMoves(whitePawn, occupied, allWhite, moveSet);
            generateCastlingMoves(side, occupied, moveSet);
        } else {
            generateRookMoves(blackRook, occupied, allBlack, moveSet);
            generateBishopMoves(blackBishop, occupied, allBlack, moveSet);
            generateQueenMoves(blackQueen, occupied, allBlack, moveSet);
            generateKnightMoves(blackKnight, occupied, allBlack, moveSet);
            generateKingMoves(blackKing, occupied, allBlack, moveSet);
            generateBlackPawnMoves(blackPawn, occupied, allBlack, moveSet);
            generateCastlingMoves(side, occupied, moveSet);
        }
        return moveSet;
    }

    string squareToCoord(int sq) {
        string s = ""; s += (char)('a' + (sq % 8)); s += (char)('1' + (sq / 8)); return s;
    }

    string moveToString(uint16_t move) {
        int from = getFrom(move), to = getTo(move), flags = getFlags(move);
        string s = squareToCoord(from) + squareToCoord(to);
        if (flags >= PROMO_KNIGHT) {
            int pt = flags & 3;
            if (pt == 0) s += 'n'; else if (pt == 1) s += 'b'; else if (pt == 2) s += 'r'; else if (pt == 3) s += 'q';
        }
        return s;
    }

    uint64_t perft(int depth) {
        if (depth == 0) return 1ULL;
        uint64_t nodes = 0;
        vector<uint16_t> moves = generateMoves(sideToMove);
        for (uint16_t move : moves) {
            UndoInfo undo;
            if (makeMove(move, undo)) { nodes += perft(depth - 1); unmakeMove(move, undo); }
        }
        return nodes;
    }

    void perftDivide(int depth) {
        uint64_t totalNodes = 0;
        vector<uint16_t> moves = generateMoves(sideToMove);
        for (uint16_t move : moves) {
            UndoInfo undo;
            if (makeMove(move, undo)) { uint64_t nodes = perft(depth - 1); unmakeMove(move, undo); cout << moveToString(move) << ": " << nodes << endl; totalNodes += nodes; }
        }
        cout << "\nTotal nodes at depth " << depth << ": " << totalNodes << endl;
    }
};

void uciLoop(Board& myBoard) {
    string command;
    thread searchThread;
    auto stopSearching = [&]() {
        myBoard.stopSearch = true;
        if (searchThread.joinable()) searchThread.join();
    };

    while (getline(cin, command)) {
        istringstream ss(command);
        string token;
        ss >> token;
        if (token == "uci") {
            stopSearching();
            cout << "id name GrandmasterZero" << endl;
            cout << "id author Arnav" << endl;
            cout << "uciok" << endl;
        } else if (token == "isready") {
            cout << "readyok" << endl;
        } else if (token == "ucinewgame") {
            stopSearching();
            myBoard.clearTT();
            myBoard.parseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        } else if (token == "position") {
            stopSearching();
            string sub;
            ss >> sub;
            if (sub == "startpos") {
                myBoard.parseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
                ss >> sub;
            } else if (sub == "fen") {
                string fenStr = "";
                while (ss >> sub && sub != "moves") fenStr += sub + " ";
                myBoard.parseFEN(fenStr);
            }
            if (sub == "moves") {
                string moveStr;
                while (ss >> moveStr) {
                    uint16_t move = myBoard.parseMove(moveStr);
                    if (move) { 
                        Board::UndoInfo undo; 
                        myBoard.makeMove(move, undo); 
                    } else {
                        break;
                    }
                }
            }
        } else if (token == "perft") {
            stopSearching();
            int depth; if (ss >> depth) myBoard.perftDivide(depth);
        } else if (token == "eval") {
            cout << "Evaluation: " << myBoard.evaluate() << " centipawns" << endl;
        } else if (token == "go") {
            stopSearching();
            Board::SearchLimits limits;
            string sub;
            while (ss >> sub) {
                if (sub == "wtime") ss >> limits.wtime;
                else if (sub == "btime") ss >> limits.btime;
                else if (sub == "winc") ss >> limits.winc;
                else if (sub == "binc") ss >> limits.binc;
                else if (sub == "movestogo") ss >> limits.movestogo;
                else if (sub == "depth") ss >> limits.depth;
                else if (sub == "movetime") ss >> limits.movetime;
                else if (sub == "infinite") limits.infinite = true;
            }
            myBoard.stopSearch = false;
            searchThread = thread([&myBoard, limits]() {
                uint16_t bestMove = myBoard.search(limits);
                cout << "bestmove " << myBoard.moveToString(bestMove) << endl;
            });
        } else if (token == "stop") {
            stopSearching();
        } else if (token == "quit") {
            stopSearching();
            break;
        }
    }
    stopSearching();
}

#ifndef UNIT_TEST
int main() {
    Evaluation::initZobrist();
    Board myBoard;
    myBoard.init();
    uciLoop(myBoard);
    return 0;
}
#endif
