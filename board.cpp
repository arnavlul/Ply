#include "board.h"
Evaluation::Score Board::evaluatePiece(uint64_t bb, int val, const Evaluation::Score pst[64], bool white) const {

        Evaluation::Score s = {0, 0};
        while (bb) {
            int sq = __builtin_ctzll(bb);
            int idx = white ? sq : (sq ^ 56);
            s.mg += val + pst[idx].mg;
            s.eg += val + pst[idx].eg;
            bb &= (bb - 1);
        }
        return s;
    
}

void Board::checkTime()  {

        if (timeLimit != -1) {
            auto now = chrono::steady_clock::now();
            auto elapsed = chrono::duration_cast<chrono::milliseconds>(now - startTime).count();
            if (elapsed >= timeLimit) stopSearch = true;
        }
    
}

uint64_t Board::generateHashKey()  {

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

uint64_t Board::getPolyglotHash() const {

        uint64_t h = 0;
        uint64_t occupancy = (whitePawn | whiteKing | whiteQueen | whiteBishop | whiteKnight | whiteRook) |
                             (blackPawn | blackKing | blackQueen | blackBishop | blackKnight | blackRook);
        uint64_t temp = occupancy;
        while (temp) {
            int sq = __builtin_ctzll(temp);
            int pt = getPieceAt(sq, true);
            if (pt != NONE) h ^= Polyglot::RANDOM_ARRAY[64 * (2 * pt - 1) + sq];
            else {
                pt = getPieceAt(sq, false);
                if (pt != NONE) h ^= Polyglot::RANDOM_ARRAY[64 * (2 * pt - 2) + sq];
            }
            temp &= (temp - 1);
        }
        if (castlingRights & WK) h ^= Polyglot::RANDOM_ARRAY[768];
        if (castlingRights & WQ) h ^= Polyglot::RANDOM_ARRAY[769];
        if (castlingRights & BK) h ^= Polyglot::RANDOM_ARRAY[770];
        if (castlingRights & BQ) h ^= Polyglot::RANDOM_ARRAY[771];
        if (enPassantSquare != -1) {
            bool capturePossible = false;
            if (sideToMove) {
                if (enPassantSquare % 8 > 0 && (whitePawn & (1ULL << (enPassantSquare - 9)))) capturePossible = true;
                if (enPassantSquare % 8 < 7 && (whitePawn & (1ULL << (enPassantSquare - 7)))) capturePossible = true;
            } else {
                if (enPassantSquare % 8 > 0 && (blackPawn & (1ULL << (enPassantSquare + 7)))) capturePossible = true;
                if (enPassantSquare % 8 < 7 && (blackPawn & (1ULL << (enPassantSquare + 9)))) capturePossible = true;
            }
            if (capturePossible) h ^= Polyglot::RANDOM_ARRAY[772 + (enPassantSquare % 8)];
        }
        if (sideToMove) h ^= Polyglot::RANDOM_ARRAY[780];
        return h;
    
}

int Board::evaluate() const {

        Evaluation::Score score = {0, 0};
        int phase = 0;

        uint64_t occupancy = (whitePawn | whiteKing | whiteQueen | whiteBishop | whiteKnight | whiteRook) |
                             (blackPawn | blackKing | blackQueen | blackBishop | blackKnight | blackRook);

        // White pieces
        score += evaluatePiece(whitePawn, Evaluation::PAWN_VAL, Evaluation::pawn_pst, true);
        
        Evaluation::Score wN = evaluatePiece(whiteKnight, Evaluation::KNIGHT_VAL, Evaluation::knight_pst, true);
        score += wN;
        phase += __builtin_popcountll(whiteKnight) * 1;

        Evaluation::Score wB = evaluatePiece(whiteBishop, Evaluation::BISHOP_VAL, Evaluation::bishop_pst, true);
        score += wB;
        phase += __builtin_popcountll(whiteBishop) * 1;
        if (__builtin_popcountll(whiteBishop) >= 2) score += {Evaluation::BISHOP_PAIR_BONUS, Evaluation::BISHOP_PAIR_BONUS};

        Evaluation::Score wR = evaluatePiece(whiteRook, Evaluation::ROOK_VAL, Evaluation::rook_pst, true);
        score += wR;
        phase += __builtin_popcountll(whiteRook) * 2;

        Evaluation::Score wQ = evaluatePiece(whiteQueen, Evaluation::QUEEN_VAL, Evaluation::queen_pst, true);
        score += wQ;
        phase += __builtin_popcountll(whiteQueen) * 4;

        score += evaluatePiece(whiteKing, Evaluation::KING_VAL, Evaluation::king_pst, true);

        // Black pieces
        score -= evaluatePiece(blackPawn, Evaluation::PAWN_VAL, Evaluation::pawn_pst, false);
        
        Evaluation::Score bN = evaluatePiece(blackKnight, Evaluation::KNIGHT_VAL, Evaluation::knight_pst, false);
        score -= bN;
        phase += __builtin_popcountll(blackKnight) * 1;

        Evaluation::Score bB = evaluatePiece(blackBishop, Evaluation::BISHOP_VAL, Evaluation::bishop_pst, false);
        score -= bB;
        phase += __builtin_popcountll(blackBishop) * 1;
        if (__builtin_popcountll(blackBishop) >= 2) score -= {Evaluation::BISHOP_PAIR_BONUS, Evaluation::BISHOP_PAIR_BONUS};

        Evaluation::Score bR = evaluatePiece(blackRook, Evaluation::ROOK_VAL, Evaluation::rook_pst, false);
        score -= bR;
        phase += __builtin_popcountll(blackRook) * 2;

        Evaluation::Score bQ = evaluatePiece(blackQueen, Evaluation::QUEEN_VAL, Evaluation::queen_pst, false);
        score -= bQ;
        phase += __builtin_popcountll(blackQueen) * 4;

        score -= evaluatePiece(blackKing, Evaluation::KING_VAL, Evaluation::king_pst, false);

        // Pawn Structure & Passed Pawns
        uint64_t wp = whitePawn;
        while (wp) {
            int sq = __builtin_ctzll(wp);
            int file = sq % 8;
            // Doubled pawns
            if (__builtin_popcountll(whitePawn & Evaluation::fileMasks[file]) > 1) score -= {Evaluation::DOUBLED_PAWN_PENALTY, Evaluation::DOUBLED_PAWN_PENALTY};
            // Isolated pawns
            if (!(whitePawn & Evaluation::adjacentFileMasks[file])) score -= {Evaluation::ISOLATED_PAWN_PENALTY, Evaluation::ISOLATED_PAWN_PENALTY};
            // Passed pawns
            if (!(blackPawn & Evaluation::passedPawnMasks[1][sq]))
                score += Evaluation::passedPawnBonus[sq / 8];
            wp &= (wp - 1);
        }
        uint64_t bp = blackPawn;
        while (bp) {
            int sq = __builtin_ctzll(bp);
            int file = sq % 8;
            // Doubled pawns
            if (__builtin_popcountll(blackPawn & Evaluation::fileMasks[file]) > 1) score += {Evaluation::DOUBLED_PAWN_PENALTY, Evaluation::DOUBLED_PAWN_PENALTY};
            // Isolated pawns
            if (!(blackPawn & Evaluation::adjacentFileMasks[file])) score += {Evaluation::ISOLATED_PAWN_PENALTY, Evaluation::ISOLATED_PAWN_PENALTY};
            // Passed pawns
            if (!(whitePawn & Evaluation::passedPawnMasks[0][sq]))
                score -= Evaluation::passedPawnBonus[7 - (sq / 8)];
            bp &= (bp - 1);
        }

        // Rook Evaluation
        uint64_t wr = whiteRook;
        while (wr) {
            int sq = __builtin_ctzll(wr);
            int file = sq % 8;
            if (!(whitePawn & Evaluation::fileMasks[file])) {
                if (!(blackPawn & Evaluation::fileMasks[file])) score += {Evaluation::ROOK_OPEN_FILE_BONUS, Evaluation::ROOK_OPEN_FILE_BONUS};
                else score += {Evaluation::ROOK_SEMI_OPEN_FILE_BONUS, Evaluation::ROOK_SEMI_OPEN_FILE_BONUS};
            }
            if (sq / 8 == 6) score += {Evaluation::ROOK_ON_7TH_BONUS, Evaluation::ROOK_ON_7TH_BONUS};
            wr &= (wr - 1);
        }
        uint64_t br = blackRook;
        while (br) {
            int sq = __builtin_ctzll(br);
            int file = sq % 8;
            if (!(blackPawn & Evaluation::fileMasks[file])) {
                if (!(whitePawn & Evaluation::fileMasks[file])) score -= {Evaluation::ROOK_OPEN_FILE_BONUS, Evaluation::ROOK_OPEN_FILE_BONUS};
                else score -= {Evaluation::ROOK_SEMI_OPEN_FILE_BONUS, Evaluation::ROOK_SEMI_OPEN_FILE_BONUS};
            }
            if (sq / 8 == 1) score -= {Evaluation::ROOK_ON_7TH_BONUS, Evaluation::ROOK_ON_7TH_BONUS};
            br &= (br - 1);
        }

        // King Safety
        int wKingSq = __builtin_ctzll(whiteKing);
        if (wKingSq < 24) { // Ranks 1-3
            int pawnsInShield = __builtin_popcountll(whitePawn & Evaluation::pawnShieldMasks[1][wKingSq]);
            int missingShield = std::max(0, 3 - pawnsInShield);
            score.mg -= Evaluation::shieldPenalty[missingShield];
        }

        int bKingSq = __builtin_ctzll(blackKing);
        if (bKingSq >= 40) { // Ranks 6-8
            int pawnsInShield = __builtin_popcountll(blackPawn & Evaluation::pawnShieldMasks[0][bKingSq]);
            int missingShield = std::max(0, 3 - pawnsInShield);
            score.mg += Evaluation::shieldPenalty[missingShield];
        }

        // King Zone Attacks (Weighted)
        uint64_t wKz = Evaluation::kingZoneMasks[1][wKingSq];
        int wAttackScore = 0;
        uint64_t tempKnights = blackKnight;
        while (tempKnights) { int sq = __builtin_ctzll(tempKnights); wAttackScore += __builtin_popcountll(knightMoveMask[sq] & wKz) * Evaluation::attackWeight[KNIGHT]; tempKnights &= (tempKnights - 1); }
        uint64_t tempBishops = blackBishop | blackQueen;
        while (tempBishops) { int sq = __builtin_ctzll(tempBishops); wAttackScore += __builtin_popcountll(getBishopAttacks(sq, occupancy) & wKz) * Evaluation::attackWeight[BISHOP]; tempBishops &= (tempBishops - 1); }
        uint64_t tempRooks = blackRook | blackQueen;
        while (tempRooks) { int sq = __builtin_ctzll(tempRooks); wAttackScore += __builtin_popcountll(getRookMoves(sq, occupancy) & wKz) * Evaluation::attackWeight[ROOK]; tempRooks &= (tempRooks - 1); }
        if (wAttackScore > 99) wAttackScore = 99;
        score.mg -= Evaluation::safetyTable[wAttackScore];

        uint64_t bKz = Evaluation::kingZoneMasks[0][bKingSq];
        int bAttackScore = 0;
        tempKnights = whiteKnight;
        while (tempKnights) { int sq = __builtin_ctzll(tempKnights); bAttackScore += __builtin_popcountll(knightMoveMask[sq] & bKz) * Evaluation::attackWeight[KNIGHT]; tempKnights &= (tempKnights - 1); }
        tempBishops = whiteBishop | whiteQueen;
        while (tempBishops) { int sq = __builtin_ctzll(tempBishops); bAttackScore += __builtin_popcountll(getBishopAttacks(sq, occupancy) & bKz) * Evaluation::attackWeight[BISHOP]; tempBishops &= (tempBishops - 1); }
        tempRooks = whiteRook | whiteQueen;
        while (tempRooks) { int sq = __builtin_ctzll(tempRooks); bAttackScore += __builtin_popcountll(getRookMoves(sq, occupancy) & bKz) * Evaluation::attackWeight[ROOK]; tempRooks &= (tempRooks - 1); }
        if (bAttackScore > 99) bAttackScore = 99;
        score.mg += Evaluation::safetyTable[bAttackScore];

        const int totalPhase = 24;
        if (phase > totalPhase) phase = totalPhase;

        int finalScore = (score.mg * phase + score.eg * (totalPhase - phase)) / totalPhase;

        return sideToMove ? finalScore : -finalScore;
    
}



bool Board::getSideToMove() const {
 return sideToMove; 
}

uint8_t Board::getCastlingRights() const {
 return castlingRights; 
}

int Board::getEnPassantSquare() const {
 return enPassantSquare; 
}

int Board::getPieceAt(int square, bool side) const {

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

bool Board::inCheck(bool side) const {

        uint64_t kingBB = side ? whiteKing : blackKing;
        return (attackedSquares(!side) & kingBB) != 0;
    
}

int Board::getMoveScore(uint16_t move, uint16_t hashMove, int ply) const {

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
            return 1000000 + (getVal(victim) * 10) - getVal(attacker);
        }
        
        if (ply < 128) {
            if (move == killerMoves[ply][0]) return 900000;
            if (move == killerMoves[ply][1]) return 800000;
        }

        return historyHeuristic[sideToMove][getFrom(move)][getTo(move)];
    
}

void Board::togglePiece(int pieceType, int square, bool side)  {

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

bool Board::makeMove(uint16_t move, UndoInfo& undo)  {

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

        historyStack[historyIndex++] = hashKey;

        uint64_t kingBB = side ? whiteKing : blackKing;
        uint64_t attacks = attackedSquares(!side);
        if (attacks & kingBB) {
            unmakeMove(move, undo);
            return false;
        }
        return true;
    
}

void Board::unmakeMove(uint16_t move, const UndoInfo& undo)  {

        historyIndex--;
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

void Board::makeNullMove(UndoInfo& undo) {
    undo.castlingRights = castlingRights;
    undo.enPassantSquare = enPassantSquare;
    undo.halfmoveClock = halfmoveClock;
    undo.capturedPieceType = NONE;
    undo.move = 0;
    undo.hashKey = hashKey;

    if (enPassantSquare != -1) hashKey ^= Evaluation::enPassantKeys[enPassantSquare % 8];
    enPassantSquare = -1;

    bool side = sideToMove;
    sideToMove = !sideToMove;
    hashKey ^= Evaluation::sideKey;

    halfmoveClock++;
    if (!side) fullmoveNumber++;

    historyStack[historyIndex++] = hashKey;
}

void Board::unmakeNullMove(const UndoInfo& undo) {
    historyIndex--;
    bool side = !sideToMove; // The side that made the null move
    if (!side) fullmoveNumber--;
    
    sideToMove = !sideToMove;

    castlingRights = undo.castlingRights;
    enPassantSquare = undo.enPassantSquare;
    halfmoveClock = undo.halfmoveClock;
    hashKey = undo.hashKey;
}

uint16_t Board::package_move(int from, int to, int flags) const {
return (uint16_t)(from | (to << 6) | (flags << 12));
}

void Board::resetBoard()  {

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
        historyIndex = 0;
        historyStack[historyIndex++] = hashKey;
    
}

void Board::parseFEN(const string& fen)  {

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
        historyIndex = 0;
        historyStack[historyIndex++] = hashKey;
    
}

uint16_t Board::parseMove(string moveStr)  {

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

void Board::init()  {

        for (int i = 0; i < 64; i++) castlingMask[i] = 15;
        castlingMask[0] = 13; castlingMask[7] = 14; castlingMask[4] = 12;
        castlingMask[56] = 7; castlingMask[63] = 11; castlingMask[60] = 3;
        tt.resize(TT_SIZE);
        clearTT();
        resetBoard();
        calculateLeapers();
    
}

string Board::squareToCoord(int sq)  {

        string s = ""; s += (char)('a' + (sq % 8)); s += (char)('1' + (sq / 8)); return s;
    
}

string Board::moveToString(uint16_t move)  {

        int from = getFrom(move), to = getTo(move), flags = getFlags(move);
        string s = squareToCoord(from) + squareToCoord(to);
        if (flags >= PROMO_KNIGHT) {
            int pt = flags & 3;
            if (pt == 0) s += 'n'; else if (pt == 1) s += 'b'; else if (pt == 2) s += 'r'; else if (pt == 3) s += 'q';
        }
        return s;
    
}

uint64_t Board::perft(int depth)  {

        if (depth == 0) return 1ULL;
        uint64_t nodes = 0;
        vector<uint16_t> moves = generateMoves(sideToMove);
        for (uint16_t move : moves) {
            UndoInfo undo;
            if (makeMove(move, undo)) { nodes += perft(depth - 1); unmakeMove(move, undo); }
        }
        return nodes;
    
}

void Board::perftDivide(int depth)  {

        uint64_t totalNodes = 0;
        vector<uint16_t> moves = generateMoves(sideToMove);
        for (uint16_t move : moves) {
            UndoInfo undo;
            if (makeMove(move, undo)) { uint64_t nodes = perft(depth - 1); unmakeMove(move, undo); cout << moveToString(move) << ": " << nodes << endl; totalNodes += nodes; }
        }
        cout << "\nTotal nodes at depth " << depth << ": " << totalNodes << endl;
    
}

