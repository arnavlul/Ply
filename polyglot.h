#ifndef POLYGLOT_H
#define POLYGLOT_H
#include "types.h"
class Board;
namespace Polyglot {
#pragma pack(push, 1)
    struct Entry {
        uint64_t key;
        uint16_t move;
        uint16_t weight;
        uint32_t learn;
    };
#pragma pack(pop)

    extern const uint64_t RANDOM_ARRAY[781];
    uint16_t getBookMove(uint64_t key, const Board& board);
    inline uint64_t swap64(uint64_t v) { return ((v & 0xFFULL) << 56) | ((v & 0xFF00ULL) << 40) | ((v & 0xFF0000ULL) << 24) | ((v & 0xFF000000ULL) << 8) | ((v >> 8) & 0xFF000000ULL) | ((v >> 24) & 0xFF0000ULL) | ((v >> 40) & 0xFF00ULL) | ((v >> 56) & 0xFFULL); }
    inline uint16_t swap16(uint16_t v) { return (v << 8) | (v >> 8); }
}
#endif
