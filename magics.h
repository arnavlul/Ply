#ifndef MAGICS_H
#define MAGICS_H

#include "types.h"

namespace Magics {

    extern uint64_t ROOK_MAGICS[64];
    extern uint64_t BISHOP_MAGICS[64];
    extern const int ROOK_SHIFTS[64];
    extern const int BISHOP_SHIFTS[64];
    extern uint64_t ROOK_MASKS[64];
    extern uint64_t BISHOP_MASKS[64];

    extern uint64_t rookTable[64][4096];
    extern uint64_t bishopTable[64][512];

    void init();

}

#endif
