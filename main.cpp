#include <iostream>
#include <string>
#include <cstdint>
#include <bit>
#include <vector>
using namespace std;

class Board{

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

    public:

        enum PieceType { NONE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };

        struct UndoInfo {
            uint8_t castlingRights;
            int enPassantSquare;
            int capturedPieceType;
            uint16_t move;
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

        int getPieceAt(int square, bool side) {
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

        void togglePiece(int pieceType, int square, bool side) {
            uint64_t bit = (1ULL << square);
            if (side) {
                switch (pieceType) {
                    case PAWN: whitePawn ^= bit; break;
                    case KNIGHT: whiteKnight ^= bit; break;
                    case BISHOP: whiteBishop ^= bit; break;
                    case ROOK: whiteRook ^= bit; break;
                    case QUEEN: whiteQueen ^= bit; break;
                    case KING: whiteKing ^= bit; break;
                }
            } else {
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
            undo.capturedPieceType = NONE;
            undo.move = move;

            int piece = getPieceAt(from, side);

            // Execute Move: Toggle from square
            togglePiece(piece, from, side);

            // Handle Captures
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

            // Handle Special Moves
            if (flags == K_CASTLE) {
                if (side) { togglePiece(ROOK, 7, 1); togglePiece(ROOK, 5, 1); }
                else { togglePiece(ROOK, 63, 0); togglePiece(ROOK, 61, 0); }
            } else if (flags == Q_CASTLE) {
                if (side) { togglePiece(ROOK, 0, 1); togglePiece(ROOK, 3, 1); }
                else { togglePiece(ROOK, 56, 0); togglePiece(ROOK, 58, 0); }
            }

            // Promotion or Regular Move to destination
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

            // Update Rights and EP
            castlingRights &= (castlingMask[from] & castlingMask[to]);
            enPassantSquare = (flags == DOUBLE_PUSH) ? (side ? from + 8 : from - 8) : -1;

            sideToMove = !sideToMove;

            // Legality Check: Was the king left in check?
            uint64_t kingBB = side ? whiteKing : blackKing;
            if (attackedSquares(!side) & kingBB) {
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

            // Promotion Undo
            if (flags >= PROMO_KNIGHT) {
                togglePiece(piece, to, side); // Remove promoted piece
                togglePiece(PAWN, to, side);  // Add pawn back at 'to'
                piece = PAWN;
            }

            // Move piece back from 'to' to 'from'
            togglePiece(piece, to, side);
            togglePiece(piece, from, side);

            // Restore Captured Piece
            if (flags & CAPTURE) {
                if (flags == EP_CAPTURE) {
                    int epSq = side ? to - 8 : to + 8;
                    togglePiece(PAWN, epSq, !side);
                } else if (undo.capturedPieceType != NONE) {
                    togglePiece(undo.capturedPieceType, to, !side);
                }
            }

            // Reverse Castling Rook
            if (flags == K_CASTLE) {
                if (side) { togglePiece(ROOK, 7, 1); togglePiece(ROOK, 5, 1); }
                else { togglePiece(ROOK, 63, 0); togglePiece(ROOK, 61, 0); }
            } else if (flags == Q_CASTLE) {
                if (side) { togglePiece(ROOK, 0, 1); togglePiece(ROOK, 3, 1); }
                else { togglePiece(ROOK, 56, 0); togglePiece(ROOK, 58, 0); }
            }

            castlingRights = undo.castlingRights;
            enPassantSquare = undo.enPassantSquare;
        }

        uint16_t package_move(int from, int to, int flags = 0){
            return (uint16_t)(from | (to << 6) | (flags << 12));
        }

        void resetBoard(){ // Initialises pieces to default positions

            whitePawn = 0x000000000000FF00ULL;
            whiteKing = 0x0000000000000010ULL;
            whiteQueen = 0x0000000000000008ULL;
            whiteBishop = 0x0000000000000024UL;
            whiteKnight = 0x0000000000000042ULL;
            whiteRook = 0x0000000000000081ULL;

            blackPawn   = 0x00FF000000000000ULL;
            blackKing   = 0x1000000000000000ULL;
            blackQueen  = 0x0800000000000000ULL;
            blackBishop = 0x2400000000000000ULL;
            blackKnight = 0x4200000000000000ULL;
            blackRook   = 0x8100000000000000ULL;

            castlingRights = 15;
            enPassantSquare = -1;
            sideToMove = 1;

        }

        void init(){
            for (int i = 0; i < 64; i++) castlingMask[i] = 15;
            castlingMask[0] = 13; // A1
            castlingMask[7] = 14; // H1
            castlingMask[4] = 12; // E1
            castlingMask[56] = 7; // A8
            castlingMask[63] = 11; // H8
            castlingMask[60] = 3;  // E8

            resetBoard();
            calculateLeapers(); // Knight and King Movement mask calculation            
        }

        void calculateLeapers(){

            const uint64_t notAFile = 0xFEFEFEFEFEFEFEFEULL;
            const uint64_t notHFile = 0x7F7F7F7F7F7F7F7FULL;
            const uint64_t notABFile = 0xFCFCFCFCFCFCFCFCULL;
            const uint64_t notGHFile = 0x3F3F3F3F3F3F3F3FULL;

            for(int square=0; square<64; square++){

                uint64_t startingSquare = 1ULL << square;

                // Calculating king moves

                uint64_t kingMoves = 0;

                kingMoves |= (startingSquare << 8) | (startingSquare >> 8); // N, S
                kingMoves |= ((startingSquare & notHFile) << 1) | ((startingSquare & notAFile) >> 1); // E, W
                kingMoves |= ((startingSquare & notHFile) << 9) | ((startingSquare & notAFile) << 7); // NE, NW
                kingMoves |= ((startingSquare & notHFile) >> 7) | ((startingSquare & notAFile) >> 9); // SE, SW
                
                kingMoveMask[square] = kingMoves;

                uint64_t knightMoves = 0;

                knightMoves |= ((startingSquare & notHFile) << 17) | ((startingSquare & notGHFile) << 10) | ((startingSquare & notGHFile) >> 6) | ((startingSquare & notHFile) >> 15);
                knightMoves |= ((startingSquare & notAFile) << 15) | ((startingSquare & notABFile) << 6) | ((startingSquare & notABFile) >> 10) | ((startingSquare & notAFile) >> 17);

                knightMoveMask[square] = knightMoves;
            }
        }

        uint64_t getRookMoves(int square, uint64_t occupancy) const {
            uint64_t attacks = 0;
            uint64_t fileA = 0x0101010101010101ULL;
            uint64_t fileH = 0x8080808080808080ULL;
            uint64_t rank1 = 0x00000000000000FFULL;
            uint64_t rank8 = 0xFF00000000000000ULL;

            int copy = square;
            // North
            while (!((1ULL << copy) & rank8)) {
                copy += 8;
                attacks |= (1ULL << copy);
                if ((1ULL << copy) & occupancy) break;
            }
            // South
            copy = square;
            while (!((1ULL << copy) & rank1)) {
                copy -= 8;
                attacks |= (1ULL << copy);
                if ((1ULL << copy) & occupancy) break;
            }
            // East
            copy = square;
            while (!((1ULL << copy) & fileH)) {
                copy += 1;
                attacks |= (1ULL << copy);
                if ((1ULL << copy) & occupancy) break;
            }
            // West
            copy = square;
            while (!((1ULL << copy) & fileA)) {
                copy -= 1;
                attacks |= (1ULL << copy);
                if ((1ULL << copy) & occupancy) break;
            }
            return attacks;
        }

        uint64_t getBishopAttacks(int square, uint64_t occupancy) const {
            uint64_t attacks = 0;
            uint64_t fileA = 0x0101010101010101ULL;
            uint64_t fileH = 0x8080808080808080ULL;
            uint64_t rank1 = 0x00000000000000FFULL;
            uint64_t rank8 = 0xFF00000000000000ULL;

            int copy = square;
            // North-East
            while (!((1ULL << copy) & rank8) && !((1ULL << copy) & fileH)) {
                copy += 9;
                attacks |= (1ULL << copy);
                if ((1ULL << copy) & occupancy) break;
            }
            // North-West
            copy = square;
            while (!((1ULL << copy) & rank8) && !((1ULL << copy) & fileA)) {
                copy += 7;
                attacks |= (1ULL << copy);
                if ((1ULL << copy) & occupancy) break;
            }
            // South-East
            copy = square;
            while (!((1ULL << copy) & rank1) && !((1ULL << copy) & fileH)) {
                copy -= 7;
                attacks |= (1ULL << copy);
                if ((1ULL << copy) & occupancy) break;
            }
            // South-West
            copy = square;
            while (!((1ULL << copy) & rank1) && !((1ULL << copy) & fileA)) {
                copy -= 9;
                attacks |= (1ULL << copy);
                if ((1ULL << copy) & occupancy) break;
            }
            return attacks;
        }

        uint64_t attackedSquares(bool side) const {
            uint64_t attacks = 0;
            uint64_t occupancy = (whitePawn | whiteKing | whiteQueen | whiteBishop | whiteKnight | whiteRook) |
                                 (blackPawn | blackKing | blackQueen | blackBishop | blackKnight | blackRook);

            const uint64_t notAFile = 0xFEFEFEFEFEFEFEFEULL;
            const uint64_t notHFile = 0x7F7F7F7F7F7F7F7FULL;

            if (side) { // White
                // Pawn attacks
                attacks |= (whitePawn << 7) & notHFile;
                attacks |= (whitePawn << 9) & notAFile;
                
                // Knight attacks
                uint64_t knights = whiteKnight;
                while (knights) {
                    int sq = __builtin_ctzll(knights);
                    attacks |= knightMoveMask[sq];
                    knights &= (knights - 1);
                }
                
                // Bishop and Queen (diagonal) attacks
                uint64_t diagSliders = whiteBishop | whiteQueen;
                while (diagSliders) {
                    int sq = __builtin_ctzll(diagSliders);
                    attacks |= getBishopAttacks(sq, occupancy);
                    diagSliders &= (diagSliders - 1);
                }
                
                // Rook and Queen (orthogonal) attacks
                uint64_t orthSliders = whiteRook | whiteQueen;
                while (orthSliders) {
                    int sq = __builtin_ctzll(orthSliders);
                    attacks |= getRookMoves(sq, occupancy);
                    orthSliders &= (orthSliders - 1);
                }
                
                // King attacks
                if (whiteKing) attacks |= kingMoveMask[__builtin_ctzll(whiteKing)];
            } else { // Black
                // Pawn attacks
                attacks |= (blackPawn >> 7) & notAFile;
                attacks |= (blackPawn >> 9) & notHFile;
                
                // Knight attacks
                uint64_t knights = blackKnight;
                while (knights) {
                    int sq = __builtin_ctzll(knights);
                    attacks |= knightMoveMask[sq];
                    knights &= (knights - 1);
                }
                
                // Bishop and Queen (diagonal) attacks
                uint64_t diagSliders = blackBishop | blackQueen;
                while (diagSliders) {
                    int sq = __builtin_ctzll(diagSliders);
                    attacks |= getBishopAttacks(sq, occupancy);
                    diagSliders &= (diagSliders - 1);
                }
                
                // Rook and Queen (orthogonal) attacks
                uint64_t orthSliders = blackRook | blackQueen;
                while (orthSliders) {
                    int sq = __builtin_ctzll(orthSliders);
                    attacks |= getRookMoves(sq, occupancy);
                    orthSliders &= (orthSliders - 1);
                }
                
                // King attacks
                if (blackKing) attacks |= kingMoveMask[__builtin_ctzll(blackKing)];
            }
            return attacks;
        }

        void generateRookMoves(uint64_t rookBoard, const uint64_t occupied, const uint64_t friendly, vector<uint16_t>& moveset){

            uint64_t rank8 = 0xFF00000000000000ULL;
            uint64_t rank1 = 0x00000000000000FFULL;
            uint64_t fileH = 0x8080808080808080ULL;
            uint64_t fileA = 0x0101010101010101ULL;

            while(rookBoard > 0){
                int from = __builtin_ctzll(rookBoard);
                int copy = from;
                while(!((1ULL << copy) & rank8)){
                    copy += 8;
                    if((1ULL << copy) & friendly) break;
                    int flag = ((1ULL << copy) & occupied) ? CAPTURE : QUIET;
                    uint16_t move = package_move(from, copy, flag);
                    moveset.push_back(move);
                    if((1ULL << copy) & occupied) break;
                }
               
                copy = from;
                while(!((1ULL << copy) & rank1)){
                    copy -= 8;
                    if((1ULL << copy) & friendly) break;
                    int flag = ((1ULL << copy) & occupied) ? CAPTURE : QUIET;
                    uint16_t move = package_move(from, copy, flag);
                    moveset.push_back(move);
                    if((1ULL << copy) & occupied) break;
                }

                copy = from;
                while(!((1ULL << copy) & fileH)){
                    copy += 1;
                    if((1ULL << copy) & friendly) break;
                    int flag = ((1ULL << copy) & occupied) ? CAPTURE : QUIET;
                    uint16_t move = package_move(from, copy, flag);
                    moveset.push_back(move);
                    if((1ULL << copy) & occupied) break;
                }

                copy = from;
                while(!((1ULL << copy) & fileA)){
                    copy -= 1;
                    if((1ULL << copy) & friendly) break;
                    int flag = ((1ULL << copy) & occupied) ? CAPTURE : QUIET;
                    uint16_t move = package_move(from, copy, flag);
                    moveset.push_back(move);
                    if((1ULL << copy) & occupied) break;
                }
                rookBoard &= (rookBoard - 1);
            }
            
        }

        void generateBishopMoves(uint64_t bishopBoard, uint64_t occupied, uint64_t friendly, vector<uint16_t> &moveset){

            uint64_t rank8 = 0xFF00000000000000ULL;
            uint64_t rank1 = 0x00000000000000FFULL;
            uint64_t fileH = 0x8080808080808080ULL;
            uint64_t fileA = 0x0101010101010101ULL;

            while(bishopBoard > 0){
                int from = __builtin_ctzll(bishopBoard);
                int copy = from;

                while(!((1ULL << copy) & rank8) && !((1ULL << copy) & fileH)){ // North-East
                    copy += 9;
                    if((1ULL << copy) & friendly) break;
                    int flag = ((1ULL << copy) & occupied) ? CAPTURE : QUIET;
                    uint16_t move = package_move(from, copy, flag);
                    moveset.push_back(move);
                    if((1ULL << copy) & occupied) break;
                }

                copy = from;
                while(!((1ULL << copy) & fileA) && !((1ULL << copy) & rank8)){ // North-West
                    copy += 7;
                    if((1ULL << copy) & friendly) break;
                    int flag = ((1ULL << copy) & occupied) ? CAPTURE : QUIET;
                    uint16_t move = package_move(from, copy, flag);
                    moveset.push_back(move);
                    if((1ULL << copy) & occupied) break;
                }

                copy = from;
                while(!((1ULL << copy) & fileH) && !((1ULL << copy) & rank1)){ // South-East
                    copy -=7;
                    if((1ULL << copy) & friendly) break;
                    int flag = ((1ULL << copy) & occupied) ? CAPTURE : QUIET;
                    uint16_t move = package_move(from, copy, flag);
                    moveset.push_back(move);
                    if((1ULL << copy) & occupied) break;
                }   

                 copy = from;
                while(!((1ULL << copy) & fileA) && !((1ULL << copy) & rank1)){ // South-West
                    copy -= 9;
                    if((1ULL << copy) & friendly) break;
                    int flag = ((1ULL << copy) & occupied) ? CAPTURE : QUIET;
                    uint16_t move = package_move(from, copy, flag);
                    moveset.push_back(move);
                    if((1ULL << copy) & occupied) break;
                }  

                bishopBoard &= (bishopBoard - 1);
            }
        }

        void generateQueenMoves(uint64_t queenBoard, uint64_t occupied, uint64_t friendly, vector<uint16_t> &moveset){

            generateRookMoves(queenBoard, occupied, friendly, moveset);
            generateBishopMoves(queenBoard, occupied, friendly, moveset);

        }

        void generateKnightMoves(uint64_t knightBoard, uint64_t occupied, uint64_t friendly, vector<uint16_t> &moveset){

            while(knightBoard > 0){
                int from = __builtin_ctzll(knightBoard);

                uint64_t this_knight = knightMoveMask[from];

                uint64_t attacks = this_knight & ~friendly;

                while(attacks){
                    int to = __builtin_ctzll(attacks);
                    int flag = ((1ULL << to) & occupied) ? CAPTURE : QUIET;
                    uint16_t move = package_move(from, to, flag);
                    moveset.push_back(move);
                    attacks &= (attacks - 1);
                }

                knightBoard &= (knightBoard - 1);
            }
            
        }

        void generateKingMoves(uint64_t kingBoard, uint64_t occupied, uint64_t friendly, vector<uint16_t> & moveset){
            
            if(kingBoard == 0) return; // Somehow no king

            int from = __builtin_ctzll(kingBoard); // No need for loop like knight, coz only ever one king

            uint64_t attacks = kingMoveMask[from];
            attacks &= ~friendly;

            while(attacks){
                int to = __builtin_ctzll(attacks);
                int flag = ((1ULL << to) & occupied) ? CAPTURE : QUIET;
                uint16_t move = package_move(from, to, flag);
                moveset.push_back(move);
                
                attacks &= (attacks - 1);
            }

        }

        void generateCastlingMoves(bool side, uint64_t occupied, vector<uint16_t>& moveSet) {
            uint64_t attacked = attackedSquares(!side);
            if (side) { // White
                // WK: Path F1 (5), G1 (6) must be empty. Squares 4, 5, 6 must NOT be attacked.
                if ((castlingRights & WK) && !(occupied & ((1ULL << 5) | (1ULL << 6)))) {
                    if (!(attacked & ((1ULL << 4) | (1ULL << 5) | (1ULL << 6)))) {
                        moveSet.push_back(package_move(4, 6, K_CASTLE));
                    }
                }
                // WQ: Path D1 (3), C1 (2), B1 (1) must be empty. Squares 4, 3, 2 must NOT be attacked.
                if ((castlingRights & WQ) && !(occupied & ((1ULL << 3) | (1ULL << 2) | (1ULL << 1)))) {
                    if (!(attacked & ((1ULL << 4) | (1ULL << 3) | (1ULL << 2)))) {
                        moveSet.push_back(package_move(4, 2, Q_CASTLE));
                    }
                }
            } else { // Black
                // BK: Path F8 (61), G8 (62) must be empty. Squares 60, 61, 62 must NOT be attacked.
                if ((castlingRights & BK) && !(occupied & ((1ULL << 61) | (1ULL << 62)))) {
                    if (!(attacked & ((1ULL << 60) | (1ULL << 61) | (1ULL << 62)))) {
                        moveSet.push_back(package_move(60, 62, K_CASTLE));
                    }
                }
                // BQ: Path D8 (59), C8 (58), B8 (57) must be empty. Squares 60, 59, 58 must NOT be attacked.
                if ((castlingRights & BQ) && !(occupied & ((1ULL << 59) | (1ULL << 58) | (1ULL << 57)))) {
                    if (!(attacked & ((1ULL << 60) | (1ULL << 59) | (1ULL << 58)))) {
                        moveSet.push_back(package_move(60, 58, Q_CASTLE));
                    }
                }
            }
        }

        void generateWhitePawnMoves(uint64_t whitePawns, uint64_t occupied, uint64_t friendly, vector<uint16_t> &moveSet){

            uint64_t rank2 = 0x000000000000FF00ULL;
            uint64_t fileH = 0x8080808080808080ULL;
            uint64_t fileA = 0x0101010101010101ULL;
            
            while(whitePawns > 0){

                int from = __builtin_ctzll(whitePawns);
                
                // Single move forward
                int to = from + 8;
                if(!((1ULL << to) & occupied)){
                    if(to >= 56){ // White Promotion
                        moveSet.push_back(package_move(from, to, PROMO_KNIGHT));
                        moveSet.push_back(package_move(from, to, PROMO_BISHOP));
                        moveSet.push_back(package_move(from, to, PROMO_ROOK));
                        moveSet.push_back(package_move(from, to, PROMO_QUEEN));
                    } else {
                        moveSet.push_back(package_move(from, to, QUIET));
                        // Double move forward
                        if(((1ULL << from) & rank2)){
                            int doubleTo = from + 16;
                            if(!((1ULL << doubleTo) & occupied)){
                                moveSet.push_back(package_move(from, doubleTo, DOUBLE_PUSH));
                            }
                        }
                    }
                }

                // Capture Left
                if(!((1ULL << from) & fileA)){
                    int toLeft = from + 7;
                    if(((1ULL << toLeft) & occupied) && !((1ULL << toLeft) & friendly)){
                        if(toLeft >= 56){ // Capture Promotion
                            moveSet.push_back(package_move(from, toLeft, PROMO_KNIGHT_CAP));
                            moveSet.push_back(package_move(from, toLeft, PROMO_BISHOP_CAP));
                            moveSet.push_back(package_move(from, toLeft, PROMO_ROOK_CAP));
                            moveSet.push_back(package_move(from, toLeft, PROMO_QUEEN_CAP));
                        } else {
                            moveSet.push_back(package_move(from, toLeft, CAPTURE));
                        }
                    } else if(toLeft == this->enPassantSquare){
                        moveSet.push_back(package_move(from, toLeft, EP_CAPTURE));
                    }
                }
                
                // Capture Right
                if(!((1ULL << from) & fileH)){
                    int toRight = from + 9;
                    if(((1ULL << toRight) & occupied) && !((1ULL << toRight) & friendly)){
                        if(toRight >= 56){ // Capture Promotion
                            moveSet.push_back(package_move(from, toRight, PROMO_KNIGHT_CAP));
                            moveSet.push_back(package_move(from, toRight, PROMO_BISHOP_CAP));
                            moveSet.push_back(package_move(from, toRight, PROMO_ROOK_CAP));
                            moveSet.push_back(package_move(from, toRight, PROMO_QUEEN_CAP));
                        } else {
                            moveSet.push_back(package_move(from, toRight, CAPTURE));
                        }
                    } else if(toRight == this->enPassantSquare){
                        moveSet.push_back(package_move(from, toRight, EP_CAPTURE));
                    }
                }

                whitePawns &= (whitePawns - 1);

            }
        }

        void generateBlackPawnMoves(uint64_t blackPawns, uint64_t occupied, uint64_t friendly, vector<uint16_t> &moveSet){

            uint64_t rank7 = 0x00FF000000000000ULL;
            uint64_t fileH = 0x8080808080808080ULL;
            uint64_t fileA = 0x0101010101010101ULL;
            
            while(blackPawns > 0){

                int from = __builtin_ctzll(blackPawns);
                
                // Single move forward
                int to = from - 8;
                if(!((1ULL << to) & occupied)){
                    if(to <= 7){ // Black Promotion
                        moveSet.push_back(package_move(from, to, PROMO_KNIGHT));
                        moveSet.push_back(package_move(from, to, PROMO_BISHOP));
                        moveSet.push_back(package_move(from, to, PROMO_ROOK));
                        moveSet.push_back(package_move(from, to, PROMO_QUEEN));
                    } else {
                        moveSet.push_back(package_move(from, to, QUIET));
                        // Double move forward
                        if(((1ULL << from) & rank7)){
                            int doubleTo = from - 16;
                            if(!((1ULL << doubleTo) & occupied)){
                                moveSet.push_back(package_move(from, doubleTo, DOUBLE_PUSH));
                            }
                        }
                    }
                }

                // Capture Left
                if(!((1ULL << from) & fileA)){
                    int toLeft = from - 9;
                    if(((1ULL << toLeft) & occupied) && !((1ULL << toLeft) & friendly)){
                        if(toLeft <= 7){ // Capture Promotion
                            moveSet.push_back(package_move(from, toLeft, PROMO_KNIGHT_CAP));
                            moveSet.push_back(package_move(from, toLeft, PROMO_BISHOP_CAP));
                            moveSet.push_back(package_move(from, toLeft, PROMO_ROOK_CAP));
                            moveSet.push_back(package_move(from, toLeft, PROMO_QUEEN_CAP));
                        } else {
                            moveSet.push_back(package_move(from, toLeft, CAPTURE));
                        }
                    } else if(toLeft == this->enPassantSquare){
                        moveSet.push_back(package_move(from, toLeft, EP_CAPTURE));
                    }
                }
                
                // Capture Right
                if(!((1ULL << from) & fileH)){
                    int toRight = from - 7;
                    if(((1ULL << toRight) & occupied) && !((1ULL << toRight) & friendly)){
                        if(toRight <= 7){ // Capture Promotion
                            moveSet.push_back(package_move(from, toRight, PROMO_KNIGHT_CAP));
                            moveSet.push_back(package_move(from, toRight, PROMO_BISHOP_CAP));
                            moveSet.push_back(package_move(from, toRight, PROMO_ROOK_CAP));
                            moveSet.push_back(package_move(from, toRight, PROMO_QUEEN_CAP));
                        } else {
                            moveSet.push_back(package_move(from, toRight, CAPTURE));
                        }
                    } else if(toRight == this->enPassantSquare){
                        moveSet.push_back(package_move(from, toRight, EP_CAPTURE));
                    }
                }

                blackPawns &= (blackPawns - 1);

            }
        }

        vector<uint16_t> generateMoves(bool side){ // 1 for white, 0 for black

            uint64_t allWhite = whitePawn | whiteKing | whiteQueen | whiteBishop | whiteKnight | whiteRook;
            uint64_t allBlack = blackPawn | blackKing | blackQueen | blackBishop | blackKnight | blackRook;

            uint64_t occupied = allWhite | allBlack;
            uint64_t unoccupied = ~occupied;

            
            vector<uint16_t> moveSet;

            if(side){ // White

                generateRookMoves(whiteRook, occupied, allWhite, moveSet);
                generateBishopMoves(whiteBishop, occupied, allWhite, moveSet);
                generateQueenMoves(whiteQueen, occupied, allWhite, moveSet);
                generateKnightMoves(whiteKnight, occupied, allWhite, moveSet);
                generateKingMoves(whiteKing, occupied, allWhite, moveSet);
                generateWhitePawnMoves(whitePawn, occupied, allWhite, moveSet);
                generateCastlingMoves(side, occupied, moveSet);

            }    
            else{

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

        void printBitBoard(uint64_t bitboard){

            for(int rank=7; rank >=0; rank--){
                cout << rank + 1;

                for(int file=0; file <=7; file++){
                    
                    int square = rank * 8 + file;

                    if(bitboard & (1ULL << square)) cout << " 1 ";
                    else cout << " . ";
                }
                cout << "\n";
            }
            cout << "  A  B  C  D  E  F  G  H";
        }

        string squareToCoord(int sq) {
            int file = sq % 8;
            int rank = sq / 8;
            string s = "";
            s += (char)('a' + file);
            s += (char)('1' + rank);
            return s;
        }

        string moveToString(uint16_t move) {
            int from = getFrom(move);
            int to = getTo(move);
            int flags = getFlags(move);
            string s = squareToCoord(from) + squareToCoord(to);
            if (flags >= PROMO_KNIGHT) {
                if ((flags & 3) == 0) s += 'n';
                else if ((flags & 3) == 1) s += 'b';
                else if ((flags & 3) == 2) s += 'r';
                else if ((flags & 3) == 3) s += 'q';
            }
            return s;
        }

        uint64_t perft(int depth) {
            if (depth == 0) return 1ULL;
            uint64_t nodes = 0;
            vector<uint16_t> moves = generateMoves(sideToMove);
            for (uint16_t move : moves) {
                UndoInfo undo;
                if (makeMove(move, undo)) {
                    nodes += perft(depth - 1);
                    unmakeMove(move, undo);
                }
            }
            return nodes;
        }

        void perftDivide(int depth) {
            uint64_t totalNodes = 0;
            vector<uint16_t> moves = generateMoves(sideToMove);
            for (uint16_t move : moves) {
                UndoInfo undo;
                if (makeMove(move, undo)) {
                    uint64_t nodes = perft(depth - 1);
                    unmakeMove(move, undo);
                    cout << moveToString(move) << ": " << nodes << endl;
                    totalNodes += nodes;
                }
            }
            cout << "\nTotal nodes at depth " << depth << ": " << totalNodes << endl;
        }

};

void uciLoop(Board& myBoard){
    string command;
    while(getline(cin, command)){

        if(command == "uci"){
            cout << "id name ChessEngine" << endl;
            cout << "id author 2905" << endl;
            cout << "uciok" << endl;
        }
        else if(command == "isready"){
            cout << "readyok" << endl;
        }
        else if(command == "ucinewgame"){
            // Reset board to start position
        }
        else if(command.substr(0,8) == "position"){
            if(command.find("startpos") != string::npos){
                myBoard.init();
            }
        }
        else if(command.substr(0, 5) == "perft") {
            int depth = stoi(command.substr(6));
            myBoard.perftDivide(depth);
        }
        else if(command.substr(0,2) == "go"){
            // Calculate best move
            cout << "bestmove e2e4" << endl; // Placeholder
        }
        else if(command == "quit"){
            break;
        }
    }
}


int main(){

    Board myBoard;
    myBoard.init();
    uciLoop(myBoard);

    return 0;
}