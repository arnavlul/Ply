#include <iostream>
#include <string>
#include <cstdint>
#include <bit>
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

    public:

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

        }

        void init(){
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
                kingMoves |= ((startingSquare & notHFile) >> 7) | ((startingSquare & notAFile) >> 9); // SE, S
                
                kingMoveMask[square] = kingMoves;

                uint64_t knightMoves = 0;

                knightMoves |= ((startingSquare & notHFile) << 17) | ((startingSquare & notGHFile) << 10) | ((startingSquare & notGHFile) >> 6) | ((startingSquare & notHFile) >> 15);
                knightMoves |= ((startingSquare & notAFile) << 15) | ((startingSquare & notABFile) << 6) | ((startingSquare & notABFile) >> 10) | ((startingSquare & notAFile) >> 17);

                knightMoveMask[square] = knightMoves;
            }
        }

        uint64_t generateRookAttackMask(uint64_t rookBoard, uint64_t occupied){

            uint64_t rank8 = 0xFF00000000000000ULL;
            uint64_t rank1 = 0x00000000000000FFULL;
            uint64_t fileH = 0x8080808080808080ULL;
            uint64_t fileA = 0x0101010101010101ULL;

            uint64_t rookAttackMask = 0;
            while(rookBoard > 0){
                int index = __builtin_ctzll(rookBoard);
                uint64_t pos = (1ULL << index);

                while(!(pos & rank8)){ // North      
                    pos <<= 8;                    
                    rookAttackMask |= pos;
                    if(pos & occupied) break;
                }
                pos = (1ULL << index);
                while(!(pos & rank1)){ // South
                    pos >>= 8;
                    rookAttackMask |= pos;
                    if(pos & occupied) break;
                }
                pos = (1ULL << index);
                while(!(pos & fileH)){ // East
                    pos <<= 1;
                    rookAttackMask |= pos;
                    if(pos & occupied) break;
                }
                pos = (1ULL << index);
                while(!(pos & fileA)){ // West
                    pos >>=  1;
                    rookAttackMask |= pos;
                    if(pos & occupied) break;
                }
                rookBoard &= (rookBoard-1);
            }
            
            return rookAttackMask;        
        }

        uint64_t generateBishopAttackMask(uint64_t bishopBoard, uint64_t occupied){

            uint64_t rank8 = 0xFF00000000000000ULL;
            uint64_t rank1 = 0x00000000000000FFULL;
            uint64_t fileH = 0x8080808080808080ULL;
            uint64_t fileA = 0x0101010101010101ULL;

            uint64_t bishopAttackMask = 0;
            while(bishopBoard > 0){
                int index = __builtin_ctzll(bishopBoard);
                uint64_t pos = (1ULL << index);

                while(!(pos & rank8) && !(pos & fileH)){ // North-East
                    pos <<= 9;
                    bishopAttackMask |= pos;
                    if(pos & occupied) break;
                }

                pos = (1ULL << index);
                while(!(pos & fileA) && !(pos & rank8)){ // North-West
                    pos <<= 7;
                    bishopAttackMask |= pos;
                    if(pos & occupied) break;
                }

                pos = (1ULL << index);
                while(!(pos & fileH) && !(pos & rank1)){ // South-East
                    pos >>= 7;
                    bishopAttackMask |= pos;
                    if(pos & occupied) break;
                }   

                 pos = (1ULL << index);
                while(!(pos & fileA) && !(pos & rank1)){ // South-West
                    pos >>= 9;
                    bishopAttackMask |= pos;
                    if(pos & occupied) break;
                }  

                bishopBoard &= (bishopBoard - 1);
            }
            
            return bishopAttackMask;
        }

        uint64_t generateKnightAttackMask(uint64_t knightBoard){

            uint64_t knightAttackMask = 0;
            while(knightBoard > 0){
                int index = __builtin_ctzll(knightBoard);

                knightAttackMask |= knightMoveMask[index];
                knightBoard &= (knightBoard - 1);
            }
            
            return knightAttackMask;
        }

        uint64_t generateKingAttackMask(uint64_t kingBoard){
            
            uint64_t kingAttackMask = 0;
            while(kingBoard > 0){
                int index = __builtin_ctzll(kingBoard);

                kingAttackMask |= kingMoveMask[index];
                kingBoard &= (kingBoard - 1);
            }

            return kingAttackMask;
        }

        void generateMoves(bool side){ // 1 for white, 0 for black

            uint64_t allWhite = whitePawn | whiteKing | whiteQueen | whiteBishop | whiteKnight | whiteRook;
            uint64_t allBlack = blackPawn | blackKing | blackQueen | blackBishop | blackKnight | blackRook;

            uint64_t occupied = allWhite | allBlack;
            uint64_t unoccupied = ~occupied;

            
            uint64_t rookAttackMask;
            uint64_t bishopAttackMask;
            uint64_t queenAttackMask;
            uint64_t knightAttackMask;
            uint64_t kingAttackMask;
            uint64_t pawnAttackMask;

            if(side){

                rookAttackMask = generateRookAttackMask(whiteRook, occupied);
                rookAttackMask &= ~allWhite;

                bishopAttackMask = generateBishopAttackMask(whiteBishop, occupied);
                bishopAttackMask &= ~allWhite;

                uint64_t queenAttackMask1 = generateRookAttackMask(whiteQueen, occupied);
                uint64_t queenAttackMask2 = generateBishopAttackMask(whiteQueen, occupied);
                queenAttackMask = (queenAttackMask1 | queenAttackMask2) & (~allWhite);

                knightAttackMask = generateKnightAttackMask(whiteKnight);
                knightAttackMask &= ~allWhite;

                kingAttackMask = generateKingAttackMask(whiteKing);
                kingAttackMask &= ~allWhite;
            }    

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
    
};

void uciLoop(Board myBoard){

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
    // uciLoop(myBoard);
    myBoard.init();
    myBoard.generateMoves();

    return 0;
}