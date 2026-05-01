#include "uci.h"
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
            cout << "id name Ply" << endl;
            cout << "id author Arnav" << endl;
            cout << "uciok" << endl;
        } else if (token == "isready") {
            cout << "readyok" << endl;
        } else if (token == "ucinewgame") {
            stopSearching();
            myBoard.clearTT();
            myBoard.clearSearchState();
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
