#include "types.h"
#include "nitro/fx/fx.h"
extern void func_ov007_020c6784(void *self, int count, const int *p2, const void *p3,
                                const void *p4, const int *const *p5, int mode);

/* Shared coordinate and threshold storage remains ROM-owned. */
extern const unsigned char data_ov007_020ccca4[1];
extern const unsigned char data_ov007_020ccca8[1];
extern const unsigned char data_ov007_020cccac[1];
extern const unsigned char data_ov007_020cccb0[1];
extern const unsigned char data_ov007_020cccb4[1];
extern const unsigned char data_ov007_020cccbc[1];
extern const unsigned char data_ov007_020cccc0[1];
extern const unsigned char data_ov007_020cccc8[1];
extern const unsigned char data_ov007_020ccccc[2];
extern const unsigned char data_ov007_020cccd0[2];
extern const unsigned char data_ov007_020cccdc[2];
extern const unsigned char data_ov007_020ccce0[2];
extern const unsigned char data_ov007_020ccce4[2];
extern const unsigned char data_ov007_020cccec[2];
extern const unsigned char data_ov007_020cccf4[2];
extern const unsigned char data_ov007_020cccfc[2];
extern const unsigned char data_ov007_020ccd00[2];
extern const unsigned char data_ov007_020ccd04[2];
extern const unsigned char data_ov007_020ccd08[3];
extern const unsigned char data_ov007_020ccd0c[3];
extern const unsigned char data_ov007_020ccd10[3];
extern const unsigned char data_ov007_020ccd14[3];
extern const unsigned char data_ov007_020ccd18[3];
extern const unsigned char data_ov007_020ccd1c[3];
extern const unsigned char data_ov007_020ccd24[3];
extern const unsigned char data_ov007_020ccd28[3];
extern const unsigned char data_ov007_020ccd2c[3];
extern const unsigned char data_ov007_020ccd3c[3];
extern const unsigned char data_ov007_020ccd40[3];
extern const unsigned char data_ov007_020ccd44[3];
extern const unsigned char data_ov007_020ccd48[3];
extern const unsigned char data_ov007_020ccd54[3];
extern const int data_ov007_020ccd64[1];
extern const unsigned char data_ov007_020ccd6c[4];
extern const int data_ov007_020ccd70[1];
extern const unsigned char data_ov007_020ccd74[4];
extern const unsigned char data_ov007_020ccd78[4];
extern const unsigned char data_ov007_020ccd80[4];
extern const unsigned char data_ov007_020ccd84[4];
extern const unsigned char data_ov007_020ccd88[4];
extern const unsigned char data_ov007_020ccd8c[4];
extern const unsigned char data_ov007_020ccd94[4];
extern const unsigned char data_ov007_020ccd9c[4];
extern const int data_ov007_020ccda0[1];
extern const int data_ov007_020ccda4[1];
extern const unsigned char data_ov007_020ccda8[4];
extern const unsigned char data_ov007_020ccdb0[4];
extern const unsigned char data_ov007_020ccdb4[4];
extern const unsigned char data_ov007_020ccdbc[5];
extern const unsigned char data_ov007_020ccdc4[5];
extern const unsigned char data_ov007_020ccdcc[5];
extern const unsigned char data_ov007_020ccddc[5];
extern const unsigned char data_ov007_020ccde4[5];
extern const unsigned char data_ov007_020ccdf4[5];
extern const unsigned char data_ov007_020ccdfc[5];
extern const unsigned char data_ov007_020cce04[5];
extern const unsigned char data_ov007_020cce0c[5];
extern const unsigned char data_ov007_020cce1c[5];
extern const unsigned char data_ov007_020cce24[5];
extern const unsigned char data_ov007_020cce2c[5];
extern const unsigned char data_ov007_020cce34[5];
extern const unsigned char data_ov007_020cce3c[5];
extern const unsigned char data_ov007_020cce44[5];
extern const unsigned char data_ov007_020cce4c[5];
extern const unsigned char data_ov007_020cce54[5];
extern const unsigned char data_ov007_020cce5c[5];
extern const unsigned char data_ov007_020cce64[5];
extern const unsigned char data_ov007_020cce6c[5];
extern const unsigned char data_ov007_020cce74[5];
extern const unsigned char data_ov007_020cce7c[5];
extern const unsigned char data_ov007_020cce84[5];
extern const unsigned char data_ov007_020cce8c[5];
extern const unsigned char data_ov007_020cce9c[6];
extern const unsigned char data_ov007_020cceac[6];
extern const unsigned char data_ov007_020cceb4[6];
extern const unsigned char data_ov007_020ccebc[6];
extern const unsigned char data_ov007_020ccecc[6];
extern const unsigned char data_ov007_020cceec[6];
extern const unsigned char data_ov007_020ccefc[6];
extern const unsigned char data_ov007_020ccf14[6];
extern const unsigned char data_ov007_020ccf1c[7];
extern const unsigned char data_ov007_020ccf2c[7];
extern const unsigned char data_ov007_020ccf34[7];
extern const unsigned char data_ov007_020ccf5c[7];
extern const unsigned char data_ov007_020ccf64[7];
extern const unsigned char data_ov007_020ccf6c[7];
extern const unsigned char data_ov007_020ccf74[7];
extern const unsigned char data_ov007_020ccf8c[7];
extern const unsigned char data_ov007_020ccf94[7];
extern const unsigned char data_ov007_020ccf9c[7];
extern const unsigned char data_ov007_020ccfac[7];
extern const unsigned char data_ov007_020ccfb4[7];
extern const unsigned char data_ov007_020ccfbc[8];
extern const unsigned char data_ov007_020ccfc4[8];
extern const unsigned char data_ov007_020ccfcc[8];
extern const unsigned char data_ov007_020ccfd4[8];
extern const int data_ov007_020ccfdc[2];
extern const unsigned char data_ov007_020ccfe4[8];
extern const int data_ov007_020ccfec[2];
extern const int data_ov007_020ccffc[2];
extern const int data_ov007_020cd004[2];
extern const unsigned char data_ov007_020cd024[8];
extern const unsigned char data_ov007_020cd02c[8];
extern const unsigned char data_ov007_020cd04c[8];
extern const unsigned char data_ov007_020cd054[8];
extern const unsigned char data_ov007_020cd05c[8];
extern const int data_ov007_020cd064[2];
extern const unsigned char data_ov007_020cd06c[8];
extern const unsigned char data_ov007_020cd074[8];
extern const unsigned char data_ov007_020cd07c[9];
extern const unsigned char data_ov007_020cd094[9];
extern const unsigned char data_ov007_020cd0ac[9];
extern const unsigned char data_ov007_020cd0b8[9];
extern const unsigned char data_ov007_020cd0f4[9];
extern const unsigned char data_ov007_020cd100[9];
extern const unsigned char data_ov007_020cd10c[10];
extern const unsigned char data_ov007_020cd118[10];
extern const unsigned char data_ov007_020cd13c[10];
extern const unsigned char data_ov007_020cd148[10];
extern const unsigned char data_ov007_020cd154[10];
extern const unsigned char data_ov007_020cd160[10];
extern const unsigned char data_ov007_020cd16c[10];
extern const unsigned char data_ov007_020cd178[10];
extern const unsigned char data_ov007_020cd184[10];
extern const unsigned char data_ov007_020cd190[10];
extern const unsigned char data_ov007_020cd19c[11];
extern const unsigned char data_ov007_020cd1a8[11];
extern const unsigned char data_ov007_020cd1b4[11];
extern const unsigned char data_ov007_020cd1c0[11];
extern const unsigned char data_ov007_020cd1cc[11];
extern const unsigned char data_ov007_020cd1d8[11];
extern const unsigned char data_ov007_020cd1e4[11];
extern const unsigned char data_ov007_020cd1f0[11];
extern const unsigned char data_ov007_020cd1fc[12];
extern const unsigned char data_ov007_020cd208[12];
extern const int data_ov007_020cd214[3];
extern const int data_ov007_020cd220[3];
extern const int data_ov007_020cd22c[3];
extern const int data_ov007_020cd244[3];
extern const unsigned char data_ov007_020cd268[12];
extern const unsigned char data_ov007_020cd274[12];
extern const int data_ov007_020cd280[3];
extern const unsigned char data_ov007_020cd28c[12];
extern const unsigned char data_ov007_020cd298[12];
extern const int data_ov007_020cd2a4[3];
extern const unsigned char data_ov007_020cd2b0[12];
extern const unsigned char data_ov007_020cd2bc[12];
extern const unsigned char data_ov007_020cd2c8[12];
extern const unsigned char data_ov007_020cd2d4[12];
extern const int data_ov007_020cd2e0[3];
extern const unsigned char data_ov007_020cd2ec[12];
extern const unsigned char data_ov007_020cd2f8[12];
extern const unsigned char data_ov007_020cd304[13];
extern const unsigned char data_ov007_020cd314[13];
extern const unsigned char data_ov007_020cd324[13];
extern const unsigned char data_ov007_020cd334[13];
extern const unsigned char data_ov007_020cd344[13];
extern const unsigned char data_ov007_020cd354[13];
extern const unsigned char data_ov007_020cd364[13];
extern const unsigned char data_ov007_020cd374[13];
extern const unsigned char data_ov007_020cd384[14];
extern const unsigned char data_ov007_020cd394[14];
extern const unsigned char data_ov007_020cd3a4[14];
extern const unsigned char data_ov007_020cd434[14];
extern const unsigned char data_ov007_020cd444[15];
extern const unsigned char data_ov007_020cd454[15];
extern const unsigned char data_ov007_020cd464[15];
extern const unsigned char data_ov007_020cd474[15];
extern const int data_ov007_020cd484[4];
extern const int data_ov007_020cd494[4];
extern const int data_ov007_020cd4a4[4];
extern const int data_ov007_020cd4b4[4];
extern const int data_ov007_020cd4c4[4];
extern const unsigned char data_ov007_020cd4d4[16];
extern const unsigned char data_ov007_020cd4e4[16];
extern const unsigned char data_ov007_020cd4f4[16];
extern const unsigned char data_ov007_020cd504[16];
extern const int data_ov007_020cd514[4];
extern const unsigned char data_ov007_020cd524[17];
extern const unsigned char data_ov007_020cd538[17];
extern const unsigned char data_ov007_020cd574[17];
extern const unsigned char data_ov007_020cd588[17];
extern const unsigned char data_ov007_020cd59c[18];
extern const unsigned char data_ov007_020cd5d8[18];
extern const unsigned char data_ov007_020cd5ec[19];
extern const unsigned char data_ov007_020cd600[19];
extern const unsigned char data_ov007_020cd664[19];
extern const unsigned char data_ov007_020cd678[19];
extern const unsigned char data_ov007_020cd68c[20];
extern const int data_ov007_020cd6a0[5];
extern const int data_ov007_020cd6b4[5];
extern const unsigned char data_ov007_020cd6c8[20];
extern const unsigned char data_ov007_020cd6dc[20];
extern const int data_ov007_020cd704[5];
extern const int data_ov007_020cd77c[5];
extern const int data_ov007_020cd7a4[5];
extern const unsigned char data_ov007_020cd7b8[20];
extern const unsigned char data_ov007_020cd7cc[20];
extern const int data_ov007_020cd7e0[5];
extern const int data_ov007_020cd7f4[5];
extern const int data_ov007_020cd808[5];
extern const int data_ov007_020cd81c[5];
extern const int data_ov007_020cd844[5];
extern const int data_ov007_020cd858[5];
extern const int data_ov007_020cd86c[5];
extern const unsigned char data_ov007_020cd880[20];
extern const unsigned char data_ov007_020cd894[21];
extern const unsigned char data_ov007_020cd8ac[21];
extern const unsigned char data_ov007_020cd8f4[22];
extern const unsigned char data_ov007_020cd90c[22];
extern const unsigned char data_ov007_020cd924[22];
extern const unsigned char data_ov007_020cd93c[22];
extern const unsigned char data_ov007_020cd954[23];
extern const unsigned char data_ov007_020cd96c[23];
extern const unsigned char data_ov007_020cd984[24];
extern const int data_ov007_020cd99c[6];
extern const unsigned char data_ov007_020cd9cc[24];
extern const int data_ov007_020cda74[6];
extern const int data_ov007_020cda8c[6];
extern const int data_ov007_020cdaec[6];
extern const unsigned char data_ov007_020cdb4c[24];
extern const unsigned char data_ov007_020cdb64[24];
extern const unsigned char data_ov007_020cdb7c[25];
extern const unsigned char data_ov007_020cdb98[25];
extern const unsigned char data_ov007_020cdbb4[25];
extern const unsigned char data_ov007_020cdbd0[25];
extern const unsigned char data_ov007_020cdbec[26];
extern const unsigned char data_ov007_020cdc08[26];
extern const unsigned char data_ov007_020cdc94[26];
extern const unsigned char data_ov007_020cdcb0[26];
extern const unsigned char data_ov007_020cdccc[27];
extern const unsigned char data_ov007_020cdce8[27];
extern const int data_ov007_020cdd04[7];
extern const int data_ov007_020cdd20[7];
extern const int data_ov007_020cdd3c[7];
extern const int data_ov007_020cdd58[7];
extern const unsigned char data_ov007_020cdd74[28];
extern const unsigned char data_ov007_020cdd90[28];
extern const int data_ov007_020cde1c[7];
extern const int data_ov007_020cde38[7];
extern const unsigned char data_ov007_020cde54[30];
extern const unsigned char data_ov007_020cde74[30];
extern const unsigned char data_ov007_020cde94[31];
extern const unsigned char data_ov007_020cdeb4[31];
extern const unsigned char data_ov007_020cdf14[31];
extern const unsigned char data_ov007_020cdf34[31];
extern const int data_ov007_020cdf54[8];
extern const int data_ov007_020cdf74[8];
extern const int data_ov007_020cdf94[8];
extern const int data_ov007_020cdfb4[8];
extern const int data_ov007_020ce034[8];
extern const int data_ov007_020ce054[8];
extern const unsigned char data_ov007_020ce074[33];
extern const unsigned char data_ov007_020ce098[33];
extern const int data_ov007_020ce104[9];
extern const int data_ov007_020ce194[9];
extern const int data_ov007_020ce1b8[9];
extern const unsigned char data_ov007_020ce1dc[37];
extern const unsigned char data_ov007_020ce204[37];
extern const unsigned char data_ov007_020ce22c[38];
extern const unsigned char data_ov007_020ce254[38];
extern const int data_ov007_020ce27c[10];
extern const int data_ov007_020ce2cc[10];
extern const int data_ov007_020ce2f4[10];
extern const int data_ov007_020ce31c[10];
extern const int data_ov007_020ce344[10];
extern const unsigned char data_ov007_020ce36c[42];
extern const unsigned char data_ov007_020ce398[42];
extern const int data_ov007_020ce3c4[11];
extern const int data_ov007_020ce3f0[11];
extern const int data_ov007_020ce41c[11];
extern const int data_ov007_020ce448[11];
extern const int data_ov007_020ce474[12];
extern const int data_ov007_020ce4a4[12];
extern const int data_ov007_020ce4d4[12];
extern const int data_ov007_020ce504[12];
extern const int data_ov007_020ce534[12];
extern const int data_ov007_020ce564[12];
extern const int data_ov007_020ce594[13];
extern const unsigned char data_ov007_020ce5c8[52];
extern const unsigned char data_ov007_020ce5fc[52];
extern const int data_ov007_020ce630[13];
extern const int data_ov007_020ce664[13];
extern const int data_ov007_020ce698[13];
extern const int data_ov007_020ce704[14];
extern const int data_ov007_020ce774[14];
extern const int data_ov007_020ce8fc[15];
extern const int data_ov007_020ce938[15];
extern const unsigned char data_ov007_020ce974[63];
extern const unsigned char data_ov007_020ce9b4[63];
extern const int data_ov007_020ce9f4[16];
extern const int data_ov007_020cea34[16];
extern const int data_ov007_020cea74[17];
extern const int data_ov007_020ceafc[17];
extern const int data_ov007_020ceb40[18];
extern const int data_ov007_020cebd0[19];
extern const int data_ov007_020cecb4[19];
extern const unsigned char data_ov007_020ced00[77];
extern const unsigned char data_ov007_020ced50[77];
extern const int data_ov007_020ceda0[20];
extern const int data_ov007_020cedf0[20];
extern const int data_ov007_020cef30[20];
extern const int data_ov007_020cefd0[21];
extern const int data_ov007_020cf178[22];
extern const int data_ov007_020cf1d0[22];
extern const int data_ov007_020cf228[23];
extern const int data_ov007_020cf284[24];
extern const int data_ov007_020cf2e4[24];
extern const int data_ov007_020cf344[25];
extern const int data_ov007_020cf3a8[25];
extern const int data_ov007_020cf40c[26];
extern const int data_ov007_020cf544[26];
extern const int data_ov007_020cf5ac[27];
extern const int data_ov007_020cf618[28];
extern const int data_ov007_020cf688[30];
extern const int data_ov007_020cf778[31];
extern const int data_ov007_020cf870[31];
extern const int data_ov007_020cf9ec[33];
extern const int data_ov007_020cfc18[37];
extern const int data_ov007_020cfcac[38];
extern const int data_ov007_020cfd44[42];
extern const int data_ov007_020cfe94[52];
extern const int data_ov007_020cff64[63];
extern const int data_ov007_020d0060[77];

/* Six portrait drawings, selected by a two-way selector and three-step cycle.
 * Plotting the byte coordinates produces Mario, Luigi and Yoshi portraits.
 * Names are reconstructed from that evidence, not recovered identifiers.
 * The projector uses these as screen X/Y inputs and negates Y; depth is separate.
 * Luigi pointer arrays and all shared leaf data remain ROM imports.
 * Threshold units and original identifiers are unproven.
 */
static const int sPortraitStrokeCounts[6] = {30, 34, 36, 20, 32, 32};
static const int sMarioFrontStrokePointCounts[30] = {16, 12, 10, 17, 7,  8, 52, 19, 16, 10,
                                                     13, 12, 24, 12, 13, 3, 2,  12, 13, 3,
                                                     77, 17, 20, 6,  8,  2, 2,  7,  7,  3};
static const int sMarioSideStrokePointCounts[34] = {22, 9,  5,  5,  2,  15, 11, 4, 1,  8,  6,  25,
                                                    25, 3,  21, 12, 5,  23, 10, 4, 14, 24, 30, 38,
                                                    20, 12, 13, 11, 33, 31, 18, 1, 1,  2};
/* The stored final one-point record is unused by the 36-stroke dispatcher. */
static const int sLuigiStrokePointCounts[37] = {18, 7,  5,  14, 6,  3,  10, 7,  8,  20, 26, 20, 22,
                                                6,  3,  14, 8,  20, 31, 3,  26, 14, 19, 9,  7,  35,
                                                9,  17, 19, 5,  6,  9,  7,  6,  14, 8,  1};
static const int sYoshiPlainStrokePointCounts[20] = {27, 14, 5,  8,  4,  3,  19, 10, 63, 11,
                                                     9,  31, 15, 11, 22, 26, 5,  20, 5,  4};
static const int sYoshiHeartStrokePointCounts[32] = {27, 14, 5,  8,  4,  3, 19, 10, 63, 11, 9,
                                                     31, 15, 11, 22, 26, 5, 20, 5,  4,  28, 42,
                                                     26, 3,  1,  4,  6,  6, 5,  5,  4,  3};
static const int sYoshiStarStrokePointCounts[32] = {27, 14, 5,  8,  4,  3,  19, 10, 63, 11, 9,
                                                    31, 15, 11, 22, 26, 5,  20, 5,  4,  37, 5,
                                                    5,  8,  9,  7,  5,  10, 8,  7,  5,  7};
static const int *sPortraitStrokePointCounts[6] = {
    sMarioFrontStrokePointCounts, sMarioSideStrokePointCounts,  sLuigiStrokePointCounts,
    sYoshiPlainStrokePointCounts, sYoshiHeartStrokePointCounts, sYoshiStarStrokePointCounts};
static const unsigned char *sMarioFrontStrokeXCoords[30] = {
    data_ov007_020cd4d4, data_ov007_020cd268, data_ov007_020cd154, data_ov007_020cd574,
    data_ov007_020ccf94, data_ov007_020cd04c, data_ov007_020ce5c8, data_ov007_020cd664,
    data_ov007_020cd4f4, data_ov007_020cd178, data_ov007_020cd334, data_ov007_020cd2b0,
    data_ov007_020cdb4c, data_ov007_020cd2c8, data_ov007_020cd354, data_ov007_020ccd54,
    data_ov007_020ccd00, data_ov007_020cd2ec, data_ov007_020cd374, data_ov007_020ccd08,
    data_ov007_020ced00, data_ov007_020cd538, data_ov007_020cd880, data_ov007_020cce9c,
    data_ov007_020ccfcc, data_ov007_020ccccc, data_ov007_020cccdc, data_ov007_020ccf64,
    data_ov007_020ccf34, data_ov007_020ccd1c};
static const unsigned char *sMarioSideStrokeXCoords[34] = {
    data_ov007_020cd924, data_ov007_020cd100, data_ov007_020cce1c, data_ov007_020cce6c,
    data_ov007_020cccfc, data_ov007_020cd474, data_ov007_020cd1c0, data_ov007_020ccdb0,
    data_ov007_020cccbc, data_ov007_020ccfbc, data_ov007_020ccefc, data_ov007_020cdb7c,
    data_ov007_020cdbb4, data_ov007_020ccd2c, data_ov007_020cd8ac, data_ov007_020cd298,
    data_ov007_020cce74, data_ov007_020cd954, data_ov007_020cd118, data_ov007_020ccd94,
    data_ov007_020cd434, data_ov007_020cd9cc, data_ov007_020cde54, data_ov007_020ce22c,
    data_ov007_020cd6c8, data_ov007_020cd208, data_ov007_020cd324, data_ov007_020cd19c,
    data_ov007_020ce074, data_ov007_020cde94, data_ov007_020cd5d8, data_ov007_020ccca4,
    data_ov007_020ccca8, data_ov007_020ccd04};
extern const unsigned char *const data_ov007_020d85fc[36];
static const unsigned char *sYoshiPlainStrokeXCoords[20] = {
    data_ov007_020cdccc, data_ov007_020cd394, data_ov007_020cce7c, data_ov007_020cd074,
    data_ov007_020ccd74, data_ov007_020ccd10, data_ov007_020cd5ec, data_ov007_020cd13c,
    data_ov007_020ce974, data_ov007_020cd1cc, data_ov007_020cd0ac, data_ov007_020cdf14,
    data_ov007_020cd464, data_ov007_020cd1e4, data_ov007_020cd8f4, data_ov007_020cdc94,
    data_ov007_020cce64, data_ov007_020cd7b8, data_ov007_020cce24, data_ov007_020ccd78};
static const unsigned char *sYoshiHeartStrokeXCoords[32] = {
    data_ov007_020cdccc, data_ov007_020cd394, data_ov007_020cce7c, data_ov007_020cd074,
    data_ov007_020ccd74, data_ov007_020ccd10, data_ov007_020cd5ec, data_ov007_020cd13c,
    data_ov007_020ce974, data_ov007_020cd1cc, data_ov007_020cd0ac, data_ov007_020cdf14,
    data_ov007_020cd464, data_ov007_020cd1e4, data_ov007_020cd8f4, data_ov007_020cdc94,
    data_ov007_020cce64, data_ov007_020cd7b8, data_ov007_020cce24, data_ov007_020ccd78,
    data_ov007_020cdd74, data_ov007_020ce36c, data_ov007_020cdbec, data_ov007_020ccd18,
    data_ov007_020cccac, data_ov007_020ccda8, data_ov007_020ccebc, data_ov007_020ccf14,
    data_ov007_020cce2c, data_ov007_020cce0c, data_ov007_020ccd84, data_ov007_020ccd40};
static const unsigned char *sYoshiStarStrokeXCoords[32] = {
    data_ov007_020cdccc, data_ov007_020cd394, data_ov007_020cce7c, data_ov007_020cd074,
    data_ov007_020ccd74, data_ov007_020ccd10, data_ov007_020cd5ec, data_ov007_020cd13c,
    data_ov007_020ce974, data_ov007_020cd1cc, data_ov007_020cd0ac, data_ov007_020cdf14,
    data_ov007_020cd464, data_ov007_020cd1e4, data_ov007_020cd8f4, data_ov007_020cdc94,
    data_ov007_020cce64, data_ov007_020cd7b8, data_ov007_020cce24, data_ov007_020ccd78,
    data_ov007_020ce1dc, data_ov007_020cce5c, data_ov007_020cce34, data_ov007_020cd05c,
    data_ov007_020cd07c, data_ov007_020ccf6c, data_ov007_020ccdfc, data_ov007_020cd190,
    data_ov007_020cd024, data_ov007_020ccf74, data_ov007_020ccdf4, data_ov007_020ccf2c};
static const unsigned char *const *sPortraitStrokeXCoords[6] = {
    sMarioFrontStrokeXCoords, sMarioSideStrokeXCoords,  data_ov007_020d85fc,
    sYoshiPlainStrokeXCoords, sYoshiHeartStrokeXCoords, sYoshiStarStrokeXCoords};
static const unsigned char *sMarioFrontStrokeYCoords[30] = {
    data_ov007_020cd4e4, data_ov007_020cd274, data_ov007_020cd160, data_ov007_020cd588,
    data_ov007_020ccf8c, data_ov007_020cd054, data_ov007_020ce5fc, data_ov007_020cd678,
    data_ov007_020cd504, data_ov007_020cd184, data_ov007_020cd344, data_ov007_020cd2bc,
    data_ov007_020cdb64, data_ov007_020cd2d4, data_ov007_020cd364, data_ov007_020ccd0c,
    data_ov007_020ccce0, data_ov007_020cd2f8, data_ov007_020cd314, data_ov007_020ccd3c,
    data_ov007_020ced50, data_ov007_020cd524, data_ov007_020cd68c, data_ov007_020cceb4,
    data_ov007_020ccfe4, data_ov007_020cccf4, data_ov007_020cccd0, data_ov007_020ccf5c,
    data_ov007_020ccfb4, data_ov007_020ccd44};
static const unsigned char *sMarioSideStrokeYCoords[34] = {
    data_ov007_020cd93c, data_ov007_020cd0f4, data_ov007_020cce84, data_ov007_020ccde4,
    data_ov007_020ccce4, data_ov007_020cd444, data_ov007_020cd1b4, data_ov007_020ccd80,
    data_ov007_020cccc8, data_ov007_020ccfd4, data_ov007_020cceec, data_ov007_020cdb98,
    data_ov007_020cdbd0, data_ov007_020ccd14, data_ov007_020cd894, data_ov007_020cd1fc,
    data_ov007_020ccdcc, data_ov007_020cd96c, data_ov007_020cd10c, data_ov007_020ccd88,
    data_ov007_020cd384, data_ov007_020cd984, data_ov007_020cde74, data_ov007_020ce254,
    data_ov007_020cd6dc, data_ov007_020cd28c, data_ov007_020cd304, data_ov007_020cd1a8,
    data_ov007_020ce098, data_ov007_020cdeb4, data_ov007_020cd59c, data_ov007_020cccb0,
    data_ov007_020cccc0, data_ov007_020cccec};
extern const unsigned char *const data_ov007_020d8690[36];
static const unsigned char *sYoshiPlainStrokeYCoords[20] = {
    data_ov007_020cdce8, data_ov007_020cd3a4, data_ov007_020ccddc, data_ov007_020cd06c,
    data_ov007_020ccdb4, data_ov007_020ccd48, data_ov007_020cd600, data_ov007_020cd148,
    data_ov007_020ce9b4, data_ov007_020cd1d8, data_ov007_020cd0b8, data_ov007_020cdf34,
    data_ov007_020cd454, data_ov007_020cd1f0, data_ov007_020cd90c, data_ov007_020cdcb0,
    data_ov007_020ccdc4, data_ov007_020cd7cc, data_ov007_020cce4c, data_ov007_020ccd6c};
static const unsigned char *sYoshiHeartStrokeYCoords[32] = {
    data_ov007_020cdce8, data_ov007_020cd3a4, data_ov007_020ccddc, data_ov007_020cd06c,
    data_ov007_020ccdb4, data_ov007_020ccd48, data_ov007_020cd600, data_ov007_020cd148,
    data_ov007_020ce9b4, data_ov007_020cd1d8, data_ov007_020cd0b8, data_ov007_020cdf34,
    data_ov007_020cd454, data_ov007_020cd1f0, data_ov007_020cd90c, data_ov007_020cdcb0,
    data_ov007_020ccdc4, data_ov007_020cd7cc, data_ov007_020cce4c, data_ov007_020ccd6c,
    data_ov007_020cdd90, data_ov007_020ce398, data_ov007_020cdc08, data_ov007_020ccd28,
    data_ov007_020cccb4, data_ov007_020ccd8c, data_ov007_020ccecc, data_ov007_020cceac,
    data_ov007_020cce04, data_ov007_020cce3c, data_ov007_020ccd9c, data_ov007_020ccd24};
static const unsigned char *sYoshiStarStrokeYCoords[32] = {
    data_ov007_020cdce8, data_ov007_020cd3a4, data_ov007_020ccddc, data_ov007_020cd06c,
    data_ov007_020ccdb4, data_ov007_020ccd48, data_ov007_020cd600, data_ov007_020cd148,
    data_ov007_020ce9b4, data_ov007_020cd1d8, data_ov007_020cd0b8, data_ov007_020cdf34,
    data_ov007_020cd454, data_ov007_020cd1f0, data_ov007_020cd90c, data_ov007_020cdcb0,
    data_ov007_020ccdc4, data_ov007_020cd7cc, data_ov007_020cce4c, data_ov007_020ccd6c,
    data_ov007_020ce204, data_ov007_020cce44, data_ov007_020cce8c, data_ov007_020cd02c,
    data_ov007_020cd094, data_ov007_020ccfac, data_ov007_020cce54, data_ov007_020cd16c,
    data_ov007_020ccfc4, data_ov007_020ccf9c, data_ov007_020ccdbc, data_ov007_020ccf1c};
static const unsigned char *const *sPortraitStrokeYCoords[6] = {
    sMarioFrontStrokeYCoords, sMarioSideStrokeYCoords,  data_ov007_020d8690,
    sYoshiPlainStrokeYCoords, sYoshiHeartStrokeYCoords, sYoshiStarStrokeYCoords};
static const int *sMarioFrontStrokeProgressThresholds[30] = {
    data_ov007_020ce9f4, data_ov007_020ce4d4, data_ov007_020ce2f4, data_ov007_020ceafc,
    data_ov007_020cde1c, data_ov007_020ce034, data_ov007_020cfe94, data_ov007_020cecb4,
    data_ov007_020cea34, data_ov007_020ce31c, data_ov007_020ce630, data_ov007_020ce504,
    data_ov007_020cf2e4, data_ov007_020ce534, data_ov007_020ce664, data_ov007_020cd2e0,
    data_ov007_020ccffc, data_ov007_020ce564, data_ov007_020ce698, data_ov007_020cd22c,
    data_ov007_020d0060, data_ov007_020cea74, data_ov007_020ceda0, data_ov007_020cd99c,
    data_ov007_020cdf94, data_ov007_020ccfdc, data_ov007_020ccfec, data_ov007_020cdd3c,
    data_ov007_020cdd58, data_ov007_020cd220};
static const int *sMarioSideStrokeProgressThresholds[34] = {
    data_ov007_020cf1d0, data_ov007_020ce1b8, data_ov007_020cd858, data_ov007_020cd86c,
    data_ov007_020cd064, data_ov007_020ce938, data_ov007_020ce448, data_ov007_020cd514,
    data_ov007_020ccd64, data_ov007_020cdf54, data_ov007_020cdaec, data_ov007_020cf344,
    data_ov007_020cf3a8, data_ov007_020cd2a4, data_ov007_020cefd0, data_ov007_020ce474,
    data_ov007_020cd6a0, data_ov007_020cf228, data_ov007_020ce27c, data_ov007_020cd4a4,
    data_ov007_020ce704, data_ov007_020cf284, data_ov007_020cf688, data_ov007_020cfcac,
    data_ov007_020cedf0, data_ov007_020ce4a4, data_ov007_020ce594, data_ov007_020ce3c4,
    data_ov007_020cf9ec, data_ov007_020cf778, data_ov007_020ceb40, data_ov007_020ccda4,
    data_ov007_020ccda0, data_ov007_020cd004};
extern const int *const data_ov007_020d8724[36];
static const int *sYoshiPlainStrokeProgressThresholds[20] = {
    data_ov007_020cf5ac, data_ov007_020ce774, data_ov007_020cd704, data_ov007_020cdfb4,
    data_ov007_020cd484, data_ov007_020cd214, data_ov007_020cebd0, data_ov007_020ce2cc,
    data_ov007_020cff64, data_ov007_020ce3f0, data_ov007_020ce104, data_ov007_020cf870,
    data_ov007_020ce8fc, data_ov007_020ce41c, data_ov007_020cf178, data_ov007_020cf544,
    data_ov007_020cd7a4, data_ov007_020cef30, data_ov007_020cd7f4, data_ov007_020cd4b4};
static const int *sYoshiHeartStrokeProgressThresholds[32] = {
    data_ov007_020cf5ac, data_ov007_020ce774, data_ov007_020cd704, data_ov007_020cdfb4,
    data_ov007_020cd484, data_ov007_020cd214, data_ov007_020cebd0, data_ov007_020ce2cc,
    data_ov007_020cff64, data_ov007_020ce3f0, data_ov007_020ce104, data_ov007_020cf870,
    data_ov007_020ce8fc, data_ov007_020ce41c, data_ov007_020cf178, data_ov007_020cf544,
    data_ov007_020cd7a4, data_ov007_020cef30, data_ov007_020cd7f4, data_ov007_020cd4b4,
    data_ov007_020cf618, data_ov007_020cfd44, data_ov007_020cf40c, data_ov007_020cd244,
    data_ov007_020ccd70, data_ov007_020cd494, data_ov007_020cda74, data_ov007_020cda8c,
    data_ov007_020cd77c, data_ov007_020cd7e0, data_ov007_020cd4c4, data_ov007_020cd280};
static const int *sYoshiStarStrokeProgressThresholds[32] = {
    data_ov007_020cf5ac, data_ov007_020ce774, data_ov007_020cd704, data_ov007_020cdfb4,
    data_ov007_020cd484, data_ov007_020cd214, data_ov007_020cebd0, data_ov007_020ce2cc,
    data_ov007_020cff64, data_ov007_020ce3f0, data_ov007_020ce104, data_ov007_020cf870,
    data_ov007_020ce8fc, data_ov007_020ce41c, data_ov007_020cf178, data_ov007_020cf544,
    data_ov007_020cd7a4, data_ov007_020cef30, data_ov007_020cd7f4, data_ov007_020cd4b4,
    data_ov007_020cfc18, data_ov007_020cd808, data_ov007_020cd81c, data_ov007_020ce054,
    data_ov007_020ce194, data_ov007_020cde38, data_ov007_020cd844, data_ov007_020ce344,
    data_ov007_020cdf74, data_ov007_020cdd04, data_ov007_020cd6b4, data_ov007_020cdd20};
static const int *const *sPortraitStrokeProgressThresholds[6] = {
    sMarioFrontStrokeProgressThresholds,
    sMarioSideStrokeProgressThresholds,
    data_ov007_020d8724,
    sYoshiPlainStrokeProgressThresholds,
    sYoshiHeartStrokeProgressThresholds,
    sYoshiStarStrokeProgressThresholds};

extern char *data_ov007_0210342c;
extern char *data_ov007_02104ba0;

void func_ov007_020bbe60(int idx) {
    int drawing = idx * 3 + *(u8 *)(data_ov007_0210342c + idx + 0xfc);
    func_ov007_020c6784(*(void **)data_ov007_02104ba0, sPortraitStrokeCounts[drawing],
                        sPortraitStrokePointCounts[drawing], sPortraitStrokeXCoords[drawing],
                        sPortraitStrokeYCoords[drawing], sPortraitStrokeProgressThresholds[drawing],
                        1);
    *(int *)(data_ov007_02104ba0 + 0x38) = 0x78;
    if (idx == 0 && *(u8 *)(data_ov007_0210342c + idx + 0xfc) == 2) {
        int *o = (int *)data_ov007_02104ba0;
        int mid = FX_MUL((o[0x38 / 4] << 12), 0x1800);
        o[0x38 / 4] = mid >> 12;
    }
    ((u8 *)(data_ov007_0210342c + 0xfc))[idx]++;
    {
        u8 *p = (u8 *)(data_ov007_0210342c + 0xfc);
        if (p[idx] >= 3)
            p[idx] = 0;
    }
}
