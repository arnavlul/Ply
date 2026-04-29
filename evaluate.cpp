#include "evaluate.h"
namespace Evaluation {
    uint64_t pieceKeys[12][64];
    uint64_t castleKeys[16];
    uint64_t enPassantKeys[8];
    uint64_t sideKey;
    uint64_t passedPawnMasks[2][64];
    uint64_t pawnShieldMasks[2][64];
    uint64_t kingZoneMasks[2][64];
    uint64_t fileMasks[8];
    uint64_t rankMasks[8];
    uint64_t adjacentFileMasks[8];

    const int safetyTable[100] = {
        0,  0,  1,  2,  3,  5,  7,  9, 12, 15,
       18, 22, 26, 30, 35, 40, 45, 51, 57, 63,
       70, 77, 84, 91, 99, 107, 115, 123, 132, 141,
       150, 160, 170, 180, 190, 201, 212, 223, 235, 247,
       259, 272, 285, 298, 312, 326, 340, 355, 370, 385,
       401, 417, 433, 450, 467, 484, 502, 520, 538, 557,
       576, 595, 615, 635, 655, 676, 697, 718, 740, 762,
       784, 807, 830, 853, 877, 901, 925, 950, 975, 1000,
       1025, 1050, 1075, 1100, 1125, 1150, 1175, 1200, 1225, 1250,
       1275, 1300, 1325, 1350, 1375, 1400, 1425, 1450, 1475, 1500
    };

    uint64_t random64() {
        static uint64_t seed = 1070372ULL;
        seed ^= seed >> 12;
        seed ^= seed << 25;
        seed ^= seed >> 27;
        return seed * 0x2545F4914F6CDD1DULL;
    }

    void initZobrist() {
        for (int i = 0; i < 12; i++)
            for (int j = 0; j < 64; j++)
                pieceKeys[i][j] = random64();
        for (int i = 0; i < 16; i++) castleKeys[i] = random64();
        for (int i = 0; i < 8; i++) enPassantKeys[i] = random64();
        sideKey = random64();
    }

    void initEvaluate() {
        for (int sq = 0; sq < 64; sq++) {
            int file = sq % 8;
            int rank = sq / 8;

            // Passed pawn masks
            uint64_t wMask = 0;
            for (int r = rank + 1; r < 8; r++) {
                wMask |= (1ULL << (r * 8 + file));
                if (file > 0) wMask |= (1ULL << (r * 8 + file - 1));
                if (file < 7) wMask |= (1ULL << (r * 8 + file + 1));
            }
            passedPawnMasks[1][sq] = wMask;

            uint64_t bMask = 0;
            for (int r = rank - 1; r >= 0; r--) {
                bMask |= (1ULL << (r * 8 + file));
                if (file > 0) bMask |= (1ULL << (r * 8 + file - 1));
                if (file < 7) bMask |= (1ULL << (r * 8 + file + 1));
            }
            passedPawnMasks[0][sq] = bMask;

            // King Zone Masks
            uint64_t kz = 0;
            for (int dr = -1; dr <= 1; dr++) {
                for (int df = -1; df <= 1; df++) {
                    int nr = rank + dr;
                    int nf = file + df;
                    if (nr >= 0 && nr < 8 && nf >= 0 && nf < 8) {
                        kz |= (1ULL << (nr * 8 + nf));
                    }
                }
            }
            // Add 3 squares in front
            int frontRankW = rank + 2;
            if (frontRankW < 8) {
                for (int df = -1; df <= 1; df++) {
                    int nf = file + df;
                    if (nf >= 0 && nf < 8) kz |= (1ULL << (frontRankW * 8 + nf));
                }
            }
            kingZoneMasks[1][sq] = kz;

            uint64_t bKz = 0;
            for (int dr = -1; dr <= 1; dr++) {
                for (int df = -1; df <= 1; df++) {
                    int nr = rank + dr;
                    int nf = file + df;
                    if (nr >= 0 && nr < 8 && nf >= 0 && nf < 8) {
                        bKz |= (1ULL << (nr * 8 + nf));
                    }
                }
            }
            int frontRankB = rank - 2;
            if (frontRankB >= 0) {
                for (int df = -1; df <= 1; df++) {
                    int nf = file + df;
                    if (nf >= 0 && nf < 8) bKz |= (1ULL << (frontRankB * 8 + nf));
                }
            }
            kingZoneMasks[0][sq] = bKz;

            // Pawn Shield Masks (3x2 grid in front of king)
            uint64_t wShield = 0;
            for (int r = rank + 1; r <= rank + 2 && r < 8; r++) {
                for (int f = file - 1; f <= file + 1; f++) {
                    if (f >= 0 && f < 8) wShield |= (1ULL << (r * 8 + f));
                }
            }
            pawnShieldMasks[1][sq] = wShield;

            uint64_t bShield = 0;
            for (int r = rank - 1; r >= rank - 2 && r >= 0; r--) {
                for (int f = file - 1; f <= file + 1; f++) {
                    if (f >= 0 && f < 8) bShield |= (1ULL << (r * 8 + f));
                }
            }
            pawnShieldMasks[0][sq] = bShield;
        }

        for (int i = 0; i < 8; i++) {
            fileMasks[i] = 0x0101010101010101ULL << i;
            rankMasks[i] = 0xFFULL << (i * 8);
        }
        for (int i = 0; i < 8; i++) {
            adjacentFileMasks[i] = 0;
            if (i > 0) adjacentFileMasks[i] |= fileMasks[i - 1];
            if (i < 7) adjacentFileMasks[i] |= fileMasks[i + 1];
        }
    }
}
