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
            cout << "option name Ponder type check default false" << endl;
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
            myBoard.isPondering = false;
            while (ss >> sub) {
                if (sub == "wtime") ss >> limits.wtime;
                else if (sub == "btime") ss >> limits.btime;
                else if (sub == "winc") ss >> limits.winc;
                else if (sub == "binc") ss >> limits.binc;
                else if (sub == "movestogo") ss >> limits.movestogo;
                else if (sub == "depth") ss >> limits.depth;
                else if (sub == "movetime") ss >> limits.movetime;
                else if (sub == "infinite") limits.infinite = true;
                else if (sub == "ponder") myBoard.isPondering = true;
            }
            myBoard.stopSearch = false;
            searchThread = thread([&myBoard, limits]() {
                uint16_t bestMove = myBoard.search(limits);
                
                // If the search finishes while still pondering, wait for ponderhit or stop
                while (myBoard.isPondering && !myBoard.stopSearch) {
                    this_thread::sleep_for(chrono::milliseconds(10));
                }

                uint16_t ponderMove = myBoard.getPonderMove();
                cout << "bestmove " << myBoard.moveToString(bestMove);
                if (ponderMove) cout << " ponder " << myBoard.moveToString(ponderMove);
                cout << endl;
            });
        } else if (token == "ponderhit") {
            myBoard.isPondering = false;
            myBoard.startTime = chrono::steady_clock::now();
        } else if (token == "stop") {
            stopSearching();
        } else if (token == "quit") {
            stopSearching();
            break;
        }
    }
    stopSearching();
}
