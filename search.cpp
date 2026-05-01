#include "board.h"
void Board::clearTT()  {

        for (int i = 0; i < TT_SIZE; i++) tt[i].key = 0;
    
}

void Board::clearSearchState() {
    for (int i = 0; i < 128; i++) killerMoves[i][0] = killerMoves[i][1] = 0;
    for (int s = 0; s < 2; s++) 
        for (int f = 0; f < 64; f++) 
            for (int t = 0; t < 64; t++) 
                historyHeuristic[s][f][t] = 0;
    for (int i = 0; i < 128; i++) pvLength[i] = 0;
}

int Board::scoreToTT(int score, int ply)  {

        if (score > 49000) return score + ply;
        if (score < -49000) return score - ply;
        return score;
    
}

int Board::scoreFromTT(int score, int ply)  {

        if (score > 49000) return score - ply;
        if (score < -49000) return score + ply;
        return score;
    
}

int Board::probeTT(uint64_t key, int depth, int alpha, int beta, int ply, uint16_t& bestMove)  {

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

void Board::storeTT(uint64_t key, int depth, int score, int flag, uint16_t bestMove, int ply)  {

        TTEntry& entry = tt[key % TT_SIZE];
        if (entry.key == key && entry.depth > depth) return;
        
        entry.key = key;
        entry.score = scoreToTT(score, ply);
        entry.depth = depth;
        entry.flag = flag;
        entry.bestMove = bestMove;
    
}

int Board::quiescence(int alpha, int beta)  {

        nodes++;
        if (nodes % 2048 == 0) checkTime();
        if (stopSearch) return 0;

        if (halfmoveClock >= 100) return 0;

        int standPat = evaluate();
        if (standPat >= beta) return beta;
        if (alpha < standPat) alpha = standPat;

        MoveList list;
        generateMoves(sideToMove, list);

        ScoredMove scoredMoves[256];
        int scoredCount = 0;
        for (int i = 0; i < list.count; i++) {
            uint16_t m = list.moves[i];
            if (getFlags(m) & CAPTURE) {
                scoredMoves[scoredCount++] = {m, getMoveScore(m, 0, 0)};
            }
        }

        sort(scoredMoves, scoredMoves + scoredCount, [](const ScoredMove& a, const ScoredMove& b) {
            return a.score > b.score;
        });

        for (int i = 0; i < scoredCount; i++) {
            auto& sm = scoredMoves[i];
            // Delta Pruning
            int victimType = getPieceAt(getTo(sm.move), !sideToMove);
            if (getFlags(sm.move) == EP_CAPTURE) victimType = PAWN;
            
            int victimValue = 0;
            switch(victimType) {
                case PAWN: victimValue = Evaluation::PAWN_VAL; break;
                case KNIGHT: victimValue = Evaluation::KNIGHT_VAL; break;
                case BISHOP: victimValue = Evaluation::BISHOP_VAL; break;
                case ROOK: victimValue = Evaluation::ROOK_VAL; break;
                case QUEEN: victimValue = Evaluation::QUEEN_VAL; break;
                default: break;
            }

            if (standPat + victimValue + 200 < alpha) continue;

            // SEE Pruning
            if (see(sm.move) < 0) continue;

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

int Board::negamax(int depth, int alpha, int beta, int ply, uint16_t& bestMoveOut, uint16_t pvMove)  {

        nodes++;
        if (nodes % 2048 == 0) checkTime();
        if (stopSearch) return 0;

        pvLength[ply] = ply;

        if (ply > 0) {
            if (halfmoveClock >= 100 || isRepetition()) return 0;
        }

        bool isCheck = inCheck(sideToMove);

        int oldAlpha = alpha;
        uint16_t ttMove = 0;
        int ttScore = probeTT(hashKey, depth, alpha, beta, ply, ttMove);
        if (ttScore != -1000001) {
            if (ply == 0) bestMoveOut = ttMove;
            return ttScore;
        }

        if (depth <= 0) return quiescence(alpha, beta);

        int staticEval = evaluate();

        // Razoring
        if (depth <= 3 && !isCheck) {
            int margin = depth * 300;
            if (staticEval + margin < alpha) {
                int qScore = quiescence(alpha, beta);
                if (qScore <= alpha) return qScore;
            }
        }

        // Null Move Pruning
        if (depth >= 3 && !isCheck && ply > 0 && staticEval >= beta) {
            uint64_t majorPieces = sideToMove ? (whiteKnight | whiteBishop | whiteRook | whiteQueen) 
                                              : (blackKnight | blackBishop | blackRook | blackQueen);
            if (majorPieces) {
                UndoInfo undo;
                makeNullMove(undo);
                uint16_t dummy;
                int R = depth >= 6 ? 3 : 2;
                int score = -negamax(depth - 1 - R, -beta, -beta + 1, ply + 1, dummy, 0);
                unmakeNullMove(undo);
                if (score >= beta) return beta;
            }
        }

        // Futility Pruning
        bool futilityPruning = false;
        if (depth == 1 && !isCheck && staticEval + 300 < alpha) {
            futilityPruning = true;
        }

        MoveList list;
        generateMoves(sideToMove, list);

        ScoredMove scoredMoves[256];
        int scoredCount = 0;
        uint16_t bestMoveThisNode = ttMove ? ttMove : pvMove;
        for (int i = 0; i < list.count; i++) {
            scoredMoves[scoredCount++] = {list.moves[i], getMoveScore(list.moves[i], bestMoveThisNode, ply)};
        }
        
        sort(scoredMoves, scoredMoves + scoredCount, [](const ScoredMove& a, const ScoredMove& b) {
            return a.score > b.score;
        });

        int legalMoves = 0;
        int bestScore = -1000000;
        uint16_t localBestMove = 0;
        int quietMovesSeen = 0;

        for (int i = 0; i < scoredCount; i++) {
            auto& sm = scoredMoves[i];
            bool isCapture = getFlags(sm.move) & CAPTURE;
            bool isPromo = getFlags(sm.move) >= PROMO_KNIGHT;

            if (futilityPruning && !isCapture && !isPromo) continue;

            UndoInfo undo;
            if (makeMove(sm.move, undo)) {
                legalMoves++;
                
                // Late Move Pruning (LMP)
                if (!isCapture && !isPromo && !isCheck) {
                    bool givesCheck = inCheck(sideToMove);
                    if (!givesCheck) {
                        quietMovesSeen++;
                        const int lmpCounts[] = {0, 8, 15, 30};
                        if (depth <= 3 && quietMovesSeen >= lmpCounts[depth]) {
                            unmakeMove(sm.move, undo);
                            continue;
                        }
                    }
                }
                
                uint16_t dummy;
                int score;

                // Check Extension (we gave check)
                int ext = (inCheck(sideToMove) && ply < 128) ? 1 : 0;

                // LMR
                if (legalMoves >= 4 && depth >= 3 && !isCapture && !isPromo && ext == 0 && !isCheck) {
                    score = -negamax(depth - 2, -alpha - 1, -alpha, ply + 1, dummy, 0);
                    if (score > alpha) {
                        score = -negamax(depth - 1 + ext, -beta, -alpha, ply + 1, dummy, 0);
                    }
                } else {
                    score = -negamax(depth - 1 + ext, -beta, -alpha, ply + 1, dummy, 0);
                }

                unmakeMove(sm.move, undo);

                if (stopSearch) return 0;

                if (score > bestScore) {
                    bestScore = score;
                    localBestMove = sm.move;
                }
                if (score > alpha) {
                    alpha = score;
                    pvTable[ply][ply] = sm.move;
                    for (int i = ply + 1; i < pvLength[ply + 1]; i++)
                        pvTable[ply][i] = pvTable[ply + 1][i];
                    pvLength[ply] = pvLength[ply + 1];
                }
                if (alpha >= beta) {
                    if (!(getFlags(sm.move) & CAPTURE) && !(getFlags(sm.move) >= PROMO_KNIGHT) && ply < 128) {
                        killerMoves[ply][1] = killerMoves[ply][0];
                        killerMoves[ply][0] = sm.move;
                        
                        int bonus = depth * depth;
                        auto& h = historyHeuristic[sideToMove][getFrom(sm.move)][getTo(sm.move)];
                        h += bonus - h * abs(bonus) / 16384;

                        // History Malus (Penalty for other quiet moves)
                        int penalty = -depth * depth;
                        for (int j = 0; j < i; j++) {
                            auto& prevSm = scoredMoves[j];
                            if (!(getFlags(prevSm.move) & CAPTURE) && !(getFlags(prevSm.move) >= PROMO_KNIGHT)) {
                                auto& hp = historyHeuristic[sideToMove][getFrom(prevSm.move)][getTo(prevSm.move)];
                                hp += penalty - hp * abs(penalty) / 16384;
                            }
                        }
                    }
                    break;
                }
            }
        }

        if (legalMoves == 0) {
            if (isCheck) return -50000 + ply;
            return 0;
        }

        int flag = HASH_EXACT;
        if (bestScore <= oldAlpha) flag = HASH_ALPHA;
        else if (bestScore >= beta) flag = HASH_BETA;
        storeTT(hashKey, depth, bestScore, flag, localBestMove, ply);

        if (ply == 0) bestMoveOut = localBestMove;
        return bestScore;
    
}

uint16_t Board::search(const SearchLimits& limits)  {

        stopSearch = false;
        nodes = 0;
        startTime = chrono::steady_clock::now();
        
        uint64_t polyglotHash = getPolyglotHash();
        uint16_t bookMove = Polyglot::getBookMove(polyglotHash, *this);
        if (bookMove != 0) {
            cout << "info string Found book move (Polyglot)" << endl;
            return bookMove;
        }

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
        MoveList allMoves;
        generateMoves(sideToMove, allMoves);
        for (int i = 0; i < allMoves.count; i++) {
            uint16_t m = allMoves.moves[i];
            UndoInfo undo;
            if (makeMove(m, undo)) {
                unmakeMove(m, undo);
                bestMove = m;
                break;
            }
        }

        int alpha = -1000000;
        int beta = 1000000;
        int prevScore = 0;
        bool aspirationValid = false;

        for (int d = 1; d <= limits.depth; d++) {
            for (int i = 0; i < 128; i++) killerMoves[i][0] = killerMoves[i][1] = 0;

            if (d >= 5 && aspirationValid) {
                alpha = prevScore - 50;
                beta = prevScore + 50;
            } else {
                alpha = -1000000;
                beta = 1000000;
            }

            int score = 0;
            while (true) {
                uint16_t currentBest = 0;
                score = negamax(d, alpha, beta, 0, currentBest, bestMove);
                
                if (stopSearch) {
                    if (currentBest != 0) bestMove = currentBest;
                    break;
                }

                if (score <= alpha) {
                    alpha -= 200;
                    if (alpha < -1000000) alpha = -1000000;
                } else if (score >= beta) {
                    beta += 200;
                    if (beta > 1000000) beta = 1000000;
                } else {
                    prevScore = score;
                    aspirationValid = true;
                    if (currentBest != 0) bestMove = currentBest;
                    break;
                }
            }

            if (stopSearch) break;
            
            auto now = chrono::steady_clock::now();
            auto elapsed = chrono::duration_cast<chrono::milliseconds>(now - startTime).count();
            if (elapsed == 0) elapsed = 1;

            string pvStr = "";
            for (int i = 0; i < pvLength[0]; i++)
                pvStr += moveToString(pvTable[0][i]) + " ";

            string scoreStr;
            if (prevScore > 49000)
                scoreStr = "mate " + to_string((50000 - prevScore + 1) / 2);
            else if (prevScore < -49000)
                scoreStr = "mate -" + to_string((50000 + prevScore + 1) / 2);
            else
                scoreStr = "cp " + to_string(prevScore);

            cout << "info depth " << d << " score " << scoreStr << " nodes " << nodes << " time " << elapsed << " nps " << (nodes * 1000 / elapsed) << " pv " << pvStr << endl;
            
            if (limits.infinite == false && timeLimit != -1 && elapsed > timeLimit / 2) break; 
        }
        return bestMove;
    
}

