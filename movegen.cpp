#include "board.h"
#include "magics.h"

void Board::calculateLeapers()  {

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

uint64_t Board::getRookMoves(int square, uint64_t occupancy) const {

        occupancy &= Magics::ROOK_MASKS[square];
        int index = (int)((occupancy * Magics::ROOK_MAGICS[square]) >> Magics::ROOK_SHIFTS[square]);
        return Magics::rookTable[square][index];
    
}

uint64_t Board::getBishopAttacks(int square, uint64_t occupancy) const {

        occupancy &= Magics::BISHOP_MASKS[square];
        int index = (int)((occupancy * Magics::BISHOP_MAGICS[square]) >> Magics::BISHOP_SHIFTS[square]);
        return Magics::bishopTable[square][index];
    
}

uint64_t Board::attackedSquares(bool side) const {

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

void Board::generateRookMoves(uint64_t rookBoard, uint64_t occupied, uint64_t friendly, MoveList& moveset) const {

        while (rookBoard > 0) {
            int from = __builtin_ctzll(rookBoard);
            uint64_t attacks = getRookMoves(from, occupied) & ~friendly;
            while (attacks) {
                int to = __builtin_ctzll(attacks);
                int flag = ((1ULL << to) & occupied) ? CAPTURE : QUIET;
                moveset.add(package_move(from, to, flag));
                attacks &= (attacks - 1);
            }
            rookBoard &= (rookBoard - 1);
        }
    
}

void Board::generateBishopMoves(uint64_t bishopBoard, uint64_t occupied, uint64_t friendly, MoveList& moveset) const {

        while (bishopBoard > 0) {
            int from = __builtin_ctzll(bishopBoard);
            uint64_t attacks = getBishopAttacks(from, occupied) & ~friendly;
            while (attacks) {
                int to = __builtin_ctzll(attacks);
                int flag = ((1ULL << to) & occupied) ? CAPTURE : QUIET;
                moveset.add(package_move(from, to, flag));
                attacks &= (attacks - 1);
            }
            bishopBoard &= (bishopBoard - 1);
        }
    
}

void Board::generateQueenMoves(uint64_t queenBoard, uint64_t occupied, uint64_t friendly, MoveList& moveset) const {

        generateRookMoves(queenBoard, occupied, friendly, moveset);
        generateBishopMoves(queenBoard, occupied, friendly, moveset);
    
}

void Board::generateKnightMoves(uint64_t knightBoard, uint64_t occupied, uint64_t friendly, MoveList& moveset) const {

        while (knightBoard > 0) {
            int from = __builtin_ctzll(knightBoard);
            uint64_t attacks = knightMoveMask[from] & ~friendly;
            while (attacks) {
                int to = __builtin_ctzll(attacks);
                int flag = ((1ULL << to) & occupied) ? CAPTURE : QUIET;
                moveset.add(package_move(from, to, flag));
                attacks &= (attacks - 1);
            }
            knightBoard &= (knightBoard - 1);
        }
    
}

void Board::generateKingMoves(uint64_t kingBoard, uint64_t occupied, uint64_t friendly, MoveList& moveset) const {

        if (kingBoard == 0) return;
        int from = __builtin_ctzll(kingBoard);
        uint64_t attacks = kingMoveMask[from] & ~friendly;
        while (attacks) {
            int to = __builtin_ctzll(attacks);
            int flag = ((1ULL << to) & occupied) ? CAPTURE : QUIET;
            moveset.add(package_move(from, to, flag));
            attacks &= (attacks - 1);
        }
    
}

void Board::generateCastlingMoves(bool side, uint64_t occupied, MoveList& moveSet) const {

        uint64_t attacked = attackedSquares(!side);
        if (side) {
            if ((castlingRights & WK) && !(occupied & ((1ULL << 5) | (1ULL << 6))))
                if (!(attacked & ((1ULL << 4) | (1ULL << 5) | (1ULL << 6)))) moveSet.add(package_move(4, 6, K_CASTLE));
            if ((castlingRights & WQ) && !(occupied & ((1ULL << 3) | (1ULL << 2) | (1ULL << 1))))
                if (!(attacked & ((1ULL << 4) | (1ULL << 3) | (1ULL << 2)))) moveSet.add(package_move(4, 2, Q_CASTLE));
        } else {
            if ((castlingRights & BK) && !(occupied & ((1ULL << 61) | (1ULL << 62))))
                if (!(attacked & ((1ULL << 60) | (1ULL << 61) | (1ULL << 62)))) moveSet.add(package_move(60, 62, K_CASTLE));
            if ((castlingRights & BQ) && !(occupied & ((1ULL << 59) | (1ULL << 58) | (1ULL << 57))))
                if (!(attacked & ((1ULL << 60) | (1ULL << 59) | (1ULL << 58)))) moveSet.add(package_move(60, 58, Q_CASTLE));
        }
    
}

void Board::generateWhitePawnMoves(uint64_t whitePawns, uint64_t occupied, uint64_t friendly, MoveList& moveSet) const {

        uint64_t rank2 = 0x000000000000FF00ULL, fileH = 0x8080808080808080ULL, fileA = 0x0101010101010101ULL;
        while (whitePawns > 0) {
            int from = __builtin_ctzll(whitePawns);
            int to = from + 8;
            if (!((1ULL << to) & occupied)) {
                if (to >= 56) { moveSet.add(package_move(from, to, PROMO_KNIGHT)); moveSet.add(package_move(from, to, PROMO_BISHOP)); moveSet.add(package_move(from, to, PROMO_ROOK)); moveSet.add(package_move(from, to, PROMO_QUEEN)); }
                else { moveSet.add(package_move(from, to, QUIET)); if (((1ULL << from) & rank2) && !((1ULL << (from + 16)) & occupied)) moveSet.add(package_move(from, from + 16, DOUBLE_PUSH)); }
            }
            if (!((1ULL << from) & fileA)) { int tl = from + 7; if (((1ULL << tl) & occupied) && !((1ULL << tl) & friendly)) { if (tl >= 56) { moveSet.add(package_move(from, tl, PROMO_KNIGHT_CAP)); moveSet.add(package_move(from, tl, PROMO_BISHOP_CAP)); moveSet.add(package_move(from, tl, PROMO_ROOK_CAP)); moveSet.add(package_move(from, tl, PROMO_QUEEN_CAP)); } else moveSet.add(package_move(from, tl, CAPTURE)); } else if (tl == enPassantSquare) moveSet.add(package_move(from, tl, EP_CAPTURE)); }
            if (!((1ULL << from) & fileH)) { int tr = from + 9; if (((1ULL << tr) & occupied) && !((1ULL << tr) & friendly)) { if (tr >= 56) { moveSet.add(package_move(from, tr, PROMO_KNIGHT_CAP)); moveSet.add(package_move(from, tr, PROMO_BISHOP_CAP)); moveSet.add(package_move(from, tr, PROMO_ROOK_CAP)); moveSet.add(package_move(from, tr, PROMO_QUEEN_CAP)); } else moveSet.add(package_move(from, tr, CAPTURE)); } else if (tr == enPassantSquare) moveSet.add(package_move(from, tr, EP_CAPTURE)); }
            whitePawns &= (whitePawns - 1);
        }
    
}

void Board::generateBlackPawnMoves(uint64_t blackPawns, uint64_t occupied, uint64_t friendly, MoveList& moveSet) const {

        uint64_t rank7 = 0x00FF000000000000ULL, fileH = 0x8080808080808080ULL, fileA = 0x0101010101010101ULL;
        while (blackPawns > 0) {
            int from = __builtin_ctzll(blackPawns);
            int to = from - 8;
            if (!((1ULL << to) & occupied)) {
                if (to <= 7) { moveSet.add(package_move(from, to, PROMO_KNIGHT)); moveSet.add(package_move(from, to, PROMO_BISHOP)); moveSet.add(package_move(from, to, PROMO_ROOK)); moveSet.add(package_move(from, to, PROMO_QUEEN)); }
                else { moveSet.add(package_move(from, to, QUIET)); if (((1ULL << from) & rank7) && !((1ULL << (from - 16)) & occupied)) moveSet.add(package_move(from, from - 16, DOUBLE_PUSH)); }
            }
            if (!((1ULL << from) & fileA)) { int tl = from - 9; if (((1ULL << tl) & occupied) && !((1ULL << tl) & friendly)) { if (tl <= 7) { moveSet.add(package_move(from, tl, PROMO_KNIGHT_CAP)); moveSet.add(package_move(from, tl, PROMO_BISHOP_CAP)); moveSet.add(package_move(from, tl, PROMO_ROOK_CAP)); moveSet.add(package_move(from, tl, PROMO_QUEEN_CAP)); } else moveSet.add(package_move(from, tl, CAPTURE)); } else if (tl == enPassantSquare) moveSet.add(package_move(from, tl, EP_CAPTURE)); }
            if (!((1ULL << from) & fileH)) { int tr = from - 7; if (((1ULL << tr) & occupied) && !((1ULL << tr) & friendly)) { if (tr <= 7) { moveSet.add(package_move(from, tr, PROMO_KNIGHT_CAP)); moveSet.add(package_move(from, tr, PROMO_BISHOP_CAP)); moveSet.add(package_move(from, tr, PROMO_ROOK_CAP)); moveSet.add(package_move(from, tr, PROMO_QUEEN_CAP)); } else moveSet.add(package_move(from, tr, CAPTURE)); } else if (tr == enPassantSquare) moveSet.add(package_move(from, tr, EP_CAPTURE)); }
            blackPawns &= (blackPawns - 1);
        }
    
}

void Board::generateMoves(bool side, MoveList& list) const {

        uint64_t allWhite = whitePawn | whiteKing | whiteQueen | whiteBishop | whiteKnight | whiteRook;
        uint64_t allBlack = blackPawn | blackKing | blackQueen | blackBishop | blackKnight | blackRook;
        uint64_t occupied = allWhite | allBlack;
        if (side) {
            generateRookMoves(whiteRook, occupied, allWhite, list);
            generateBishopMoves(whiteBishop, occupied, allWhite, list);
            generateQueenMoves(whiteQueen, occupied, allWhite, list);
            generateKnightMoves(whiteKnight, occupied, allWhite, list);
            generateKingMoves(whiteKing, occupied, allWhite, list);
            generateWhitePawnMoves(whitePawn, occupied, allWhite, list);
            generateCastlingMoves(side, occupied, list);
        } else {
            generateRookMoves(blackRook, occupied, allBlack, list);
            generateBishopMoves(blackBishop, occupied, allBlack, list);
            generateQueenMoves(blackQueen, occupied, allBlack, list);
            generateKnightMoves(blackKnight, occupied, allBlack, list);
            generateKingMoves(blackKing, occupied, allBlack, list);
            generateBlackPawnMoves(blackPawn, occupied, allBlack, list);
            generateCastlingMoves(side, occupied, list);
        }
    
}
