#include "uci.h"
#include "evaluate.h"
int main() {
    Evaluation::initZobrist();
    Evaluation::initEvaluate();
    Board myBoard;
    myBoard.init();
    uciLoop(myBoard);
    return 0;
}
