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

        uint16_t package_move(int from, int to){

            uint16_t moveRep = 0;
            moveRep |= from;
            moveRep |= (to << 6);
            return moveRep;

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
                    uint64_t move = package_move(from, copy);
                    moveset.push_back(move);
                    if((1ULL << copy) & occupied) break;
                }
               
                copy = from;
                while(!((1ULL << copy) & rank1)){
                    copy -= 8;
                    if((1ULL << copy) & friendly) break;
                    uint64_t move = package_move(from, copy);
                    moveset.push_back(move);
                    if((1ULL << copy) & occupied) break;
                }

                copy = from;
                while(!((1ULL << copy) & fileH)){
                    copy += 1;
                    if((1ULL << copy) & friendly) break;
                    uint64_t move = package_move(from, copy);
                    moveset.push_back(move);
                    if((1ULL << copy) & occupied) break;
                }

                copy = from;
                while(!((1ULL << copy) & fileA)){
                    copy -= 1;
                    if((1ULL << copy) & friendly) break;
                    uint64_t move = package_move(from, copy);
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
                    uint16_t move = package_move(from, copy);
                    moveset.push_back(move);
                    if((1ULL << copy) & occupied) break;
                }

                copy = from;
                while(!((1ULL << copy) & fileA) && !((1ULL << copy) & rank8)){ // North-West
                    copy += 7;
                    if((1ULL << copy) & friendly) break;
                    uint16_t move = package_move(from, copy);
                    moveset.push_back(move);
                    if((1ULL << copy) & occupied) break;
                }

                copy = from;
                while(!((1ULL << copy) & fileH) && !((1ULL << copy) & rank1)){ // South-East
                    copy -=7;
                    if((1ULL << copy) & friendly) break;
                    uint16_t move = package_move(from, copy);
                    moveset.push_back(move);
                    if((1ULL << copy) & occupied) break;
                }   

                 copy = from;
                while(!((1ULL << copy) & fileA) && !((1ULL << copy) & rank1)){ // South-West
                    copy -= 9;
                    if((1ULL << copy) & friendly) break;
                    uint16_t move = package_move(from, copy);
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

        void generateKnightAttackMask(uint64_t knightBoard, uint64_t friendly, vector<uint16_t> &moveset){

            

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