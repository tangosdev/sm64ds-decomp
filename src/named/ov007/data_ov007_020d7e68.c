#include "types.h"

extern const int data_ov007_020cf700[30];
extern const int data_ov007_020cfa70[34];
extern const int data_ov007_020cfb84[36];
extern const int data_ov007_020cef80[20];
extern const int data_ov007_020cf96c[32];
extern const int data_ov007_020cf8ec[32];
extern const unsigned char *const data_ov007_020d7ffc[30];
extern const unsigned char *const data_ov007_020d8464[34];
extern const unsigned char *const data_ov007_020d85fc[36];
extern const unsigned char *const data_ov007_020d7f0c[20];
extern const unsigned char *const data_ov007_020d82e4[32];
extern const unsigned char *const data_ov007_020d8164[32];
extern const unsigned char *const data_ov007_020d8074[30];
extern const unsigned char *const data_ov007_020d84ec[34];
extern const unsigned char *const data_ov007_020d8690[36];
extern const unsigned char *const data_ov007_020d7f5c[20];
extern const unsigned char *const data_ov007_020d8364[32];
extern const unsigned char *const data_ov007_020d81e4[32];
extern const int *const data_ov007_020d80ec[30];
extern const int *const data_ov007_020d8574[34];
extern const int *const data_ov007_020d8724[36];
extern const int *const data_ov007_020d7fac[20];
extern const int *const data_ov007_020d83e4[32];
extern const int *const data_ov007_020d8264[32];

/* Pointer tables for the six portraits. They sit at 0x020d7e68, before a
 * .data claim on an earlier function, so they cannot live in the reader. */
#pragma define_section forcedata ".data" ".data" ".data" data_mode=far_abs
#pragma section forcedata begin
const int *sPortraitStrokePointCounts[6] = {data_ov007_020cf700, data_ov007_020cfa70,
                                            data_ov007_020cfb84, data_ov007_020cef80,
                                            data_ov007_020cf96c, data_ov007_020cf8ec};
const unsigned char *const *sPortraitStrokeXCoords[6] = {
    data_ov007_020d7ffc, data_ov007_020d8464, data_ov007_020d85fc,
    data_ov007_020d7f0c, data_ov007_020d82e4, data_ov007_020d8164};
const unsigned char *const *sPortraitStrokeYCoords[6] = {
    data_ov007_020d8074, data_ov007_020d84ec, data_ov007_020d8690,
    data_ov007_020d7f5c, data_ov007_020d8364, data_ov007_020d81e4};
const int *const *sPortraitStrokeProgressThresholds[6] = {
    data_ov007_020d80ec, data_ov007_020d8574, data_ov007_020d8724,
    data_ov007_020d7fac, data_ov007_020d83e4, data_ov007_020d8264};
#pragma section forcedata end
