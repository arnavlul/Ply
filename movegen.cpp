#include "board.h"
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

uint64_t Board::getBishopAttacks(int square, uint64_t occupancy) const {

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

void Board::generateRookMoves(uint64_t rookBoard, uint64_t occupied, uint64_t friendly, vector<uint16_t>& moveset) const {

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

void Board::generateBishopMoves(uint64_t bishopBoard, uint64_t occupied, uint64_t friendly, vector<uint16_t>& moveset) const {

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

void Board::generateQueenMoves(uint64_t queenBoard, uint64_t occupied, uint64_t friendly, vector<uint16_t>& moveset) const {

        generateRookMoves(queenBoard, occupied, friendly, moveset);
        generateBishopMoves(queenBoard, occupied, friendly, moveset);
    
}

void Board::generateKnightMoves(uint64_t knightBoard, uint64_t occupied, uint64_t friendly, vector<uint16_t>& moveset) const {

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

void Board::generateKingMoves(uint64_t kingBoard, uint64_t occupied, uint64_t friendly, vector<uint16_t>& moveset) const {

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

void Board::generateCastlingMoves(bool side, uint64_t occupied, vector<uint16_t>& moveSet) const {

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

void Board::generateWhitePawnMoves(uint64_t whitePawns, uint64_t occupied, uint64_t friendly, vector<uint16_t>& moveSet) const {

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

void Board::generateBlackPawnMoves(uint64_t blackPawns, uint64_t occupied, uint64_t friendly, vector<uint16_t>& moveSet) const {

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

vector<uint16_t> Board::generateMoves(bool side) const {

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

