#include "magics.h"
#include <random>

namespace Magics {

    uint64_t ROOK_MASKS[64];
    uint64_t BISHOP_MASKS[64];
    uint64_t rookTable[64][4096];
    uint64_t bishopTable[64][512];

    uint64_t ROOK_MAGICS[64] = {
        0xa8002c000108020ULL, 0x6c00049b0002001ULL, 0x100200010090040ULL, 0x2480041000800801ULL,
        0x280028004000800ULL, 0x900410008040022ULL, 0x280020001001080ULL, 0x400080010004220ULL,
        0x1000c604130d0011ULL, 0x8480008080080081ULL, 0x2140023801100802ULL, 0x2008140041101ULL,
        0x8020008120008ULL, 0x2000812000100ULL, 0x100448100045ULL, 0x18000a120030ULL,
        0x108080800108020ULL, 0x6c08109024000301ULL, 0x80010090040ULL, 0x48045000800801ULL,
        0x28004000800ULL, 0x90041000804002ULL, 0x280020001001080ULL, 0x400080010004200ULL,
        0x40102011000040ULL, 0x801011000d1100ULL, 0x11002230110402ULL, 0x81000850304002ULL,
        0x4012820001001ULL, 0x401082000301ULL, 0x11008500041015ULL, 0x1105210040082ULL,
        0x20400080808080ULL, 0x10020200100080ULL, 0x800080800080ULL, 0x480020000440eULL,
        0x1000800020002ULL, 0x8000100020002ULL, 0x800010001000202ULL, 0x8000100010002ULL,
        0x20400080808080ULL, 0x10020200100080ULL, 0x800080800080ULL, 0x480020000440ULL,
        0x1000800020002ULL, 0x8000100020002ULL, 0x800010001000202ULL, 0x8000100010002ULL,
        0x20400080808080ULL, 0x10020200100080ULL, 0x800080800080ULL, 0x480020000440ULL,
        0x1000800020002ULL, 0x8000100020002ULL, 0x800010001000202ULL, 0x8000100010002ULL,
        0x20400080808080ULL, 0x10020200100080ULL, 0x800080800080ULL, 0x480020000440ULL,
        0x1000800020002ULL, 0x8000100020002ULL, 0x800010001000202ULL, 0x8000100010002ULL
    };

    uint64_t BISHOP_MAGICS[64] = {
        0x40040822104ULL, 0x40810a410800ULL, 0x2021000c01080ULL, 0x40102011000ULL,
        0x40102011000ULL, 0x2021000c01080ULL, 0x40810a410800ULL, 0x40040822104ULL,
        0x4001021200ULL, 0x4001021200ULL, 0x4001021200ULL, 0x4001021200ULL,
        0x4001021200ULL, 0x4001021200ULL, 0x4001021200ULL, 0x4001021200ULL,
        0x200100808204ULL, 0x200100808204ULL, 0x200100808204ULL, 0x200100808204ULL,
        0x200100808204ULL, 0x200100808204ULL, 0x200100808204ULL, 0x200100808204ULL,
        0x4100410041ULL, 0x4100410041ULL, 0x4100410041ULL, 0x4100410041ULL,
        0x4100410041ULL, 0x4100410041ULL, 0x4100410041ULL, 0x4100410041ULL,
        0x4100410041ULL, 0x4100410041ULL, 0x4100410041ULL, 0x4100410041ULL,
        0x4100410041ULL, 0x4100410041ULL, 0x4100410041ULL, 0x4100410041ULL,
        0x200100808204ULL, 0x200100808204ULL, 0x200100808204ULL, 0x200100808204ULL,
        0x200100808204ULL, 0x200100808204ULL, 0x200100808204ULL, 0x200100808204ULL,
        0x4001021200ULL, 0x4001021200ULL, 0x4001021200ULL, 0x4001021200ULL,
        0x4001021200ULL, 0x4001021200ULL, 0x4001021200ULL, 0x4001021200ULL,
        0x40040822104ULL, 0x40810a410800ULL, 0x2021000c01080ULL, 0x40102011000ULL,
        0x40102011000ULL, 0x2021000c01080ULL, 0x40810a410800ULL, 0x40040822104ULL
    };

    const int ROOK_SHIFTS[64] = {
        52, 53, 53, 53, 53, 53, 53, 52,
        53, 54, 54, 54, 54, 54, 54, 53,
        53, 54, 54, 54, 54, 54, 54, 53,
        53, 54, 54, 54, 54, 54, 54, 53,
        53, 54, 54, 54, 54, 54, 54, 53,
        53, 54, 54, 54, 54, 54, 54, 53,
        53, 54, 54, 54, 54, 54, 54, 53,
        52, 53, 53, 53, 53, 53, 53, 52
    };

    const int BISHOP_SHIFTS[64] = {
        58, 59, 59, 59, 59, 59, 59, 58,
        59, 59, 59, 59, 59, 59, 59, 59,
        59, 59, 57, 57, 57, 57, 59, 59,
        59, 59, 57, 55, 55, 57, 59, 59,
        59, 59, 57, 55, 55, 57, 59, 59,
        59, 59, 57, 57, 57, 57, 59, 59,
        59, 59, 59, 59, 59, 59, 59, 59,
        58, 59, 59, 59, 59, 59, 59, 58
    };

    uint64_t rookMask(int sq) {
        uint64_t mask = 0;
        int r = sq / 8, f = sq % 8;
        for (int i = r + 1; i < 7; i++) mask |= (1ULL << (i * 8 + f));
        for (int i = r - 1; i > 0; i--) mask |= (1ULL << (i * 8 + f));
        for (int i = f + 1; i < 7; i++) mask |= (1ULL << (r * 8 + i));
        for (int i = f - 1; i > 0; i--) mask |= (1ULL << (r * 8 + i));
        return mask;
    }

    uint64_t bishopMask(int sq) {
        uint64_t mask = 0;
        int r = sq / 8, f = sq % 8;
        for (int i = r + 1, j = f + 1; i < 7 && j < 7; i++, j++) mask |= (1ULL << (i * 8 + j));
        for (int i = r + 1, j = f - 1; i < 7 && j > 0; i++, j--) mask |= (1ULL << (i * 8 + j));
        for (int i = r - 1, j = f + 1; i > 0 && j < 7; i--, j++) mask |= (1ULL << (i * 8 + j));
        for (int i = r - 1, j = f - 1; i > 0 && j > 0; i--, j--) mask |= (1ULL << (i * 8 + j));
        return mask;
    }

    uint64_t rookAttacksOnTheFly(int square, uint64_t occupancy) {
        uint64_t attacks = 0ULL;
        int tr = square / 8, tf = square % 8;
        for (int r = tr + 1; r <= 7; r++) { attacks |= (1ULL << (r * 8 + tf)); if (occupancy & (1ULL << (r * 8 + tf))) break; }
        for (int r = tr - 1; r >= 0; r--) { attacks |= (1ULL << (r * 8 + tf)); if (occupancy & (1ULL << (r * 8 + tf))) break; }
        for (int f = tf + 1; f <= 7; f++) { attacks |= (1ULL << (tr * 8 + f)); if (occupancy & (1ULL << (tr * 8 + f))) break; }
        for (int f = tf - 1; f >= 0; f--) { attacks |= (1ULL << (tr * 8 + f)); if (occupancy & (1ULL << (tr * 8 + f))) break; }
        return attacks;
    }

    uint64_t bishopAttacksOnTheFly(int square, uint64_t occupancy) {
        uint64_t attacks = 0ULL;
        int tr = square / 8, tf = square % 8;
        for (int r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++) { attacks |= (1ULL << (r * 8 + f)); if (occupancy & (1ULL << (r * 8 + f))) break; }
        for (int r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--) { attacks |= (1ULL << (r * 8 + f)); if (occupancy & (1ULL << (r * 8 + f))) break; }
        for (int r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++) { attacks |= (1ULL << (r * 8 + f)); if (occupancy & (1ULL << (r * 8 + f))) break; }
        for (int r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--) { attacks |= (1ULL << (r * 8 + f)); if (occupancy & (1ULL << (r * 8 + f))) break; }
        return attacks;
    }

    uint64_t setOccupancy(int index, int bitsInMask, uint64_t mask) {
        uint64_t occupancy = 0ULL;
        for (int i = 0; i < bitsInMask; i++) {
            int square = __builtin_ctzll(mask);
            mask &= (mask - 1);
            if (index & (1 << i)) occupancy |= (1ULL << square);
        }
        return occupancy;
    }

    uint64_t findMagic(int sq, int bits, bool bishop) {
        uint64_t mask = bishop ? bishopMask(sq) : rookMask(sq);
        int numOccupancies = 1 << bits;
        uint64_t occupancies[4096], attacks[4096], used[4096];
        bool occupied[4096];
        for (int i = 0; i < numOccupancies; i++) {
            occupancies[i] = setOccupancy(i, bits, mask);
            attacks[i] = bishop ? bishopAttacksOnTheFly(sq, occupancies[i]) : rookAttacksOnTheFly(sq, occupancies[i]);
        }
        std::mt19937_64 rng(sq + (bishop ? 64 : 0));
        while (true) {
            uint64_t magic = rng() & rng() & rng();
            if (__builtin_popcountll((mask * magic) & 0xFF00000000000000ULL) < 6) continue;
            for (int i = 0; i < numOccupancies; i++) occupied[i] = false;
            bool fail = false;
            for (int i = 0; i < numOccupancies; i++) {
                int index = (int)((occupancies[i] * magic) >> (64 - bits));
                if (!occupied[index]) {
                    occupied[index] = true;
                    used[index] = attacks[i];
                }
                else if (used[index] != attacks[i]) { fail = true; break; }
            }
            if (!fail) return magic;
        }
    }

    void init() {
        for (int i = 0; i < 64; i++) {
            ROOK_MASKS[i] = rookMask(i);
            BISHOP_MASKS[i] = bishopMask(i);

            int rookBits = 64 - ROOK_SHIFTS[i];
            int bishopBits = 64 - BISHOP_SHIFTS[i];

            // ROOK
            bool rookValid = true;
            for (int j = 0; j < (1 << rookBits); j++) {
                uint64_t occ = setOccupancy(j, rookBits, ROOK_MASKS[i]);
                int index = (int)((occ * ROOK_MAGICS[i]) >> ROOK_SHIFTS[i]);
                uint64_t atk = rookAttacksOnTheFly(i, occ);
                if (rookTable[i][index] == 0) rookTable[i][index] = atk;
                else if (rookTable[i][index] != atk) { rookValid = false; break; }
            }

            if (!rookValid) {
                ROOK_MAGICS[i] = findMagic(i, rookBits, false);
                for (int j = 0; j < 4096; j++) rookTable[i][j] = 0;
                for (int j = 0; j < (1 << rookBits); j++) {
                    uint64_t occ = setOccupancy(j, rookBits, ROOK_MASKS[i]);
                    int index = (int)((occ * ROOK_MAGICS[i]) >> ROOK_SHIFTS[i]);
                    rookTable[i][index] = rookAttacksOnTheFly(i, occ);
                }
            }

            // BISHOP
            bool bishopValid = true;
            for (int j = 0; j < (1 << bishopBits); j++) {
                uint64_t occ = setOccupancy(j, bishopBits, BISHOP_MASKS[i]);
                int index = (int)((occ * BISHOP_MAGICS[i]) >> BISHOP_SHIFTS[i]);
                uint64_t atk = bishopAttacksOnTheFly(i, occ);
                if (bishopTable[i][index] == 0) bishopTable[i][index] = atk;
                else if (bishopTable[i][index] != atk) { bishopValid = false; break; }
            }

            if (!bishopValid) {
                BISHOP_MAGICS[i] = findMagic(i, bishopBits, true);
                for (int j = 0; j < 512; j++) bishopTable[i][j] = 0;
                for (int j = 0; j < (1 << bishopBits); j++) {
                    uint64_t occ = setOccupancy(j, bishopBits, BISHOP_MASKS[i]);
                    int index = (int)((occ * BISHOP_MAGICS[i]) >> BISHOP_SHIFTS[i]);
                    bishopTable[i][index] = bishopAttacksOnTheFly(i, occ);
                }
            }
        }
    }

}
