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

        void generateKnightMoves(uint64_t knightBoard, uint64_t friendly, vector<uint16_t> &moveset){

            while(knightBoard > 0){
                int from = __builtin_ctzll(knightBoard);

                uint64_t this_knight = knightMoveMask[from];

                uint64_t attacks = this_knight & ~friendly;

                while(attacks){
                    int to = __builtin_ctzll(attacks);
                    uint16_t move = package_move(from, to);
                    moveset.push_back(move);
                    attacks &= (attacks - 1);
                }

                knightBoard &= (knightBoard - 1);
            }
            
        }

        void generateKingMoves(uint64_t kingBoard, uint64_t friendly, vector<uint16_t> & moveset){
            
            if(kingBoard == 0) return; // Somehow no king

            int from = __builtin_ctzll(kingBoard); // No need for loop like knight, coz only ever one king

            uint64_t attacks = kingMoveMask[from];
            attacks &= ~friendly;

            while(attacks){
                int to = __builtin_ctzll(attacks);
                uint16_t move = package_move(from, to);
                moveset.push_back(move);
                
                attacks &= (attacks - 1);
            }

        }

        void generateWhitePawnMoves(uint64_t whitePawns, uint64_t occupied, uint64_t friendly, vector<uint16_t> &moveSet){

            uint64_t rank2 = 0x000000000000FF00ULL;
            uint64_t fileH = 0x8080808080808080ULL;
            uint64_t fileA = 0x0101010101010101ULL;
            
            while(whitePawns > 0){

                int from = __builtin_ctzll(whitePawns);
                int copy = from;
                
                // Single move forward
                copy += 8;
                if(!((1ULL << copy) & occupied)){
                    uint16_t move = package_move(from, copy);
                    moveSet.push_back(move);

                    if((1ULL << from) & rank2){
                        copy += 8;
                        if(!(1ULL << copy & occupied)){
                            uint16_t move = package_move(from, copy);
                            moveSet.push_back(move);
                        }
                    }
                }

                copy = from;

                if(!((1ULL << copy) & fileA)){ // Forward-left attack
                    copy += 7;
                    if(((1ULL << copy) & occupied) && !((1ULL << copy) & friendly)){
                        uint16_t move = package_move(from, copy);
                        moveSet.push_back(move);
                    }
                }
                
                copy = from;
                if(!((1ULL << copy) & fileH)){ // Forward-right attack
                    copy += 9;
                    if(((1ULL << copy) & occupied) && !((1ULL << copy) & friendly)){
                        uint16_t move = package_move(from, copy);
                        moveSet.push_back(move);
                    }
                }

                whitePawns &= (whitePawns - 1);

            }
        }

        void generateBlackPawnMoves(uint64)

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
                generateKnightMoves(whiteKnight, allWhite, moveSet);
                generateKingMoves(whiteKing, allWhite, moveSet);

            }    
            else{

                generateRookMoves(blackRook, occupied, allBlack, moveSet);
                generateBishopMoves(blackBishop, occupied, allBlack, moveSet);
                generateQueenMoves(blackQueen, occupied, allBlack, moveSet);
                generateKnightMoves(blackKnight, allBlack, moveSet);
                generateKingMoves(blackKing, allBlack, moveSet);

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
    vector<uint16_t> moves = myBoard.generateMoves(1);

    return 0;
}