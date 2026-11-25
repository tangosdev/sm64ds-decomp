#include "types.h"
#include "Ov007Work.h"
#include "nitro/fx/fx.h"
#include "Ov007Picker.h"
#include "private/ov007_picker_scene.h"

/* Six portrait drawings, selected by a two-way selector and three-step cycle.
 * Plotting the byte coordinates produces Mario, Luigi and Yoshi portraits.
 * Names are reconstructed from that evidence, not recovered identifiers.
 * The projector uses these as screen X/Y inputs and negates Y; depth is separate.
 * Each stroke is pointCounts[stroke] byte coordinates plus one progress
 * threshold per point; threshold units are unknown.
 */
static const int sPortraitStrokeCounts[6] = {30, 34, 36, 20, 32, 32};
extern const int data_ov007_020cf700[30];
extern const int data_ov007_020cfa70[34];
extern const int data_ov007_020cfb84[37];
extern const int data_ov007_020cef80[20];
extern const int data_ov007_020cf96c[32];
extern const int data_ov007_020cf8ec[32];
const unsigned char data_ov007_020ccca4[1] = {172};

const unsigned char data_ov007_020ccca8[1] = {167};

const unsigned char data_ov007_020cccac[1] = {207};

const unsigned char data_ov007_020cccb0[1] = {141};

const unsigned char data_ov007_020cccb4[1] = {12};

const unsigned char data_ov007_020cccb8[1] = {99};

const unsigned char data_ov007_020cccbc[1] = {99};

const unsigned char data_ov007_020cccc0[1] = {143};

const unsigned char data_ov007_020cccc4[1] = {40};

const unsigned char data_ov007_020cccc8[1] = {99};

const unsigned char data_ov007_020ccccc[2] = {116, 115};

const unsigned char data_ov007_020cccd0[4] = {103, 101, 0, 0};

const unsigned char data_ov007_020cccdc[2] = {113, 117};

const unsigned char data_ov007_020ccce0[2] = {115, 112};

const unsigned char data_ov007_020ccce4[4] = {106, 110, 0, 0};

const unsigned char data_ov007_020cccec[4] = {145, 140, 0, 0};

const unsigned char data_ov007_020cccf4[4] = {103, 107, 0, 0};

const unsigned char data_ov007_020cccfc[2] = {127, 128};

const unsigned char data_ov007_020ccd00[2] = {175, 180};

const unsigned char data_ov007_020ccd04[2] = {166, 167};

const unsigned char data_ov007_020ccd08[3] = {81, 81, 79};

const unsigned char data_ov007_020ccd0c[3] = {106, 110, 119};

const unsigned char data_ov007_020ccd10[3] = {128, 130, 126};

const unsigned char data_ov007_020ccd14[3] = {142, 146, 148};

const unsigned char data_ov007_020ccd18[3] = {234, 236, 233};

const unsigned char data_ov007_020ccd1c[3] = {86, 88, 87};

const unsigned char data_ov007_020ccd20[3] = {89, 89, 89};

const unsigned char data_ov007_020ccd24[3] = {28, 25, 26};

const unsigned char data_ov007_020ccd28[3] = {16, 12, 16};

const unsigned char data_ov007_020ccd2c[3] = {88, 90, 94};

const unsigned char data_ov007_020ccd30[3] = {165, 169, 166};

const unsigned char data_ov007_020ccd34[3] = {97, 101, 105};

const unsigned char data_ov007_020ccd38[3] = {83, 79, 74};

const unsigned char data_ov007_020ccd3c[3] = {115, 111, 107};

const unsigned char data_ov007_020ccd40[3] = {245, 250, 245};

const unsigned char data_ov007_020ccd44[3] = {92, 97, 102};

const unsigned char data_ov007_020ccd48[3] = {60, 56, 58};

const unsigned char data_ov007_020ccd4c[3] = {120, 118, 121};

const unsigned char data_ov007_020ccd50[3] = {101, 100, 100};

const unsigned char data_ov007_020ccd54[4] = {178, 177, 173, 0};

const int data_ov007_020ccd64[1] = {1};

const unsigned char data_ov007_020ccd6c[4] = {64, 60, 62, 64};

const int data_ov007_020ccd70[1] = {1};

const unsigned char data_ov007_020ccd74[4] = {131, 127, 124, 123};

const unsigned char data_ov007_020ccd78[4] = {74, 73, 69, 73};

const unsigned char data_ov007_020ccd80[4] = {91, 93, 97, 101};

const unsigned char data_ov007_020ccd84[4] = {244, 248, 251, 247};

const unsigned char data_ov007_020ccd88[4] = {113, 113, 116, 120};

const unsigned char data_ov007_020ccd8c[4] = {21, 23, 19, 24};

const unsigned char data_ov007_020ccd94[4] = {179, 175, 171, 171};

const int data_ov007_020ccd98[1] = {1};

const unsigned char data_ov007_020ccd9c[4] = {45, 45, 42, 42};

const int data_ov007_020ccda0[1] = {1};

const int data_ov007_020ccda4[1] = {1};

const unsigned char data_ov007_020ccda8[4] = {190, 195, 192, 192};

const unsigned char data_ov007_020ccdb0[4] = {99, 96, 95, 96};

const unsigned char data_ov007_020ccdb4[4] = {54, 52, 56, 60};

const unsigned char data_ov007_020ccdbc[5] = {48, 46, 45, 45, 45};

const unsigned char data_ov007_020ccdc4[5] = {60, 64, 63, 66, 62};

const unsigned char data_ov007_020ccdcc[5] = {147, 148, 148, 151, 148};

const unsigned char data_ov007_020ccdd4[5] = {36, 39, 45, 49, 53};

const unsigned char data_ov007_020ccddc[5] = {57, 61, 62, 58, 55};

const unsigned char data_ov007_020ccde4[5] = {101, 105, 104, 108, 105};

const unsigned char data_ov007_020ccdec[5] = {108, 108, 107, 106, 106};

const unsigned char data_ov007_020ccdf4[5] = {245, 251, 247, 243, 248};

const unsigned char data_ov007_020ccdfc[5] = {189, 193, 189, 186, 191};

const unsigned char data_ov007_020cce04[5] = {75, 71, 67, 72, 75};

const unsigned char data_ov007_020cce0c[5] = {237, 242, 242, 238, 242};

const unsigned char data_ov007_020cce14[5] = {129, 126, 125, 125, 128};

const unsigned char data_ov007_020cce1c[5] = {132, 127, 125, 125, 128};

const unsigned char data_ov007_020cce24[5] = {100, 96, 92, 96, 100};

const unsigned char data_ov007_020cce2c[5] = {222, 223, 223, 221, 225};

const unsigned char data_ov007_020cce34[5] = {213, 214, 215, 216, 214};

const unsigned char data_ov007_020cce3c[5] = {64, 65, 60, 60, 63};

const unsigned char data_ov007_020cce44[5] = {52, 56, 61, 57, 53};

const unsigned char data_ov007_020cce4c[5] = {60, 60, 63, 62, 62};

const unsigned char data_ov007_020cce54[5] = {74, 69, 69, 72, 72};

const unsigned char data_ov007_020cce5c[5] = {222, 223, 221, 223, 223};

const unsigned char data_ov007_020cce64[5] = {131, 131, 127, 130, 132};

const unsigned char data_ov007_020cce6c[5] = {131, 129, 125, 126, 129};

const unsigned char data_ov007_020cce74[5] = {113, 109, 105, 108, 112};

const unsigned char data_ov007_020cce7c[5] = {143, 143, 147, 149, 146};

const unsigned char data_ov007_020cce84[5] = {96, 97, 101, 105, 108};

const unsigned char data_ov007_020cce8c[5] = {52, 57, 52, 48, 53};

const unsigned char data_ov007_020cce94[5] = {96, 99, 103, 107, 104};

const unsigned char data_ov007_020cce9c[6] = {171, 171, 170, 168, 168, 164};

const unsigned char data_ov007_020ccea4[6] = {175, 171, 168, 165, 163, 162};

const unsigned char data_ov007_020cceac[6] = {72, 70, 66, 69, 73, 69};

const unsigned char data_ov007_020cceb4[6] = {105, 110, 106, 102, 111, 112};

const unsigned char data_ov007_020ccebc[6] = {189, 193, 197, 191, 191, 195};

const unsigned char data_ov007_020ccec4[6] = {108, 111, 114, 118, 122, 126};

const unsigned char data_ov007_020ccecc[6] = {43, 44, 43, 41, 45, 45};

const unsigned char data_ov007_020cced4[6] = {105, 105, 104, 103, 102, 103};

const unsigned char data_ov007_020ccedc[6] = {107, 104, 104, 103, 102, 102};

const unsigned char data_ov007_020ccee4[6] = {36, 39, 44, 49, 53, 57};

const unsigned char data_ov007_020cceec[6] = {79, 83, 88, 92, 96, 100};

const unsigned char data_ov007_020ccef4[6] = {38, 43, 48, 53, 58, 54};

const unsigned char data_ov007_020ccefc[6] = {90, 91, 90, 90, 90, 90};

const unsigned char data_ov007_020ccf04[6] = {101, 97, 96, 96, 96, 100};

const unsigned char data_ov007_020ccf0c[6] = {94, 96, 100, 104, 108, 109};

const unsigned char data_ov007_020ccf14[6] = {197, 200, 203, 200, 198, 201};

const unsigned char data_ov007_020ccf1c[7] = {33, 29, 25, 26, 30, 28, 31};

const unsigned char data_ov007_020ccf24[7] = {93, 92, 96, 100, 104, 108, 111};

const unsigned char data_ov007_020ccf2c[7] = {242, 246, 248, 244, 240, 246, 242};

const unsigned char data_ov007_020ccf34[7] = {171, 171, 171, 168, 169, 169, 170};

const unsigned char data_ov007_020ccf3c[7] = {148, 152, 148, 151, 147, 150, 146};

const unsigned char data_ov007_020ccf44[7] = {99, 99, 96, 93, 91, 95, 99};

const unsigned char data_ov007_020ccf4c[7] = {83, 79, 76, 79, 83, 80, 81};

const unsigned char data_ov007_020ccf54[7] = {129, 125, 123, 122, 121, 121, 124};

const unsigned char data_ov007_020ccf5c[7] = {104, 100, 105, 95, 91, 96, 92};

const unsigned char data_ov007_020ccf64[7] = {89, 88, 87, 87, 87, 85, 86};

const unsigned char data_ov007_020ccf6c[7] = {187, 187, 183, 182, 185, 190, 187};

const unsigned char data_ov007_020ccf74[7] = {243, 246, 243, 243, 247, 243, 248};

const unsigned char data_ov007_020ccf7c[7] = {56, 57, 57, 57, 55, 54, 53};

const unsigned char data_ov007_020ccf84[7] = {114, 110, 106, 111, 107, 103, 99};

const unsigned char data_ov007_020ccf8c[7] = {97, 95, 98, 102, 110, 114, 116};

const unsigned char data_ov007_020ccf94[7] = {119, 115, 111, 109, 109, 110, 114};

const unsigned char data_ov007_020ccf9c[7] = {75, 72, 69, 73, 74, 70, 70};

const unsigned char data_ov007_020ccfa4[7] = {116, 115, 115, 111, 113, 109, 109};

const unsigned char data_ov007_020ccfac[7] = {39, 35, 35, 39, 42, 43, 40};

const unsigned char data_ov007_020ccfb4[7] = {94, 104, 110, 113, 107, 102, 106};

const unsigned char data_ov007_020ccfbc[8] = {100, 99, 97, 93, 90, 90, 93, 97};

const unsigned char data_ov007_020ccfc4[8] = {96, 90, 86, 90, 95, 95, 89, 94};

const unsigned char data_ov007_020ccfcc[8] = {85, 90, 91, 90, 90, 88, 85, 89};

const unsigned char data_ov007_020ccfd4[8] = {83, 79, 75, 74, 78, 82, 79, 79};

const int data_ov007_020ccfdc[2] = {0, 19};

const unsigned char data_ov007_020ccfe4[8] = {113, 110, 106, 102, 96, 89, 92, 89};

const int data_ov007_020ccfec[2] = {0, 12};

const unsigned char data_ov007_020ccff4[8] = {169, 170, 173, 176, 171, 166, 162, 159};

const int data_ov007_020ccffc[2] = {0, 7};

const int data_ov007_020cd004[2] = {1, 22};

const unsigned char data_ov007_020cd00c[8] = {80, 85, 89, 94, 98, 102, 106, 110};

const unsigned char data_ov007_020cd014[8] = {133, 138, 142, 145, 146, 146, 144, 141};

const unsigned char data_ov007_020cd01c[8] = {74, 77, 81, 82, 79, 75, 74, 74};

const unsigned char data_ov007_020cd024[8] = {231, 232, 230, 229, 230, 234, 233, 233};

const unsigned char data_ov007_020cd02c[8] = {21, 16, 12, 8, 8, 13, 17, 14};

const unsigned char data_ov007_020cd034[8] = {90, 91, 90, 90, 90, 89, 89, 89};

const unsigned char data_ov007_020cd044[8] = {51, 48, 48, 53, 56, 56, 51, 47};

const unsigned char data_ov007_020cd04c[8] = {121, 117, 113, 112, 113, 117, 120, 116};

const unsigned char data_ov007_020cd054[8] = {100, 99, 102, 106, 110, 112, 108, 109};

const unsigned char data_ov007_020cd05c[8] = {232, 234, 235, 237, 233, 231, 230, 234};

const int data_ov007_020cd064[2] = {1, 7};

const unsigned char data_ov007_020cd06c[8] = {50, 46, 43, 42, 45, 49, 53, 57};

const unsigned char data_ov007_020cd074[8] = {134, 132, 129, 125, 122, 120, 119, 118};

const unsigned char data_ov007_020cd07c[9] = {208, 209, 207, 205, 204, 205, 208, 209, 206};

const unsigned char data_ov007_020cd088[9] = {105, 109, 113, 117, 118, 116, 112, 109, 108};

const unsigned char data_ov007_020cd094[9] = {24, 20, 16, 12, 16, 21, 25, 21, 18};

const unsigned char data_ov007_020cd0a0[9] = {55, 54, 53, 53, 57, 59, 58, 58, 59};

const unsigned char data_ov007_020cd0ac[9] = {113, 117, 121, 125, 128, 131, 133, 135, 135};

const unsigned char data_ov007_020cd0b8[9] = {139, 140, 140, 138, 135, 132, 128, 124, 120};

const unsigned char data_ov007_020cd0c4[9] = {146, 143, 143, 146, 150, 154, 153, 149, 145};

const unsigned char data_ov007_020cd0d0[9] = {136, 140, 140, 144, 140, 143, 139, 141, 137};

const unsigned char data_ov007_020cd0dc[9] = {97, 95, 91, 89, 90, 86, 88, 84, 87};

const unsigned char data_ov007_020cd0e8[9] = {104, 108, 113, 117, 116, 112, 108, 104, 100};

const unsigned char data_ov007_020cd0f4[9] = {94, 92, 93, 97, 101, 105, 109, 112, 111};

const unsigned char data_ov007_020cd100[9] = {131, 127, 123, 120, 119, 119, 121, 124, 128};

const unsigned char data_ov007_020cd10c[10] = {107, 104, 105, 108, 112, 116, 120, 124, 128, 127};

const unsigned char data_ov007_020cd118[10] = {184, 181, 177, 174, 172, 171, 170, 170, 172, 176};

const unsigned char data_ov007_020cd124[10] = {133, 133, 131, 127, 122, 120, 119, 122, 126, 130};

const unsigned char data_ov007_020cd130[10] = {87, 83, 79, 76, 77, 81, 85, 81, 79, 81};

const unsigned char data_ov007_020cd13c[10] = {136, 134, 130, 126, 123, 121, 119, 118, 118, 118};

const unsigned char data_ov007_020cd148[10] = {37, 33, 33, 35, 38, 42, 46, 50, 54, 58};

const unsigned char data_ov007_020cd154[10] = {141, 145, 147, 146, 142, 139, 139, 140, 144, 140};

const unsigned char data_ov007_020cd160[10] = {111, 108, 104, 100, 99, 102, 106, 110, 109, 108};

const unsigned char data_ov007_020cd16c[10] = {95, 91, 95, 99, 95, 90, 95, 99, 98, 94};

const unsigned char data_ov007_020cd178[10] = {141, 138, 129, 125, 117, 119, 123, 127, 135, 141};

const unsigned char data_ov007_020cd184[10] = {145, 148, 150, 150, 148, 152, 154, 153, 153, 147};

const unsigned char data_ov007_020cd190[10] = {207, 210, 205, 204, 208, 210, 206, 205, 210, 210};

const unsigned char data_ov007_020cd19c[11] = {174, 176, 180, 179, 175, 171,
                                               170, 171, 167, 163, 159};

const unsigned char data_ov007_020cd1a8[11] = {134, 138, 138, 142, 143, 141,
                                               145, 149, 148, 146, 145};

const unsigned char data_ov007_020cd1b4[11] = {105, 101, 96, 92, 88, 85, 88, 92, 96, 100, 104};

const unsigned char data_ov007_020cd1c0[11] = {100, 102, 101, 100, 100, 97, 94, 92, 92, 92, 93};

const unsigned char data_ov007_020cd1cc[11] = {106, 108, 111, 115, 119, 123,
                                               127, 131, 135, 138, 141};

const unsigned char data_ov007_020cd1d8[11] = {142, 143, 147, 149, 151, 151,
                                               150, 148, 145, 142, 139};

const unsigned char data_ov007_020cd1e4[11] = {174, 176, 178, 179, 179, 178,
                                               177, 178, 182, 186, 187};

const unsigned char data_ov007_020cd1f0[11] = {60, 64, 68, 72, 76, 80, 84, 88, 89, 91, 95};

const unsigned char data_ov007_020cd1fc[12] = {142, 143, 142, 140, 138, 142,
                                               146, 150, 152, 151, 148, 144};

const unsigned char data_ov007_020cd208[12] = {118, 117, 115, 114, 112, 109,
                                               108, 104, 102, 101, 100, 99};

const int data_ov007_020cd214[3] = {1, 13, 20};

const int data_ov007_020cd220[3] = {0, 19, 22};

const int data_ov007_020cd22c[3] = {0, 7, 11};

const int data_ov007_020cd238[3] = {1, 6, 9};

const int data_ov007_020cd244[3] = {1, 7, 15};

const int data_ov007_020cd250[3] = {1, 11, 37};

const int data_ov007_020cd25c[3] = {1, 12, 18};

const unsigned char data_ov007_020cd268[12] = {142, 146, 149, 150, 149, 147,
                                               143, 139, 136, 136, 137, 140};

const unsigned char data_ov007_020cd274[12] = {113, 112, 109, 105, 101, 97,
                                               96,  99,  102, 106, 110, 113};

const int data_ov007_020cd280[3] = {1, 8, 14};

const unsigned char data_ov007_020cd28c[12] = {56, 52, 48, 44, 40, 43, 47, 44, 40, 44, 48, 52};

const unsigned char data_ov007_020cd298[12] = {96,  100, 104, 108, 112, 114,
                                               115, 114, 110, 106, 103, 101};

const int data_ov007_020cd2a4[3] = {1, 18, 24};

const unsigned char data_ov007_020cd2b0[12] = {118, 114, 110, 106, 102, 99,
                                               99,  103, 107, 111, 115, 117};

const unsigned char data_ov007_020cd2bc[12] = {87, 79, 77, 76, 77, 80, 84, 83, 81, 81, 82, 86};

const unsigned char data_ov007_020cd2c8[12] = {167, 168, 168, 165, 169, 169,
                                               173, 173, 173, 172, 172, 171};

const unsigned char data_ov007_020cd2d4[12] = {88,  92,  101, 110, 113, 118,
                                               116, 108, 103, 99,  95,  91};

const int data_ov007_020cd2e0[3] = {0, 9, 14};

const unsigned char data_ov007_020cd2ec[12] = {90, 90, 92, 91, 88, 87, 84, 84, 85, 85, 86, 86};

const unsigned char data_ov007_020cd2f8[12] = {88,  98,  106, 110, 114, 118,
                                               115, 111, 106, 102, 98,  94};

const unsigned char data_ov007_020cd304[13] = {54, 55, 51, 46, 43, 47, 51, 47, 43, 47, 52, 56, 52};

const unsigned char data_ov007_020cd314[13] = {105, 101, 97,  95,  96,  99, 103,
                                               112, 117, 121, 129, 130, 128};

const unsigned char data_ov007_020cd324[13] = {118, 114, 113, 112, 109, 108, 105,
                                               102, 101, 100, 99,  97,  97};

const unsigned char data_ov007_020cd334[13] = {138, 144, 147, 151, 155, 158, 159,
                                               155, 151, 146, 142, 139, 137};

const unsigned char data_ov007_020cd344[13] = {88, 80, 77, 77, 79, 82, 86, 83, 81, 80, 82, 86, 90};

const unsigned char data_ov007_020cd354[13] = {172, 174, 177, 181, 184, 185, 184,
                                               184, 182, 180, 177, 173, 169};

const unsigned char data_ov007_020cd364[13] = {106, 102, 98,  97,  100, 104, 108,
                                               112, 120, 124, 127, 128, 127};

const unsigned char data_ov007_020cd374[13] = {84, 84, 83, 79, 75, 72, 71, 71, 72, 74, 80, 84, 88};

const unsigned char data_ov007_020cd384[14] = {91,  95,  99,  103, 106, 107, 110,
                                               114, 116, 120, 124, 124, 121, 117};

const unsigned char data_ov007_020cd394[14] = {153, 150, 146, 142, 139, 139, 138,
                                               140, 144, 148, 152, 155, 156, 156};

const unsigned char data_ov007_020cd3a4[14] = {58, 54, 53, 54, 57, 61, 65,
                                               69, 70, 69, 67, 63, 59, 55};

const unsigned char data_ov007_020cd3b4[14] = {108, 103, 99, 95,  91,  87,  88,
                                               91,  95,  99, 103, 107, 109, 109};

const unsigned char data_ov007_020cd3c4[14] = {88,  91,  95,  99,  103, 107, 111,
                                               115, 115, 115, 116, 113, 110, 106};

const unsigned char data_ov007_020cd3d4[14] = {103, 102, 98, 94, 90, 86, 82,
                                               77,  72,  69, 65, 62, 60, 58};

const unsigned char data_ov007_020cd3e4[14] = {180, 185, 187, 189, 189, 189, 187,
                                               186, 182, 179, 176, 172, 168, 164};

const unsigned char data_ov007_020cd3f4[14] = {51, 53, 54, 56, 56, 56, 56,
                                               55, 51, 49, 49, 49, 50, 51};

const unsigned char data_ov007_020cd404[14] = {65, 60, 55, 50, 45, 41, 37,
                                               33, 33, 38, 45, 51, 57, 63};

const unsigned char data_ov007_020cd414[14] = {154, 156, 156, 156, 155, 154, 152,
                                               150, 146, 150, 154, 157, 161, 164};

const unsigned char data_ov007_020cd424[14] = {102, 102, 103, 102, 102, 100, 96,
                                               93,  92,  91,  92,  93,  97,  101};

const unsigned char data_ov007_020cd434[14] = {157, 160, 160, 158, 155, 151, 148,
                                               149, 153, 155, 156, 160, 163, 163};

const unsigned char data_ov007_020cd444[15] = {85, 81, 78, 77, 77, 78, 82, 86,
                                               90, 86, 82, 79, 79, 81, 84};

const unsigned char data_ov007_020cd454[15] = {102, 100, 98, 96,  95,  94,  94, 94,
                                               94,  96,  99, 102, 106, 110, 117};

const unsigned char data_ov007_020cd464[15] = {143, 147, 151, 155, 159, 163, 167, 171,
                                               175, 179, 182, 185, 186, 187, 187};

const unsigned char data_ov007_020cd474[15] = {136, 134, 131, 127, 123, 119, 117, 115,
                                               114, 115, 117, 120, 124, 128, 131};

const int data_ov007_020cd484[4] = {1, 14, 21, 31};

const int data_ov007_020cd494[4] = {1, 8, 14, 20};

const int data_ov007_020cd4a4[4] = {1, 9, 14, 18};

const int data_ov007_020cd4b4[4] = {1, 14, 25, 40};

const int data_ov007_020cd4c4[4] = {1, 7, 10, 12};

const unsigned char data_ov007_020cd4d4[16] = {147, 151, 154, 155, 155, 153, 149, 145,
                                               141, 138, 136, 136, 137, 139, 143, 147};

const unsigned char data_ov007_020cd4e4[16] = {118, 116, 112, 108, 100, 92,  89,  89,
                                               91,  94,  98,  106, 110, 114, 116, 117};

const unsigned char data_ov007_020cd4f4[16] = {111, 108, 108, 104, 106, 103, 103, 105,
                                               99,  101, 104, 100, 100, 97,  98,  94};

const unsigned char data_ov007_020cd504[16] = {140, 143, 139, 141, 133, 137, 133, 129,
                                               135, 130, 126, 129, 124, 127, 123, 124};

const int data_ov007_020cd514[4] = {1, 20, 28, 35};

const unsigned char data_ov007_020cd524[17] = {62, 60, 56, 52, 48, 44, 40, 37, 35,
                                               34, 34, 35, 41, 49, 57, 63, 63};

const unsigned char data_ov007_020cd538[17] = {141, 145, 148, 149, 149, 147, 144, 141, 137,
                                               133, 125, 121, 113, 109, 109, 117, 121};

const unsigned char data_ov007_020cd54c[17] = {161, 158, 162, 166, 163, 159, 164, 168, 165,
                                               162, 158, 162, 167, 165, 170, 167, 172};

const unsigned char data_ov007_020cd560[17] = {135, 139, 136, 135, 139, 142, 138, 137, 141,
                                               144, 146, 143, 139, 144, 141, 145, 142};

const unsigned char data_ov007_020cd574[17] = {120, 121, 119, 117, 113, 109, 103, 101, 102,
                                               102, 104, 107, 111, 115, 118, 120, 120};

const unsigned char data_ov007_020cd588[17] = {107, 103, 95,  91,  89,  88,  94,  102, 106,
                                               110, 114, 117, 118, 116, 113, 109, 105};

const unsigned char data_ov007_020cd59c[18] = {
    140, 137, 137, 141, 136, 141, 137, 141, 145, 141, 137, 141, 145, 141, 144, 145, 143, 139,
};

const unsigned char data_ov007_020cd5b0[18] = {
    121, 125, 128, 129, 130, 131, 131, 129, 126, 122, 119, 117, 116, 116, 116, 117, 120, 124,
};

const unsigned char data_ov007_020cd5c4[18] = {113, 114, 111, 107, 103, 99,  94,  90,  87,
                                               87,  90,  94,  99,  103, 107, 111, 114, 113};

const unsigned char data_ov007_020cd5d8[18] = {
    174, 174, 170, 170, 168, 167, 164, 164, 165, 163, 162, 162, 163, 160, 157, 161, 165, 167,
};

const unsigned char data_ov007_020cd5ec[19] = {
    168, 171, 172, 174, 174, 173, 173, 171, 169, 166, 161, 157, 153, 148, 144, 140, 137, 136, 135,
};

const unsigned char data_ov007_020cd600[19] = {71, 66, 62, 58, 54, 50, 46, 42, 38, 34,
                                               31, 29, 28, 28, 30, 32, 35, 39, 43};

const unsigned char data_ov007_020cd614[19] = {
    118, 121, 123, 123, 124, 123, 122, 119, 115, 111, 107, 103, 101, 99, 97, 96, 96, 96, 98,
};

const unsigned char data_ov007_020cd628[19] = {64, 60, 56, 52, 48, 43, 38, 35, 33, 32,
                                               33, 35, 39, 43, 47, 51, 55, 59, 63};

const unsigned char data_ov007_020cd63c[19] = {
    169, 165, 161, 158, 161, 164, 161, 158, 158, 161, 158, 155, 157, 158, 159, 157, 155, 152, 150,
};

const unsigned char data_ov007_020cd650[19] = {101, 99, 97,  93,  99,  103, 99,  96,  101, 106,
                                               102, 98, 103, 107, 111, 107, 112, 117, 113};

const unsigned char data_ov007_020cd664[19] = {
    147, 151, 150, 148, 150, 153, 152, 150, 152, 154, 154, 153, 154, 155, 158, 159, 155, 153, 156,
};

const unsigned char data_ov007_020cd678[19] = {
    138, 140, 136, 132, 137, 134, 130, 126, 135, 131, 127, 123, 127, 122, 119, 123, 126, 130, 126,
};

const unsigned char data_ov007_020cd68c[20] = {56, 47, 42, 41, 45, 42, 39, 47, 51, 55,
                                               53, 49, 45, 47, 50, 47, 43, 52, 56, 56};

const int data_ov007_020cd6a0[5] = {1, 15, 21, 35, 47};

const int data_ov007_020cd6b4[5] = {1, 7, 11, 13, 17};

const unsigned char data_ov007_020cd6c8[20] = {
    114, 118, 121, 123, 124, 124, 123, 121, 118, 114, 110, 106, 102, 99, 96, 95, 94, 94, 96, 99,
};

const unsigned char data_ov007_020cd6dc[20] = {64, 61, 58, 54, 50, 46, 42, 38, 35, 34,
                                               34, 34, 36, 39, 43, 47, 51, 55, 59, 62};

const int data_ov007_020cd6f0[5] = {1, 10, 14, 19, 43};

const int data_ov007_020cd704[5] = {1, 14, 21, 28, 37};

const unsigned char data_ov007_020cd718[20] = {104, 100, 96, 92, 88, 84,  81,  78,  77,  77,
                                               80,  84,  88, 92, 97, 101, 106, 109, 113, 116};

const unsigned char data_ov007_020cd72c[20] = {
    114, 111, 111, 110, 110, 113, 116, 120, 124, 128,
    131, 134, 135, 135, 135, 134, 132, 129, 127, 123,
};

const unsigned char data_ov007_020cd740[20] = {
    144, 147, 149, 152, 156, 159, 161, 164, 164, 165,
    164, 162, 160, 156, 153, 150, 147, 144, 142, 138,
};

const unsigned char data_ov007_020cd754[20] = {
    171, 166, 162, 159, 155, 151, 147, 143, 139, 134,
    130, 126, 122, 118, 114, 110, 106, 102, 98,  94,
};

const unsigned char data_ov007_020cd768[20] = {95, 95, 94, 91, 89, 86, 84, 81, 78, 76,
                                               74, 72, 71, 70, 69, 69, 69, 69, 70, 71};

const int data_ov007_020cd77c[5] = {1, 6, 9, 12, 15};

const int data_ov007_020cd790[5] = {1, 12, 15, 17, 20};

const int data_ov007_020cd7a4[5] = {1, 10, 13, 27, 31};

const unsigned char data_ov007_020cd7b8[20] = {
    133, 130, 127, 124, 128, 130, 132, 129, 125, 122,
    118, 124, 128, 131, 132, 132, 133, 132, 132, 133,
};

const unsigned char data_ov007_020cd7cc[20] = {
    125, 130, 133, 136, 133, 129, 125, 129, 133, 136,
    139, 136, 133, 129, 125, 126, 122, 126, 130, 126,
};

const int data_ov007_020cd7e0[5] = {1, 7, 12, 14, 18};

const int data_ov007_020cd7f4[5] = {1, 13, 18, 29, 34};

const int data_ov007_020cd808[5] = {1, 8, 11, 21, 26};

const int data_ov007_020cd81c[5] = {1, 15, 27, 31, 46};

const unsigned char data_ov007_020cd830[20] = {
    92, 95, 99, 102, 104, 107, 111, 115, 119, 124, 128, 132, 136, 139, 142, 145, 148, 152, 156, 159,
};

const int data_ov007_020cd844[5] = {1, 8, 13, 17, 20};

const int data_ov007_020cd858[5] = {1, 24, 38, 57, 79};

const int data_ov007_020cd86c[5] = {1, 12, 23, 29, 39};

const unsigned char data_ov007_020cd880[20] = {
    142, 138, 137, 133, 130, 126, 123, 119, 117, 116,
    120, 122, 123, 127, 130, 133, 134, 138, 139, 143,
};

const unsigned char data_ov007_020cd894[21] = {
    149, 153, 156, 158, 160, 162, 162, 162, 162, 161, 158,
    155, 153, 152, 150, 148, 145, 142, 139, 136, 132,
};

const unsigned char data_ov007_020cd8ac[21] = {
    96,  98,  101, 105, 109, 113, 117, 121, 125, 129, 133,
    136, 140, 144, 148, 152, 155, 159, 162, 165, 166,
};

const unsigned char data_ov007_020cd8c4[22] = {
    156, 157, 159, 161, 164, 166, 170, 174, 178, 182, 183,
    184, 183, 182, 181, 179, 176, 173, 169, 165, 161, 159,
};

const unsigned char data_ov007_020cd8dc[22] = {
    122, 118, 114, 110, 107, 103, 100, 98,  98,  99,  103,
    107, 111, 115, 119, 123, 127, 131, 134, 135, 135, 131,
};

const unsigned char data_ov007_020cd8f4[22] = {
    173, 175, 177, 178, 176, 175, 175, 176, 177, 175, 173,
    172, 173, 174, 175, 173, 171, 170, 170, 168, 172, 174,
};

const unsigned char data_ov007_020cd90c[22] = {62, 66, 71, 75, 69, 65, 69, 75, 79, 74, 69,
                                               65, 69, 73, 78, 74, 68, 64, 69, 74, 77, 81};

const unsigned char data_ov007_020cd924[22] = {
    132, 132, 131, 130, 127, 123, 119, 116, 115, 114, 114,
    114, 115, 118, 122, 126, 129, 131, 132, 132, 133, 132,
};

const unsigned char data_ov007_020cd93c[22] = {
    101, 97,  93,  89,  86,  87,  90,  93,  97,  101, 105,
    109, 113, 116, 118, 117, 114, 110, 106, 102, 98,  94,
};

const unsigned char data_ov007_020cd954[23] = {
    163, 165, 166, 168, 170, 172, 176, 180, 184, 188, 190, 191,
    191, 191, 190, 188, 186, 183, 180, 176, 172, 168, 165,
};

const unsigned char data_ov007_020cd96c[23] = {
    119, 115, 111, 107, 103, 99,  96,  96,  96,  99,  103, 107,
    111, 115, 119, 123, 127, 130, 133, 135, 136, 133, 128,
};

const unsigned char data_ov007_020cd984[24] = {
    99, 96, 93, 90, 87, 86, 86, 85, 83, 81, 79, 78, 76, 74, 73, 72, 70, 71, 71, 69, 69, 69, 70, 72,
};

const int data_ov007_020cd99c[6] = {0, 15, 22, 24, 32, 38};

const unsigned char data_ov007_020cd9cc[24] = {
    172, 168, 165, 161, 157, 153, 149, 145, 141, 137, 132, 128,
    124, 120, 116, 112, 108, 104, 100, 96,  92,  88,  84,  80,
};

const int data_ov007_020cd9fc[6] = {1, 12, 17, 21, 25, 31};

const int data_ov007_020cda44[6] = {1, 10, 12, 15, 20, 26};

const int data_ov007_020cda74[6] = {1, 5, 8, 11, 16, 18};

const int data_ov007_020cda8c[6] = {1, 5, 7, 12, 14, 17};

const int data_ov007_020cdaa4[6] = {1, 15, 22, 25, 28, 33};

const int data_ov007_020cdad4[6] = {1, 11, 12, 13, 14, 822};

const int data_ov007_020cdaec[6] = {1, 8, 11, 15, 18, 23};

const unsigned char data_ov007_020cdb4c[24] = {
    87,  86,  86,  87,  90,  96,  100, 104, 110, 118, 122, 126,
    130, 138, 146, 149, 152, 156, 160, 166, 169, 171, 171, 170,
};

const unsigned char data_ov007_020cdb64[24] = {
    121, 125, 133, 137, 141, 147, 150, 152, 158, 164, 164, 166,
    166, 164, 160, 156, 153, 151, 148, 142, 139, 131, 123, 119,
};

const unsigned char data_ov007_020cdb7c[25] = {
    104, 100, 96, 92, 88, 84, 80,  76,  73,  72,  71,  71,  73,
    75,  79,  83, 87, 92, 96, 100, 104, 108, 111, 114, 114,
};

const unsigned char data_ov007_020cdb98[25] = {
    109, 106, 104, 103, 103, 104, 105, 107, 111, 115, 119, 123, 127,
    131, 134, 136, 136, 137, 137, 136, 134, 132, 129, 125, 121,
};

const unsigned char data_ov007_020cdbb4[25] = {
    115, 119, 123, 127, 131, 134, 138, 138, 138, 135, 132, 127, 123,
    120, 116, 112, 108, 104, 100, 97,  94,  90,  86,  84,  80,
};

const unsigned char data_ov007_020cdbd0[25] = {
    123, 125, 124, 124, 123, 119, 117, 121, 125, 129, 132, 133, 135,
    138, 139, 136, 139, 140, 139, 136, 139, 141, 140, 136, 137,
};

const unsigned char data_ov007_020cdbec[26] = {
    216, 219, 216, 216, 216, 214, 212, 213, 213, 210, 209, 210, 211,
    211, 210, 210, 209, 210, 213, 213, 214, 216, 219, 217, 213, 211,
};

const unsigned char data_ov007_020cdc08[26] = {
    28, 34, 30, 35, 40, 36, 31, 36, 40, 35, 29, 34, 40,
    45, 41, 35, 40, 46, 43, 36, 40, 46, 43, 47, 41, 45,
};

const unsigned char data_ov007_020cdc24[26] = {
    114, 117, 121, 125, 129, 133, 136, 137, 136, 134, 131, 127, 123,
    119, 115, 111, 107, 103, 100, 97,  93,  89,  85,  82,  86,  90,
};

const unsigned char data_ov007_020cdc40[26] = {
    124, 125, 126, 126, 125, 123, 120, 124, 128, 132, 135, 138, 139,
    139, 138, 137, 135, 134, 138, 141, 142, 142, 142, 139, 138, 137,
};

const unsigned char data_ov007_020cdc5c[26] = {
    163, 163, 161, 159, 156, 154, 150, 146, 143, 139, 135, 131, 127,
    122, 118, 114, 110, 106, 103, 100, 98,  96,  94,  93,  92,  91,
};

const unsigned char data_ov007_020cdc78[26] = {
    59, 55, 51, 47, 44, 40, 36, 33, 30, 27, 25, 24, 24,
    23, 24, 25, 27, 29, 33, 37, 41, 45, 49, 53, 57, 61,
};

const unsigned char data_ov007_020cdc94[26] = {
    152, 153, 150, 152, 153, 150, 148, 144, 147, 142, 147, 143, 140,
    143, 147, 150, 150, 150, 147, 143, 140, 142, 139, 139, 140, 144,
};

const unsigned char data_ov007_020cdcb0[26] = {
    59, 64, 67, 63, 59, 63, 67, 69, 65, 67, 63, 66, 63,
    66, 66, 63, 58, 63, 66, 64, 61, 57, 60, 64, 68, 68,
};

const unsigned char data_ov007_020cdccc[27] = {
    157, 158, 159, 158, 157, 155, 154, 153, 151, 148, 144, 140, 137, 134,
    133, 132, 131, 132, 132, 133, 135, 139, 143, 147, 151, 155, 158,
};

const unsigned char data_ov007_020cdce8[27] = {
    78, 73, 69, 65, 61, 57, 53, 49, 45, 42, 41, 41, 44, 48,
    52, 56, 60, 64, 68, 72, 76, 77, 79, 79, 77, 75, 72,
};

const int data_ov007_020cdd04[7] = {1, 6, 8, 13, 16, 21, 26};

const int data_ov007_020cdd20[7] = {1, 6, 8, 11, 13, 18, 26};

const int data_ov007_020cdd3c[7] = {0, 16, 21, 28, 31, 35, 41};

const int data_ov007_020cdd58[7] = {0, 8, 10, 13, 17, 19, 25};

const unsigned char data_ov007_020cdd74[28] = {
    207, 207, 208, 207, 207, 207, 209, 211, 215, 219, 222, 221, 220, 218,
    220, 224, 229, 233, 234, 234, 232, 229, 225, 221, 217, 213, 209, 205,
};

const unsigned char data_ov007_020cdd90[28] = {
    53, 48, 44, 40, 36, 31, 26, 22, 21, 22, 25, 30, 34, 38,
    34, 32, 31, 34, 38, 42, 46, 49, 51, 52, 52, 52, 54, 56,
};

const int data_ov007_020cddac[7] = {1, 12, 17, 21, 26, 32, 42};

const int data_ov007_020cddc8[7] = {1, 9, 16, 21, 26, 40, 45};

const int data_ov007_020cdde4[7] = {1, 17, 29, 38, 47, 55, 63};

const int data_ov007_020cde00[7] = {1, 9, 11, 18, 23, 24, 25};

const int data_ov007_020cde1c[7] = {0, 14, 21, 27, 40, 48, 59};

const int data_ov007_020cde38[7] = {1, 7, 10, 14, 16, 18, 23};

const unsigned char data_ov007_020cde54[30] = {
    159, 155, 152, 148, 144, 140, 136, 132, 128, 124, 120, 116, 111, 107, 102,
    98,  94,  90,  86,  82,  78,  74,  71,  69,  70,  73,  77,  81,  85,  89,
};

const unsigned char data_ov007_020cde74[30] = {
    86, 85, 82, 79, 77, 76, 74, 73, 72, 70, 68, 67, 66, 64, 64,
    63, 63, 63, 62, 62, 63, 65, 68, 72, 76, 79, 81, 82, 83, 84,
};

const unsigned char data_ov007_020cde94[31] = {
    166, 166, 162, 158, 159, 162, 164, 165, 164, 162, 159, 160, 162, 163, 161, 158,
    155, 157, 158, 159, 160, 157, 155, 155, 157, 154, 152, 148, 150, 152, 149,
};

const unsigned char data_ov007_020cdeb4[31] = {
    104, 99,  93,  91,  96,  101, 106, 110, 106, 102, 97,  104, 110, 114, 109, 104,
    101, 107, 111, 115, 119, 115, 109, 115, 121, 117, 113, 108, 113, 118, 115,
};

const unsigned char data_ov007_020cded4[31] = {
    171, 167, 162, 159, 155, 152, 147, 142, 138, 133, 129, 125, 121, 117, 113, 109,
    104, 100, 96,  91,  86,  82,  78,  74,  70,  68,  68,  72,  76,  80,  84,
};

const unsigned char data_ov007_020cdef4[31] = {
    94, 93, 89, 86, 83, 80, 77, 74, 71, 68, 67, 65, 64, 63, 62, 62,
    61, 61, 61, 62, 63, 64, 65, 68, 71, 75, 79, 81, 81, 82, 81,
};

const unsigned char data_ov007_020cdf14[31] = {
    168, 171, 173, 175, 177, 180, 184, 187, 189, 190, 191, 191, 191, 190, 189, 188,
    186, 183, 180, 177, 173, 169, 165, 160, 155, 151, 147, 143, 139, 135, 131,
};

const unsigned char data_ov007_020cdf34[31] = {
    71,  74,  78,  82,  86,  89,  92,  96,  100, 104, 109, 113, 117, 121, 125, 129,
    133, 137, 140, 143, 145, 148, 149, 151, 152, 152, 153, 152, 150, 149, 148,
};

const int data_ov007_020cdf54[8] = {1, 14, 24, 33, 46, 50, 61, 71};

const int data_ov007_020cdf74[8] = {1, 7, 9, 12, 14, 19, 21, 27};

const int data_ov007_020cdf94[8] = {0, 9, 21, 24, 35, 49, 56, 63};

const int data_ov007_020cdfb4[8] = {1, 16, 26, 36, 45, 53, 59, 63};

const int data_ov007_020cdfd4[8] = {1, 9, 12, 15, 20, 25, 30, 38};

const int data_ov007_020cdff4[8] = {1, 11, 16, 24, 30, 33, 38, 64};

const int data_ov007_020ce014[8] = {1, 9, 12, 15, 18, 20, 23, 25};

const int data_ov007_020ce034[8] = {0, 9, 16, 22, 30, 44, 50, 66};

const int data_ov007_020ce054[8] = {1, 9, 11, 13, 25, 29, 31, 41};

const unsigned char data_ov007_020ce074[33] = {
    135, 135, 131, 131, 132, 129, 125, 127, 127, 125, 122, 123, 123, 120, 118, 118, 120,
    120, 116, 114, 115, 115, 112, 111, 111, 111, 108, 108, 104, 102, 99,  95,  91,
};

const unsigned char data_ov007_020ce098[33] = {
    119, 125, 122, 126, 130, 125, 123, 128, 132, 128, 125, 130, 134, 129, 124, 128, 132,
    137, 131, 126, 132, 136, 132, 128, 133, 137, 132, 137, 136, 140, 137, 141, 140,
};

const unsigned char data_ov007_020ce0bc[35] = {
    134, 133, 132, 131, 131, 131, 128, 127, 125, 125, 126, 127, 124, 123, 122, 123, 123, 120,
    119, 117, 118, 118, 118, 118, 116, 115, 114, 114, 115, 115, 113, 111, 109, 110, 106,
};

const unsigned char data_ov007_020ce0e0[35] = {
    123, 127, 131, 135, 130, 126, 130, 135, 139, 135, 131, 127, 133, 138, 134, 130, 126, 131,
    136, 140, 136, 132, 128, 124, 128, 133, 138, 133, 128, 124, 128, 133, 138, 133, 135,
};

const int data_ov007_020ce104[9] = {1, 19, 24, 29, 34, 40, 47, 57, 62};

const int data_ov007_020ce128[9] = {1, 27, 33, 42, 47, 56, 63, 71, 81};

const int data_ov007_020ce14c[9] = {1, 11, 41, 45, 52, 77, 87, 96, 101};

const int data_ov007_020ce170[9] = {1, 10, 13, 29, 37, 48, 50, 52, 54};

const int data_ov007_020ce194[9] = {1, 10, 12, 15, 24, 27, 30, 33, 37};

const int data_ov007_020ce1b8[9] = {1, 14, 25, 36, 45, 54, 63, 76, 85};

const unsigned char data_ov007_020ce1dc[37] = {
    226, 228, 228, 227, 223, 221, 217, 215, 210, 205, 201, 199, 201, 204, 206, 207, 205, 203, 202,
    206, 210, 214, 218, 221, 225, 227, 227, 227, 226, 230, 234, 238, 241, 239, 235, 231, 227,
};

const unsigned char data_ov007_020ce204[37] = {
    47, 41, 36, 31, 30, 34, 38, 42, 41, 40, 40, 44, 48, 52, 56, 60, 64, 68, 72,
    72, 69, 68, 71, 76, 78, 74, 69, 65, 61, 62, 61, 60, 57, 53, 52, 49, 47,
};

const unsigned char data_ov007_020ce22c[38] = {
    186, 188, 189, 189, 189, 187, 185, 183, 180, 177, 174, 171, 167, 163, 160, 157, 154, 150, 147,
    143, 139, 135, 131, 126, 122, 118, 114, 110, 106, 102, 99,  95,  93,  91,  90,  88,  88,  88,
};

const unsigned char data_ov007_020ce254[38] = {
    97, 93, 88, 84, 80, 76, 72, 67, 63, 60, 57, 54, 51, 48, 45, 42, 39, 36, 33,
    31, 29, 28, 28, 28, 28, 28, 28, 28, 30, 31, 34, 37, 41, 45, 49, 53, 57, 61,
};

const int data_ov007_020ce27c[10] = {1, 9, 14, 18, 23, 28, 33, 38, 45, 49};

const int data_ov007_020ce2a4[10] = {1, 7, 12, 18, 23, 26, 30, 37, 41, 45};

const int data_ov007_020ce2cc[10] = {1, 20, 31, 38, 46, 53, 61, 68, 76, 83};

const int data_ov007_020ce2f4[10] = {0, 8, 14, 20, 27, 36, 42, 48, 59, 69};

const int data_ov007_020ce31c[10] = {0, 17, 28, 34, 47, 66, 70, 74, 84, 93};

const int data_ov007_020ce344[10] = {1, 10, 16, 18, 23, 25, 28, 29, 34, 36};

const unsigned char data_ov007_020ce36c[42] = {
    227, 230, 230, 230, 229, 228, 226, 230, 232, 233, 232, 229, 226, 225,
    226, 227, 227, 226, 224, 224, 225, 225, 223, 222, 221, 219, 220, 221,
    221, 219, 217, 217, 217, 215, 214, 213, 214, 214, 212, 211, 210, 206,
};

const unsigned char data_ov007_020ce398[42] = {
    35, 38, 42, 47, 43, 38, 34, 35, 39, 43, 47, 44, 40, 35, 41, 47, 51, 45, 39, 44, 49,
    53, 48, 44, 40, 36, 42, 48, 52, 47, 42, 49, 53, 47, 43, 47, 52, 57, 51, 46, 50, 53,
};

const int data_ov007_020ce3c4[11] = {1, 15, 18, 27, 32, 36, 49, 60, 65, 69, 88};

const int data_ov007_020ce3f0[11] = {1, 23, 27, 31, 35, 38, 41, 46, 50, 54, 57};

const int data_ov007_020ce41c[11] = {1, 11, 16, 20, 24, 30, 37, 56, 65, 78, 86};

const int data_ov007_020ce448[11] = {1, 13, 19, 24, 29, 40, 50, 58, 63, 69, 76};

const int data_ov007_020ce474[12] = {1, 13, 17, 21, 34, 41, 45, 49, 54, 59, 66, 75};

const int data_ov007_020ce4a4[12] = {1, 8, 11, 14, 18, 35, 37, 49, 55, 69, 72, 76};

const int data_ov007_020ce4d4[12] = {0, 10, 16, 22, 29, 37, 46, 52, 60, 65, 70, 76};

const int data_ov007_020ce504[12] = {0, 15, 20, 26, 32, 38, 41, 59, 63, 67, 71, 77};

const int data_ov007_020ce534[12] = {0, 6, 12, 17, 34, 37, 50, 56, 59, 62, 65, 72};

const int data_ov007_020ce564[12] = {0, 13, 24, 31, 37, 41, 57, 60, 64, 67, 70, 74};

const int data_ov007_020ce594[13] = {1, 13, 18, 21, 34, 38, 42, 55, 58, 73, 76, 80, 88};

const unsigned char data_ov007_020ce5c8[52] = {
    148, 150, 149, 146, 143, 135, 127, 123, 115, 111, 107, 109, 112, 116, 120, 124, 132, 136,
    140, 144, 147, 152, 155, 159, 162, 161, 159, 156, 155, 155, 151, 147, 143, 135, 131, 128,
    124, 120, 116, 114, 110, 106, 102, 100, 100, 96,  94,  93,  94,  97,  101, 105,
};

const unsigned char data_ov007_020ce5fc[52] = {
    123, 127, 131, 135, 138, 142, 142, 142, 138, 136, 128, 120, 117, 116, 114, 113, 113, 114,
    116, 117, 120, 121, 118, 116, 119, 123, 127, 130, 134, 138, 141, 141, 140, 144, 144, 141,
    142, 145, 144, 140, 142, 142, 140, 136, 132, 130, 126, 122, 118, 121, 123, 124,
};

const int data_ov007_020ce630[13] = {0, 22, 27, 33, 41, 45, 48, 61, 64, 69, 75, 80, 85};

const int data_ov007_020ce664[13] = {0, 12, 18, 25, 33, 38, 42, 46, 55, 60, 64, 69, 80};

const int data_ov007_020ce698[13] = {0, 10, 15, 21, 28, 33, 37, 45, 49, 54, 65, 71, 76};

const int data_ov007_020ce704[14] = {1, 10, 14, 17, 22, 30, 42, 51, 60, 75, 82, 89, 93, 95};

const int data_ov007_020ce774[14] = {1, 10, 17, 24, 29, 34, 41, 49, 56, 61, 70, 77, 82, 87};

const int data_ov007_020ce7ac[14] = {1, 9, 12, 15, 18, 24, 31, 36, 40, 44, 48, 53, 61, 64};

const int data_ov007_020ce7e4[14] = {1, 10, 14, 17, 21, 24, 28, 33, 37, 55, 61, 66, 71, 77};

const int data_ov007_020ce81c[14] = {1, 19, 23, 27, 31, 34, 38, 42, 47, 49, 53, 59, 62, 69};

const int data_ov007_020ce88c[14] = {1, 10, 12, 14, 16, 18, 20, 24, 30, 33, 35, 37, 39, 41};

const int data_ov007_020ce8fc[15] = {1, 15, 21, 27, 33, 39, 45, 52, 58, 66, 73, 80, 86, 90, 93};

const int data_ov007_020ce938[15] = {1, 16, 21, 26, 32, 40, 49, 57, 64, 74, 80, 86, 92, 101, 110};

const unsigned char data_ov007_020ce974[63] = {
    138, 137, 135, 134, 130, 127, 123, 119, 115, 111, 106, 102, 98,  94,  90,  86,
    82,  78,  74,  70,  66,  63,  59,  57,  54,  52,  50,  48,  47,  46,  47,  46,
    47,  48,  49,  51,  54,  57,  60,  64,  68,  73,  77,  81,  85,  89,  93,  97,
    101, 105, 109, 113, 117, 121, 124, 127, 130, 132, 136, 138, 139, 140, 141,
};

const unsigned char data_ov007_020ce9b4[63] = {
    81,  77,  73,  69,  67,  63,  61,  58,  57,  55,  53,  52,  51,  51,  51,  51,
    52,  54,  56,  59,  61,  64,  67,  71,  75,  79,  83,  87,  91,  95,  99,  103,
    107, 111, 115, 120, 124, 127, 130, 133, 135, 136, 138, 139, 140, 141, 142, 143,
    142, 141, 140, 140, 137, 135, 132, 129, 126, 122, 119, 115, 111, 106, 102,
};

const int data_ov007_020ce9f4[16] = {0,  14, 23,  33,  49,  64,  75,  83,
                                     91, 98, 105, 118, 127, 137, 146, 156};

const int data_ov007_020cea34[16] = {0,  16, 21, 33, 43,  55,  63,  65,
                                     81, 85, 88, 98, 108, 118, 125, 139};

const int data_ov007_020cea74[17] = {0,  15, 19, 23, 27, 31, 35, 39, 43,
                                     47, 55, 59, 69, 78, 88, 99, 103};

const int data_ov007_020ceab8[17] = {1,  15, 25, 29, 32, 34, 40, 43, 47,
                                     48, 51, 53, 55, 64, 71, 77, 83};

const int data_ov007_020ceafc[17] = {0,  10, 19, 25, 31, 38,  48,  57, 61,
                                     66, 72, 79, 88, 94, 103, 111, 119};

const int data_ov007_020ceb40[18] = {1,  13, 15, 20, 25, 31, 39, 42, 45,
                                     51, 53, 56, 58, 68, 72, 76, 85, 97};

const int data_ov007_020ceb88[18] = {1,  17, 23, 28, 33, 38, 42, 48,  54,
                                     60, 68, 72, 76, 81, 85, 92, 100, 106};

const int data_ov007_020cebd0[19] = {1,  9,  12, 15, 18, 22, 25, 28, 32, 35,
                                     40, 44, 47, 52, 58, 66, 73, 79, 87};

const int data_ov007_020cec1c[19] = {1,  7,  10, 12, 15, 18, 21, 25, 28, 32,
                                     36, 41, 44, 48, 51, 55, 58, 62, 69};

const int data_ov007_020cec68[19] = {1,  11, 14, 17, 27, 29, 42, 44,  54, 56,
                                     64, 67, 73, 75, 77, 85, 97, 110, 115};

const int data_ov007_020cecb4[19] = {0,  22, 26, 29, 36, 43,  45,  51,  58, 63,
                                     65, 67, 76, 83, 98, 104, 116, 118, 121};

const unsigned char data_ov007_020ced00[77] = {
    173, 170, 166, 163, 159, 151, 147, 139, 131, 123, 115, 111, 107, 103, 95,  92,
    88,  85,  83,  80,  77,  79,  85,  91,  94,  102, 106, 110, 114, 122, 130, 134,
    142, 151, 155, 159, 162, 166, 169, 172, 174, 175, 174, 178, 182, 182, 182, 178,
    176, 173, 172, 169, 162, 154, 146, 137, 133, 129, 125, 117, 109, 106, 102, 99,
    95,  93,  90,  88,  85,  83,  80,  76,  74,  74,  76,  79,  83,
};

const unsigned char data_ov007_020ced50[77] = {
    93, 90, 88, 84, 81, 77, 75, 73, 71, 73, 73, 74, 76, 77, 81, 84, 86, 89, 93, 96,
    93, 89, 81, 75, 72, 68, 67, 65, 65, 63, 63, 64, 64, 66, 68, 69, 72, 75, 78, 82,
    86, 90, 94, 91, 83, 75, 71, 63, 59, 55, 51, 47, 40, 34, 30, 27, 26, 26, 25, 27,
    29, 32, 33, 36, 39, 43, 46, 50, 53, 57, 61, 69, 77, 85, 89, 92, 93,
};

const int data_ov007_020ceda0[20] = {0,  13,  16,  30,  35,  52,  62,  73,  75,  79,
                                     99, 102, 105, 117, 122, 129, 134, 149, 153, 170};

const int data_ov007_020cedf0[20] = {1,  16, 19, 24, 28, 33, 39, 44, 48, 53,
                                     57, 60, 64, 69, 74, 79, 83, 87, 92, 99};

const int data_ov007_020cee40[20] = {1,  12, 16, 21, 27, 33, 39, 45, 52,  58,
                                     65, 72, 76, 79, 84, 88, 93, 98, 102, 108};

const int data_ov007_020cee90[20] = {1,  15,  23,  37,  49,  58,  65,  72,  79,  87,
                                     94, 100, 108, 114, 120, 126, 133, 140, 148, 154};

const int data_ov007_020ceee0[20] = {1,  14, 20, 25, 30, 36, 42, 48,  54,  60,
                                     66, 71, 77, 82, 87, 92, 98, 102, 108, 113};

const int data_ov007_020cef30[20] = {1,  15, 18, 21, 36, 38, 40, 50, 53,  56,
                                     59, 73, 74, 75, 77, 78, 89, 98, 100, 103};

const int data_ov007_020cefd0[21] = {1,  11, 17, 22, 28, 32, 37, 41,  45,  51, 57,
                                     62, 70, 78, 84, 89, 93, 99, 105, 111, 120};

const int data_ov007_020cf120[22] = {
    1, 14, 20, 25, 30, 35, 40, 46, 53, 61, 68, 75, 80, 84, 89, 93, 97, 102, 107, 112, 117, 123,
};

const int data_ov007_020cf178[22] = {
    1, 10, 14, 17, 29, 31, 42, 44, 46, 55, 57, 61, 68, 69, 71, 80, 82, 85, 92, 95, 105, 108,
};

const int data_ov007_020cf1d0[22] = {
    1,   12,  20,  30,  41,  58,  69,  80,  89,  97,  107,
    114, 123, 132, 144, 153, 162, 169, 174, 180, 186, 192,
};

const int data_ov007_020cf228[23] = {
    1,  19, 23,  28,  33,  40,  47,  54,  60,  70,  77,  84,
    89, 95, 102, 109, 114, 120, 125, 131, 136, 145, 156,
};

const int data_ov007_020cf284[24] = {
    1,  14, 17, 24, 30,  44,  51,  57,  63,  69,  74,  78,
    83, 87, 92, 97, 101, 105, 110, 116, 121, 126, 132, 140,
};

const int data_ov007_020cf2e4[24] = {
    0,   15,  30,  40,  49,  65,  75,  84,  104, 115, 120, 125,
    130, 138, 151, 157, 165, 179, 187, 200, 207, 220, 234, 240,
};

const int data_ov007_020cf344[25] = {
    1,  8,  13, 17, 21,  25,  30,  36,  41,  48,  53,  60,  68,
    75, 82, 89, 95, 100, 106, 111, 115, 120, 126, 131, 139,
};

const int data_ov007_020cf3a8[25] = {
    1,   15,  21,  24,  29,  35,  48,  61,  64,  69,  73,  77,  99,
    105, 112, 117, 135, 143, 150, 156, 167, 179, 188, 194, 213,
};

const int data_ov007_020cf40c[26] = {
    1,  13, 20, 26, 28, 34, 36, 42, 43, 51, 53,  57,  59,
    61, 67, 69, 77, 79, 85, 87, 91, 93, 97, 107, 111, 118,
};

const int data_ov007_020cf474[26] = {
    1,   16,  24,  30,  36,  43,  49,  75,  81,  88,  95,  102, 108,
    116, 125, 137, 156, 168, 179, 187, 193, 200, 206, 212, 231, 234,
};

const int data_ov007_020cf4dc[26] = {
    1,  8,  11, 14, 16, 19, 22, 25, 28, 31, 35, 38,  42,
    46, 50, 54, 59, 65, 70, 76, 81, 85, 91, 97, 102, 110,
};

const int data_ov007_020cf544[26] = {
    1,  11, 14,  27,  35,  41,  43,  48,  53,  63,  68,  76,  82,
    94, 97, 100, 103, 120, 122, 127, 133, 140, 148, 152, 156, 159,
};

const int data_ov007_020cf5ac[27] = {
    1,   16,  21,  25,  30,  35,  41,  46,  51,  59,  68,  79,  90,  101,
    110, 116, 121, 126, 136, 147, 157, 166, 174, 182, 188, 198, 202,
};

const int data_ov007_020cf618[28] = {
    1,  10, 14, 17, 20, 24, 29,  34,  39,  44,  51,  56,  58,  62,
    74, 79, 84, 89, 93, 98, 102, 107, 113, 118, 124, 131, 139, 147,
};

const int data_ov007_020cf688[30] = {
    1,  14, 18, 23, 29, 34,  39,  44,  48,  53,  58,  62,  66,  71,  75,
    79, 84, 88, 93, 98, 103, 111, 117, 125, 135, 142, 148, 152, 157, 162,
};

const int data_ov007_020cf778[31] = {
    1,  6,  8,  11, 14, 16, 18, 20, 23, 25, 27, 33, 35, 39, 43, 45,
    47, 51, 52, 53, 55, 60, 62, 69, 71, 78, 79, 81, 85, 87, 94,
};

const int data_ov007_020cf7f4[31] = {
    1,  8,  11, 14, 16, 18, 21, 24, 27, 30,  32,  36,  39,  42,  45,  48,
    52, 56, 61, 65, 70, 75, 79, 86, 94, 101, 106, 115, 121, 126, 131,
};

const int data_ov007_020cf870[31] = {
    1,  7,  12, 16, 21, 28, 34,  41,  48,  52,  57,  61,  65,  68,  71,  76,
    79, 84, 86, 90, 93, 97, 101, 105, 109, 113, 118, 122, 127, 135, 146,
};

const int data_ov007_020cf9ec[33] = {
    1,  16, 22, 29, 34,  38,  42,  46,  49,  56,  60,  66,  70,  74,  76,  80,  82,
    84, 89, 92, 98, 100, 107, 109, 114, 116, 123, 128, 136, 141, 146, 150, 153,
};

const int data_ov007_020cfaf8[35] = {
    1,   20,  22,  24,  35,  38,  51,  53,  55,  67,  69,  71,  85,  87,  96,  98,  100, 109,
    111, 114, 119, 122, 124, 127, 132, 134, 136, 144, 146, 150, 155, 157, 159, 168, 177,
};

const int data_ov007_020cfc18[37] = {
    1,   13,  15,  18,  35,  37,  39,  43,  60,  63,  66,  78,  82,  85,  88,  100, 103, 105, 109,
    118, 121, 124, 140, 143, 148, 157, 160, 162, 171, 183, 188, 191, 196, 204, 208, 212, 218,
};

const int data_ov007_020cfcac[38] = {
    1,  12, 16, 19, 23,  27,  30,  35,  39,  42,  46,  50,  55,  59,  63,  67,  70,  75,  79,
    84, 88, 92, 97, 100, 104, 109, 116, 122, 129, 135, 143, 150, 156, 161, 168, 173, 177, 183,
};

const int data_ov007_020cfd44[42] = {
    1,   16,  18,  20,  29,  31,  33,  53,  56,  58,  60,  66,  68,  70,
    75,  77,  79,  85,  87,  96,  98,  100, 105, 106, 107, 109, 118, 120,
    123, 129, 132, 143, 145, 150, 152, 160, 161, 163, 167, 169, 179, 187,
};

const int data_ov007_020cfe94[52] = {
    0,   15,  20,  27,  33,  43,  53,  58,  69,  77,  92,  108, 114, 120, 124, 129, 140, 147,
    153, 161, 171, 202, 206, 210, 231, 237, 242, 246, 282, 293, 304, 311, 319, 345, 352, 357,
    372, 379, 388, 395, 408, 414, 421, 428, 436, 457, 468, 477, 484, 517, 524, 530,
};

const int data_ov007_020cff64[63] = {
    1,   11,  15,  19,  23,  27,  31,  35,  38,  42,  45,  48,  53,  56,  59,  62,
    65,  69,  72,  75,  78,  83,  87,  90,  94,  98,  102, 107, 110, 113, 116, 120,
    124, 127, 132, 137, 143, 146, 149, 152, 155, 159, 163, 166, 170, 174, 178, 182,
    185, 188, 192, 196, 200, 205, 209, 212, 215, 219, 223, 228, 232, 237, 245,
};

const int data_ov007_020d0060[77] = {
    0,   20,  27,  33,  38,  48,  55,  63,  73,  84,  92,  98,  104, 110, 123, 130,
    137, 143, 149, 157, 174, 182, 190, 196, 201, 210, 215, 219, 222, 232, 241, 246,
    256, 267, 271, 276, 280, 284, 288, 294, 300, 305, 313, 362, 370, 378, 381, 388,
    392, 395, 398, 401, 408, 418, 429, 437, 441, 445, 448, 455, 465, 470, 474, 478,
    482, 485, 489, 493, 497, 501, 505, 514, 524, 535, 542, 548, 558,
};

static const int *sPortraitStrokePointCounts[6] = {
    data_ov007_020cf700, data_ov007_020cfa70, data_ov007_020cfb84,
    data_ov007_020cef80, data_ov007_020cf96c, data_ov007_020cf8ec,
};
extern const unsigned char *data_ov007_020d7ffc[30];
extern const unsigned char *data_ov007_020d8464[34];
extern const unsigned char *data_ov007_020d85fc[37];
extern const unsigned char *data_ov007_020d7f0c[20];
extern const unsigned char *data_ov007_020d82e4[32];
extern const unsigned char *data_ov007_020d8164[32];
static const unsigned char *const *sPortraitStrokeXCoords[6] = {
    data_ov007_020d7ffc, data_ov007_020d8464, data_ov007_020d85fc,
    data_ov007_020d7f0c, data_ov007_020d82e4, data_ov007_020d8164,
};
extern const unsigned char *data_ov007_020d8074[30];
extern const unsigned char *data_ov007_020d84ec[34];
extern const unsigned char *data_ov007_020d8690[37];
extern const unsigned char *data_ov007_020d7f5c[20];
extern const unsigned char *data_ov007_020d8364[32];
extern const unsigned char *data_ov007_020d81e4[32];
static const unsigned char *const *sPortraitStrokeYCoords[6] = {
    data_ov007_020d8074, data_ov007_020d84ec, data_ov007_020d8690,
    data_ov007_020d7f5c, data_ov007_020d8364, data_ov007_020d81e4,
};
extern const int *data_ov007_020d80ec[30];
extern const int *data_ov007_020d8574[34];
extern const int *data_ov007_020d8724[37];
extern const int *data_ov007_020d7fac[20];
extern const int *data_ov007_020d83e4[32];
extern const int *data_ov007_020d8264[32];
static const int *const *sPortraitStrokeProgressThresholds[6] = {
    data_ov007_020d80ec, data_ov007_020d8574, data_ov007_020d8724,
    data_ov007_020d7fac, data_ov007_020d83e4, data_ov007_020d8264,
};

extern Ov007PickerScene *sPickerScene;

/* The per-figure stroke stages as a pointer: indexing through it, not
 * through gMenuWork->strokeStage, gives the ROM's base + 0xfc then [idx] form. */
static inline u8 *StrokeStages(void) {
    return gMenuWork->strokeStage;
}

/* Loads figure idx's next portrait drawing (strokeStage[idx]: 0, 1, 2, then
 * back to 0) into the picker, with a replay of 120 frames; figure 0's third
 * drawing gets 1.5 times that. */
void func_ov007_020bbe60(int idx) {
    int drawing = idx * 3 + StrokeStages()[idx];
    func_ov007_020c6784(sPickerScene->picker, sPortraitStrokeCounts[drawing],
                        sPortraitStrokePointCounts[drawing], sPortraitStrokeXCoords[drawing],
                        sPortraitStrokeYCoords[drawing], sPortraitStrokeProgressThresholds[drawing],
                        1);
    sPickerScene->replayFrames = 120;
    if (idx == 0 && StrokeStages()[idx] == 2) {
        int frames = FX_MUL(sPickerScene->replayFrames << 12, 0x1800);
        sPickerScene->replayFrames = frames >> 12;
    }
    StrokeStages()[idx]++;
    if (StrokeStages()[idx] >= 3)
        StrokeStages()[idx] = 0;
}

const int data_ov007_020cef80[20] = {27, 14, 5,  8,  4,  3,  19, 10, 63, 11,
                                     9,  31, 15, 11, 22, 26, 5,  20, 5,  4};
const int data_ov007_020cf700[30] = {
    16, 12, 10, 17, 7, 8,  52, 19, 16, 10, 13, 12, 24, 12, 13,
    3,  2,  12, 13, 3, 77, 17, 20, 6,  8,  2,  2,  7,  7,  3,
};
const int data_ov007_020cf8ec[32] = {
    27, 14, 5, 8, 4,  3, 19, 10, 63, 11, 9, 31, 15, 11, 22, 26,
    5,  20, 5, 4, 37, 5, 5,  8,  9,  7,  5, 10, 8,  7,  5,  7,
};
const int data_ov007_020cf96c[32] = {
    27, 14, 5, 8, 4,  3,  19, 10, 63, 11, 9, 31, 15, 11, 22, 26,
    5,  20, 5, 4, 28, 42, 26, 3,  1,  4,  6, 6,  5,  5,  4,  3,
};
const int data_ov007_020cfa70[34] = {
    22, 9,  5, 5,  2,  15, 11, 4,  1,  8,  6,  25, 25, 3,  21, 12, 5,
    23, 10, 4, 14, 24, 30, 38, 20, 12, 13, 11, 33, 31, 18, 1,  1,  2,
};
const int data_ov007_020cfb84[37] = {
    18, 7,  5,  14, 6, 3, 10, 7, 8,  20, 26, 20, 22, 6, 3, 14, 8, 20, 31,
    3,  26, 14, 19, 9, 7, 35, 9, 17, 19, 5,  6,  9,  7, 6, 14, 8, 1,
};

const unsigned char *data_ov007_020d7f0c[20] = {
    data_ov007_020cdccc, data_ov007_020cd394, data_ov007_020cce7c, data_ov007_020cd074,
    data_ov007_020ccd74, data_ov007_020ccd10, data_ov007_020cd5ec, data_ov007_020cd13c,
    data_ov007_020ce974, data_ov007_020cd1cc, data_ov007_020cd0ac, data_ov007_020cdf14,
    data_ov007_020cd464, data_ov007_020cd1e4, data_ov007_020cd8f4, data_ov007_020cdc94,
    data_ov007_020cce64, data_ov007_020cd7b8, data_ov007_020cce24, data_ov007_020ccd78,
};
const unsigned char *data_ov007_020d7f5c[20] = {
    data_ov007_020cdce8, data_ov007_020cd3a4, data_ov007_020ccddc, data_ov007_020cd06c,
    data_ov007_020ccdb4, data_ov007_020ccd48, data_ov007_020cd600, data_ov007_020cd148,
    data_ov007_020ce9b4, data_ov007_020cd1d8, data_ov007_020cd0b8, data_ov007_020cdf34,
    data_ov007_020cd454, data_ov007_020cd1f0, data_ov007_020cd90c, data_ov007_020cdcb0,
    data_ov007_020ccdc4, data_ov007_020cd7cc, data_ov007_020cce4c, data_ov007_020ccd6c,
};
const int *data_ov007_020d7fac[20] = {
    data_ov007_020cf5ac, data_ov007_020ce774, data_ov007_020cd704, data_ov007_020cdfb4,
    data_ov007_020cd484, data_ov007_020cd214, data_ov007_020cebd0, data_ov007_020ce2cc,
    data_ov007_020cff64, data_ov007_020ce3f0, data_ov007_020ce104, data_ov007_020cf870,
    data_ov007_020ce8fc, data_ov007_020ce41c, data_ov007_020cf178, data_ov007_020cf544,
    data_ov007_020cd7a4, data_ov007_020cef30, data_ov007_020cd7f4, data_ov007_020cd4b4,
};
const unsigned char *data_ov007_020d7ffc[30] = {
    data_ov007_020cd4d4, data_ov007_020cd268, data_ov007_020cd154, data_ov007_020cd574,
    data_ov007_020ccf94, data_ov007_020cd04c, data_ov007_020ce5c8, data_ov007_020cd664,
    data_ov007_020cd4f4, data_ov007_020cd178, data_ov007_020cd334, data_ov007_020cd2b0,
    data_ov007_020cdb4c, data_ov007_020cd2c8, data_ov007_020cd354, data_ov007_020ccd54,
    data_ov007_020ccd00, data_ov007_020cd2ec, data_ov007_020cd374, data_ov007_020ccd08,
    data_ov007_020ced00, data_ov007_020cd538, data_ov007_020cd880, data_ov007_020cce9c,
    data_ov007_020ccfcc, data_ov007_020ccccc, data_ov007_020cccdc, data_ov007_020ccf64,
    data_ov007_020ccf34, data_ov007_020ccd1c,
};
const unsigned char *data_ov007_020d8074[30] = {
    data_ov007_020cd4e4, data_ov007_020cd274, data_ov007_020cd160, data_ov007_020cd588,
    data_ov007_020ccf8c, data_ov007_020cd054, data_ov007_020ce5fc, data_ov007_020cd678,
    data_ov007_020cd504, data_ov007_020cd184, data_ov007_020cd344, data_ov007_020cd2bc,
    data_ov007_020cdb64, data_ov007_020cd2d4, data_ov007_020cd364, data_ov007_020ccd0c,
    data_ov007_020ccce0, data_ov007_020cd2f8, data_ov007_020cd314, data_ov007_020ccd3c,
    data_ov007_020ced50, data_ov007_020cd524, data_ov007_020cd68c, data_ov007_020cceb4,
    data_ov007_020ccfe4, data_ov007_020cccf4, data_ov007_020cccd0, data_ov007_020ccf5c,
    data_ov007_020ccfb4, data_ov007_020ccd44,
};
const int *data_ov007_020d80ec[30] = {
    data_ov007_020ce9f4, data_ov007_020ce4d4, data_ov007_020ce2f4, data_ov007_020ceafc,
    data_ov007_020cde1c, data_ov007_020ce034, data_ov007_020cfe94, data_ov007_020cecb4,
    data_ov007_020cea34, data_ov007_020ce31c, data_ov007_020ce630, data_ov007_020ce504,
    data_ov007_020cf2e4, data_ov007_020ce534, data_ov007_020ce664, data_ov007_020cd2e0,
    data_ov007_020ccffc, data_ov007_020ce564, data_ov007_020ce698, data_ov007_020cd22c,
    data_ov007_020d0060, data_ov007_020cea74, data_ov007_020ceda0, data_ov007_020cd99c,
    data_ov007_020cdf94, data_ov007_020ccfdc, data_ov007_020ccfec, data_ov007_020cdd3c,
    data_ov007_020cdd58, data_ov007_020cd220,
};
const unsigned char *data_ov007_020d8164[32] = {
    data_ov007_020cdccc, data_ov007_020cd394, data_ov007_020cce7c, data_ov007_020cd074,
    data_ov007_020ccd74, data_ov007_020ccd10, data_ov007_020cd5ec, data_ov007_020cd13c,
    data_ov007_020ce974, data_ov007_020cd1cc, data_ov007_020cd0ac, data_ov007_020cdf14,
    data_ov007_020cd464, data_ov007_020cd1e4, data_ov007_020cd8f4, data_ov007_020cdc94,
    data_ov007_020cce64, data_ov007_020cd7b8, data_ov007_020cce24, data_ov007_020ccd78,
    data_ov007_020ce1dc, data_ov007_020cce5c, data_ov007_020cce34, data_ov007_020cd05c,
    data_ov007_020cd07c, data_ov007_020ccf6c, data_ov007_020ccdfc, data_ov007_020cd190,
    data_ov007_020cd024, data_ov007_020ccf74, data_ov007_020ccdf4, data_ov007_020ccf2c,
};
const unsigned char *data_ov007_020d81e4[32] = {
    data_ov007_020cdce8, data_ov007_020cd3a4, data_ov007_020ccddc, data_ov007_020cd06c,
    data_ov007_020ccdb4, data_ov007_020ccd48, data_ov007_020cd600, data_ov007_020cd148,
    data_ov007_020ce9b4, data_ov007_020cd1d8, data_ov007_020cd0b8, data_ov007_020cdf34,
    data_ov007_020cd454, data_ov007_020cd1f0, data_ov007_020cd90c, data_ov007_020cdcb0,
    data_ov007_020ccdc4, data_ov007_020cd7cc, data_ov007_020cce4c, data_ov007_020ccd6c,
    data_ov007_020ce204, data_ov007_020cce44, data_ov007_020cce8c, data_ov007_020cd02c,
    data_ov007_020cd094, data_ov007_020ccfac, data_ov007_020cce54, data_ov007_020cd16c,
    data_ov007_020ccfc4, data_ov007_020ccf9c, data_ov007_020ccdbc, data_ov007_020ccf1c,
};
const int *data_ov007_020d8264[32] = {
    data_ov007_020cf5ac, data_ov007_020ce774, data_ov007_020cd704, data_ov007_020cdfb4,
    data_ov007_020cd484, data_ov007_020cd214, data_ov007_020cebd0, data_ov007_020ce2cc,
    data_ov007_020cff64, data_ov007_020ce3f0, data_ov007_020ce104, data_ov007_020cf870,
    data_ov007_020ce8fc, data_ov007_020ce41c, data_ov007_020cf178, data_ov007_020cf544,
    data_ov007_020cd7a4, data_ov007_020cef30, data_ov007_020cd7f4, data_ov007_020cd4b4,
    data_ov007_020cfc18, data_ov007_020cd808, data_ov007_020cd81c, data_ov007_020ce054,
    data_ov007_020ce194, data_ov007_020cde38, data_ov007_020cd844, data_ov007_020ce344,
    data_ov007_020cdf74, data_ov007_020cdd04, data_ov007_020cd6b4, data_ov007_020cdd20,
};
const unsigned char *data_ov007_020d82e4[32] = {
    data_ov007_020cdccc, data_ov007_020cd394, data_ov007_020cce7c, data_ov007_020cd074,
    data_ov007_020ccd74, data_ov007_020ccd10, data_ov007_020cd5ec, data_ov007_020cd13c,
    data_ov007_020ce974, data_ov007_020cd1cc, data_ov007_020cd0ac, data_ov007_020cdf14,
    data_ov007_020cd464, data_ov007_020cd1e4, data_ov007_020cd8f4, data_ov007_020cdc94,
    data_ov007_020cce64, data_ov007_020cd7b8, data_ov007_020cce24, data_ov007_020ccd78,
    data_ov007_020cdd74, data_ov007_020ce36c, data_ov007_020cdbec, data_ov007_020ccd18,
    data_ov007_020cccac, data_ov007_020ccda8, data_ov007_020ccebc, data_ov007_020ccf14,
    data_ov007_020cce2c, data_ov007_020cce0c, data_ov007_020ccd84, data_ov007_020ccd40,
};
const unsigned char *data_ov007_020d8364[32] = {
    data_ov007_020cdce8, data_ov007_020cd3a4, data_ov007_020ccddc, data_ov007_020cd06c,
    data_ov007_020ccdb4, data_ov007_020ccd48, data_ov007_020cd600, data_ov007_020cd148,
    data_ov007_020ce9b4, data_ov007_020cd1d8, data_ov007_020cd0b8, data_ov007_020cdf34,
    data_ov007_020cd454, data_ov007_020cd1f0, data_ov007_020cd90c, data_ov007_020cdcb0,
    data_ov007_020ccdc4, data_ov007_020cd7cc, data_ov007_020cce4c, data_ov007_020ccd6c,
    data_ov007_020cdd90, data_ov007_020ce398, data_ov007_020cdc08, data_ov007_020ccd28,
    data_ov007_020cccb4, data_ov007_020ccd8c, data_ov007_020ccecc, data_ov007_020cceac,
    data_ov007_020cce04, data_ov007_020cce3c, data_ov007_020ccd9c, data_ov007_020ccd24,
};
const int *data_ov007_020d83e4[32] = {
    data_ov007_020cf5ac, data_ov007_020ce774, data_ov007_020cd704, data_ov007_020cdfb4,
    data_ov007_020cd484, data_ov007_020cd214, data_ov007_020cebd0, data_ov007_020ce2cc,
    data_ov007_020cff64, data_ov007_020ce3f0, data_ov007_020ce104, data_ov007_020cf870,
    data_ov007_020ce8fc, data_ov007_020ce41c, data_ov007_020cf178, data_ov007_020cf544,
    data_ov007_020cd7a4, data_ov007_020cef30, data_ov007_020cd7f4, data_ov007_020cd4b4,
    data_ov007_020cf618, data_ov007_020cfd44, data_ov007_020cf40c, data_ov007_020cd244,
    data_ov007_020ccd70, data_ov007_020cd494, data_ov007_020cda74, data_ov007_020cda8c,
    data_ov007_020cd77c, data_ov007_020cd7e0, data_ov007_020cd4c4, data_ov007_020cd280,
};
const unsigned char *data_ov007_020d8464[34] = {
    data_ov007_020cd924, data_ov007_020cd100, data_ov007_020cce1c, data_ov007_020cce6c,
    data_ov007_020cccfc, data_ov007_020cd474, data_ov007_020cd1c0, data_ov007_020ccdb0,
    data_ov007_020cccbc, data_ov007_020ccfbc, data_ov007_020ccefc, data_ov007_020cdb7c,
    data_ov007_020cdbb4, data_ov007_020ccd2c, data_ov007_020cd8ac, data_ov007_020cd298,
    data_ov007_020cce74, data_ov007_020cd954, data_ov007_020cd118, data_ov007_020ccd94,
    data_ov007_020cd434, data_ov007_020cd9cc, data_ov007_020cde54, data_ov007_020ce22c,
    data_ov007_020cd6c8, data_ov007_020cd208, data_ov007_020cd324, data_ov007_020cd19c,
    data_ov007_020ce074, data_ov007_020cde94, data_ov007_020cd5d8, data_ov007_020ccca4,
    data_ov007_020ccca8, data_ov007_020ccd04,
};
const unsigned char *data_ov007_020d84ec[34] = {
    data_ov007_020cd93c, data_ov007_020cd0f4, data_ov007_020cce84, data_ov007_020ccde4,
    data_ov007_020ccce4, data_ov007_020cd444, data_ov007_020cd1b4, data_ov007_020ccd80,
    data_ov007_020cccc8, data_ov007_020ccfd4, data_ov007_020cceec, data_ov007_020cdb98,
    data_ov007_020cdbd0, data_ov007_020ccd14, data_ov007_020cd894, data_ov007_020cd1fc,
    data_ov007_020ccdcc, data_ov007_020cd96c, data_ov007_020cd10c, data_ov007_020ccd88,
    data_ov007_020cd384, data_ov007_020cd984, data_ov007_020cde74, data_ov007_020ce254,
    data_ov007_020cd6dc, data_ov007_020cd28c, data_ov007_020cd304, data_ov007_020cd1a8,
    data_ov007_020ce098, data_ov007_020cdeb4, data_ov007_020cd59c, data_ov007_020cccb0,
    data_ov007_020cccc0, data_ov007_020cccec,
};
const int *data_ov007_020d8574[34] = {
    data_ov007_020cf1d0, data_ov007_020ce1b8, data_ov007_020cd858, data_ov007_020cd86c,
    data_ov007_020cd064, data_ov007_020ce938, data_ov007_020ce448, data_ov007_020cd514,
    data_ov007_020ccd64, data_ov007_020cdf54, data_ov007_020cdaec, data_ov007_020cf344,
    data_ov007_020cf3a8, data_ov007_020cd2a4, data_ov007_020cefd0, data_ov007_020ce474,
    data_ov007_020cd6a0, data_ov007_020cf228, data_ov007_020ce27c, data_ov007_020cd4a4,
    data_ov007_020ce704, data_ov007_020cf284, data_ov007_020cf688, data_ov007_020cfcac,
    data_ov007_020cedf0, data_ov007_020ce4a4, data_ov007_020ce594, data_ov007_020ce3c4,
    data_ov007_020cf9ec, data_ov007_020cf778, data_ov007_020ceb40, data_ov007_020ccda4,
    data_ov007_020ccda0, data_ov007_020cd004,
};
const unsigned char *data_ov007_020d85fc[37] = {
    data_ov007_020cd5b0, data_ov007_020ccf54, data_ov007_020cce14, data_ov007_020cd424,
    data_ov007_020ccf04, data_ov007_020ccd50, data_ov007_020cd124, data_ov007_020ccf44,
    data_ov007_020cd034, data_ov007_020cd718, data_ov007_020cdc24, data_ov007_020cd830,
    data_ov007_020cd8c4, data_ov007_020ccea4, data_ov007_020ccd30, data_ov007_020cd414,
    data_ov007_020ccff4, data_ov007_020cd754, data_ov007_020cded4, data_ov007_020ccd20,
    data_ov007_020cdc5c, data_ov007_020cd3e4, data_ov007_020cd614, data_ov007_020cd088,
    data_ov007_020ccfa4, data_ov007_020ce0bc, data_ov007_020cd0dc, data_ov007_020cd54c,
    data_ov007_020cd63c, data_ov007_020ccdec, data_ov007_020ccedc, data_ov007_020cd0e8,
    data_ov007_020ccf84, data_ov007_020cced4, data_ov007_020cd3f4, data_ov007_020cd044,
    data_ov007_020cccc4,
};
const unsigned char *data_ov007_020d8690[37] = {
    data_ov007_020cd5c4, data_ov007_020ccf24, data_ov007_020cce94, data_ov007_020cd3b4,
    data_ov007_020ccf0c, data_ov007_020ccd34, data_ov007_020cd130, data_ov007_020ccf4c,
    data_ov007_020cd00c, data_ov007_020cd72c, data_ov007_020cdc40, data_ov007_020cd740,
    data_ov007_020cd8dc, data_ov007_020ccec4, data_ov007_020ccd4c, data_ov007_020cd3c4,
    data_ov007_020cd014, data_ov007_020cd768, data_ov007_020cdef4, data_ov007_020ccd38,
    data_ov007_020cdc78, data_ov007_020cd3d4, data_ov007_020cd628, data_ov007_020cd0c4,
    data_ov007_020ccf3c, data_ov007_020ce0e0, data_ov007_020cd0d0, data_ov007_020cd560,
    data_ov007_020cd650, data_ov007_020ccdd4, data_ov007_020ccee4, data_ov007_020cd0a0,
    data_ov007_020ccf7c, data_ov007_020ccef4, data_ov007_020cd404, data_ov007_020cd01c,
    data_ov007_020cccb8,
};
const int *data_ov007_020d8724[37] = {
    data_ov007_020ceb88, data_ov007_020cddac, data_ov007_020cd6f0, data_ov007_020ce7ac,
    data_ov007_020cd9fc, data_ov007_020cd238, data_ov007_020ce2a4, data_ov007_020cddc8,
    data_ov007_020cdfd4, data_ov007_020cee40, data_ov007_020cf474, data_ov007_020cee90,
    data_ov007_020cf120, data_ov007_020cda44, data_ov007_020cd250, data_ov007_020ce7e4,
    data_ov007_020cdff4, data_ov007_020ceee0, data_ov007_020cf7f4, data_ov007_020cd25c,
    data_ov007_020cf4dc, data_ov007_020ce81c, data_ov007_020cec1c, data_ov007_020ce128,
    data_ov007_020cdde4, data_ov007_020cfaf8, data_ov007_020ce14c, data_ov007_020ceab8,
    data_ov007_020cec68, data_ov007_020cd790, data_ov007_020cdaa4, data_ov007_020ce170,
    data_ov007_020cde00, data_ov007_020cdad4, data_ov007_020ce88c, data_ov007_020ce014,
    data_ov007_020ccd98,
};
