# Ply

Ply is a high-performance, UCI-compatible chess engine written in C++. It utilizes advanced bitboard techniques and modern search heuristics to achieve a competitive playing strength of approximately **2185 Elo**.

## Features

### Search Heuristics
*   **Iterative Deepening:** Progressive deepening for better time management and move ordering.
*   **Negamax with Alpha-Beta Pruning:** Optimized minimax search.
*   **Quiescence Search:** Prevents the "horizon effect" by searching tactical captures until stability.
*   **Null Move Pruning (NMP):** Efficiently prunes branches where the opponent has no immediate threats.
*   **Late Move Reduction (LMR):** Searches quiet, late moves at shallower depths to save time.
*   **Aspiration Windows:** Narrows search bounds around previous scores for faster cutoffs.
*   **Check Extensions:** Searches tactical check positions more deeply.
*   **Pruning:** Includes Futility Pruning, Delta Pruning (in Quiescence), and SEE Pruning.
*   **Draw Detection:** Handles three-fold repetition and the 50-move rule correctly.

### Evaluation
*   **Material & PSQT:** Base evaluation using material weights and Piece-Square Tables.
*   **Pawn Structure:** Penalties for doubled and isolated pawns; bonuses for passed pawns.
*   **Rook Activity:** Bonuses for rooks on open files, semi-open files, and the 7th rank.
*   **Bishop Pair:** Positional bonus for possessing both bishops.
*   **King Safety:** Dynamic pawn shield evaluation and king zone attack tracking.
*   **Tapered Eval:** Smooth interpolation between middle-game and endgame values.

### Technical Implementation
*   **Bitboards:** Efficient board representation using 64-bit integers.
*   **Transposition Table (TT):** Stores previously searched positions with Zobrist hashing.
*   **Opening Book:** Supports Polyglot (`.bin`) opening books for diverse and strong early play.
*   **Move Ordering:** Combines TT moves, **Static Exchange Evaluation (SEE)**, Killer Moves, and History Heuristics.

## Building and Compiling

Ply can be compiled using `g++` with high optimization levels.

### Requirements
*   A C++17 compatible compiler (e.g., GCC, Clang, or MSVC).

### Compilation Command
To build the engine, run the following command in the root directory:

```bash
g++ -O3 board.cpp evaluate.cpp movegen.cpp polyglot.cpp uci.cpp search.cpp main.cpp -o engine.exe
```

## Usage

Ply follows the **Universal Chess Interface (UCI)** protocol. You can run it directly from the command line or load it into any chess GUI that supports UCI (such as Arena, Cute Chess, or LucasChess).

### Basic UCI Commands
*   `uci`: Identify the engine and list supported options.
*   `isready`: Sync with the engine.
*   `ucinewgame`: Prepare the engine for a new game.
*   `position startpos moves [moves]`: Set the current board position.
*   `go depth [n]` or `go movetime [ms]`: Start searching for the best move.

## Performance

In testing against established benchmarks (Stockfish 2200), Ply maintains a competitive performance level:
*   **Estimated Strength:** ~2250 Elo
*   **Nodes Per Second (NPS):** ~1.1 - 1.2M (depending on hardware)

## License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

---
**Author:** Arnav Tyagi  
**Date:** April 2026
