#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef union {
    u32 w;
    void *p;
} Word;

extern const Word data_ov057_021111a0[];
extern const Word data_ov057_021111a4[];
extern const Word data_ov057_021111a8[];
extern const Word data_ov057_021111c4[];
extern Word data_ov057_021114a0[];
extern Word data_ov057_021114a4[];
extern Word data_ov057_021114a8[];
extern Word data_ov057_021114ac[];
extern Word data_ov057_021114b4[];
extern Word data_ov057_021114bc[];
extern Word data_ov057_021114c4[];
extern Word data_ov057_021114cc[];
extern Word data_ov057_021114d8[];
extern Word data_ov057_021114e8[];
extern Word data_ov057_021114f8[];
extern Word data_ov057_02111508[];
extern Word data_ov057_021115ec[];

const Word data_ov057_021111a0[1] = {{.w = 0x00000000}};

Word data_ov057_021114a8[1] = {{.w = 0x00000004}};

Word data_ov057_021114a0[1] = {{.w = 0x00000737}};

Word data_ov057_021114a4[1] = {{.w = 0x000007d0}};

const Word data_ov057_021111a4[1] = {{.w = 0x00001000}};

Word data_ov057_021114ac[2] = {{.w = 0x00000001}, {.p = data_ov057_021114c4}};

Word data_ov057_021114c4[2] = {{.w = 0x00001200}, {.p = data_ov057_021115ec}};

Word data_ov057_021114b4[2] = {{.w = 0x00000006}, {.p = data_ov057_02111508 + 45}};

Word data_ov057_021114bc[2] = {{.w = 0xffffdd01}, {.w = 0xd6d82710}};

Word data_ov057_021114cc[3] = {
    {.p = data_ov057_021114ac}, {.p = data_ov057_02111508}, {.w = 0x06400000}};

Word data_ov057_021114d8[4] = {
    {.w = 0x0000ff07}, {.w = 0x00000000}, {.w = 0x00000000}, {.w = 0x00000000}};

Word data_ov057_021114e8[4] = {
    {.w = 0x5f74616d}, {.w = 0x6e5f7a7a}, {.w = 0x72616761}, {.w = 0x00000065}};

Word data_ov057_021114f8[4] = {
    {.w = 0x00000000}, {.w = 0xfbe60a8c}, {.w = 0x00000000}, {.w = 0x05780000}};

const Word data_ov057_021111a8[7] = {
    {.w = 0x0000ffff}, {.p = data_ov057_021114e8}, {.w = 0x00000001}, {.w = 0x00000001},
    {.w = 0x00000169}, {.w = 0x00000001},          {.w = 0x00000001}};

Word data_ov057_02111508[57] = {{.w = 0x00000169},
                                {.p = data_ov057_021111a4},
                                {.p = data_ov057_021111c4},
                                {.p = data_ov057_021111a0},
                                {.w = 0x00000001},
                                {.p = data_ov057_021111a8},
                                {.p = data_ov057_02111508 + 29},
                                {.p = data_ov057_021114b4},
                                {.w = 0x07360738},
                                {.w = 0x073a0739},
                                {.p = data_ov057_021114cc},
                                {.w = 0x07080001},
                                {.w = 0x0000004f},
                                {.w = 0x53504c43},
                                {.w = 0x00010008},
                                {.w = 0x0003cfc7},
                                {.w = 0x000000ff},
                                {.w = 0xffffffff},
                                {.w = 0xffffffff},
                                {.w = 0xffffffff},
                                {.w = 0xffffffff},
                                {.w = 0x53504c43},
                                {.w = 0x00010008},
                                {.w = 0x0003cfc7},
                                {.w = 0x000000ff},
                                {.w = 0xffffffff},
                                {.w = 0xffffffff},
                                {.w = 0xffffffff},
                                {.w = 0xffffffff},
                                {.w = 0x53504c43},
                                {.w = 0x00020008},
                                {.w = 0x00080fc0},
                                {.w = 0x000000ff},
                                {.w = 0x01000fc0},
                                {.w = 0x000000ff},
                                {.w = 0xffffffff},
                                {.w = 0xffffffff},
                                {.w = 0x53504c43},
                                {.w = 0x00010008},
                                {.w = 0x0003cfc7},
                                {.w = 0x000000ff},
                                {.w = 0xffffffff},
                                {.w = 0xffffffff},
                                {.w = 0xffffffff},
                                {.w = 0xffffffff},
                                {.w = 0x00000104},
                                {.p = data_ov057_021114d8},
                                {.w = 0x0000010b},
                                {.p = data_ov057_021114a0},
                                {.w = 0x00000101},
                                {.p = data_ov057_021114f8},
                                {.w = 0x00000108},
                                {.p = data_ov057_021114bc},
                                {.w = 0x0000010c},
                                {.p = data_ov057_021114a4},
                                {.w = 0x0000010e},
                                {.p = data_ov057_021114a8}};

Word data_ov057_021115ec[77] = {
    {.w = 0x00000123}, {.w = 0x000003e8}, {.w = 0x00000000}, {.w = 0xffff0000}, {.w = 0x00000001},
    {.w = 0x000002ee}, {.w = 0x00000000}, {.w = 0xffff0000}, {.w = 0x00000002}, {.w = 0x079602ee},
    {.w = 0x00000000}, {.w = 0xffff0000}, {.w = 0x00000003}, {.w = 0x09ec02ee}, {.w = 0x00000000},
    {.w = 0xffff0000}, {.w = 0x055d0002}, {.w = 0x055d02ee}, {.w = 0x20000000}, {.w = 0xffff0000},
    {.w = 0x07960002}, {.w = 0x000002ee}, {.w = 0x40000000}, {.w = 0xffff0000}, {.w = 0x055d0002},
    {.w = 0xfaa302ee}, {.w = 0x60000000}, {.w = 0xffff0000}, {.w = 0x00000002}, {.w = 0xf86a02ee},
    {.w = 0x80000000}, {.w = 0xffff0000}, {.w = 0xfaa30002}, {.w = 0xfaa302ee}, {.w = 0xa0000000},
    {.w = 0xffff0000}, {.w = 0xf86a0002}, {.w = 0x000002ee}, {.w = 0xc0000000}, {.w = 0xffff0000},
    {.w = 0xfaa30002}, {.w = 0x055d02ee}, {.w = 0xe0000000}, {.w = 0xffff0000}, {.w = 0x07040003},
    {.w = 0x070402ee}, {.w = 0x20000000}, {.w = 0xffff0000}, {.w = 0x09ec0003}, {.w = 0x000002ee},
    {.w = 0x40000000}, {.w = 0xffff0000}, {.w = 0x07040003}, {.w = 0xf8fc02ee}, {.w = 0x60000000},
    {.w = 0xffff0000}, {.w = 0x00000003}, {.w = 0xf61402ee}, {.w = 0x80000000}, {.w = 0xffff0000},
    {.w = 0xf8fc0003}, {.w = 0xf8fc02ee}, {.w = 0xa0000000}, {.w = 0xffff0000}, {.w = 0xf6140003},
    {.w = 0x000002ee}, {.w = 0xc0000000}, {.w = 0xffff0000}, {.w = 0xf8fc0003}, {.w = 0x070402ee},
    {.w = 0xe0000000}, {.w = 0xffff0000}, {.w = 0x00000000}, {.w = 0x00000000}, {.w = 0x00000000},
    {.w = 0x00000000}, {.w = 0x00000000}};

const Word data_ov057_021111c4[181] = {
    {.w = 0x000b0000}, {.w = 0x00220017}, {.w = 0x0039002e}, {.w = 0x00500044}, {.w = 0x0066005b},
    {.w = 0x007d0072}, {.w = 0x00940089}, {.w = 0x00ab009f}, {.w = 0x00c100b6}, {.w = 0x00d800cd},
    {.w = 0x00ef00e4}, {.w = 0x010600fa}, {.w = 0x011c0111}, {.w = 0x01330128}, {.w = 0x014a013f},
    {.w = 0x01610155}, {.w = 0x0177016c}, {.w = 0x018e0183}, {.w = 0x01a5019a}, {.w = 0x01bc01b0},
    {.w = 0x01d201c7}, {.w = 0x01e901de}, {.w = 0x020001f5}, {.w = 0x0217020b}, {.w = 0x022e0222},
    {.w = 0x02440239}, {.w = 0x025b0250}, {.w = 0x02720266}, {.w = 0x0289027d}, {.w = 0x029f0294},
    {.w = 0x02b602ab}, {.w = 0x02cd02c1}, {.w = 0x02e402d8}, {.w = 0x02fa02ef}, {.w = 0x03110306},
    {.w = 0x0328031c}, {.w = 0x033f0333}, {.w = 0x0355034a}, {.w = 0x036c0361}, {.w = 0x03830377},
    {.w = 0x039a038e}, {.w = 0x03b003a5}, {.w = 0x03c703bc}, {.w = 0x03de03d2}, {.w = 0x03f503e9},
    {.w = 0x040b0400}, {.w = 0x04220417}, {.w = 0x0439042e}, {.w = 0x04500444}, {.w = 0x0466045b},
    {.w = 0x047d0472}, {.w = 0x04940489}, {.w = 0x04ab049f}, {.w = 0x04c104b6}, {.w = 0x04d804cd},
    {.w = 0x04ef04e4}, {.w = 0x050604fa}, {.w = 0x051c0511}, {.w = 0x05330528}, {.w = 0x054a053f},
    {.w = 0x05610555}, {.w = 0x0577056c}, {.w = 0x058e0583}, {.w = 0x05a5059a}, {.w = 0x05bc05b0},
    {.w = 0x05d205c7}, {.w = 0x05e905de}, {.w = 0x060005f5}, {.w = 0x0617060b}, {.w = 0x062e0622},
    {.w = 0x06440639}, {.w = 0x065b0650}, {.w = 0x06720666}, {.w = 0x0689067d}, {.w = 0x069f0694},
    {.w = 0x06b606ab}, {.w = 0x06cd06c1}, {.w = 0x06e406d8}, {.w = 0x06fa06ef}, {.w = 0x07110706},
    {.w = 0x0728071c}, {.w = 0x073f0733}, {.w = 0x0755074a}, {.w = 0x076c0761}, {.w = 0x07830777},
    {.w = 0x079a078e}, {.w = 0x07b007a5}, {.w = 0x07c707bc}, {.w = 0x07de07d2}, {.w = 0x07f507e9},
    {.w = 0xf80bf800}, {.w = 0xf822f817}, {.w = 0xf839f82e}, {.w = 0xf850f844}, {.w = 0xf866f85b},
    {.w = 0xf87df872}, {.w = 0xf894f889}, {.w = 0xf8abf89f}, {.w = 0xf8c1f8b6}, {.w = 0xf8d8f8cd},
    {.w = 0xf8eff8e4}, {.w = 0xf906f8fa}, {.w = 0xf91cf911}, {.w = 0xf933f928}, {.w = 0xf94af93f},
    {.w = 0xf961f955}, {.w = 0xf977f96c}, {.w = 0xf98ef983}, {.w = 0xf9a5f99a}, {.w = 0xf9bcf9b0},
    {.w = 0xf9d2f9c7}, {.w = 0xf9e9f9de}, {.w = 0xfa00f9f5}, {.w = 0xfa17fa0b}, {.w = 0xfa2efa22},
    {.w = 0xfa44fa39}, {.w = 0xfa5bfa50}, {.w = 0xfa72fa66}, {.w = 0xfa89fa7d}, {.w = 0xfa9ffa94},
    {.w = 0xfab6faab}, {.w = 0xfacdfac1}, {.w = 0xfae4fad8}, {.w = 0xfafafaef}, {.w = 0xfb11fb06},
    {.w = 0xfb28fb1c}, {.w = 0xfb3ffb33}, {.w = 0xfb55fb4a}, {.w = 0xfb6cfb61}, {.w = 0xfb83fb77},
    {.w = 0xfb9afb8e}, {.w = 0xfbb0fba5}, {.w = 0xfbc7fbbc}, {.w = 0xfbdefbd2}, {.w = 0xfbf5fbe9},
    {.w = 0xfc0bfc00}, {.w = 0xfc22fc17}, {.w = 0xfc39fc2e}, {.w = 0xfc50fc44}, {.w = 0xfc66fc5b},
    {.w = 0xfc7dfc72}, {.w = 0xfc94fc89}, {.w = 0xfcabfc9f}, {.w = 0xfcc1fcb6}, {.w = 0xfcd8fccd},
    {.w = 0xfceffce4}, {.w = 0xfd06fcfa}, {.w = 0xfd1cfd11}, {.w = 0xfd33fd28}, {.w = 0xfd4afd3f},
    {.w = 0xfd61fd55}, {.w = 0xfd77fd6c}, {.w = 0xfd8efd83}, {.w = 0xfda5fd9a}, {.w = 0xfdbcfdb0},
    {.w = 0xfdd2fdc7}, {.w = 0xfde9fdde}, {.w = 0xfe00fdf5}, {.w = 0xfe17fe0b}, {.w = 0xfe2efe22},
    {.w = 0xfe44fe39}, {.w = 0xfe5bfe50}, {.w = 0xfe72fe66}, {.w = 0xfe89fe7d}, {.w = 0xfe9ffe94},
    {.w = 0xfeb6feab}, {.w = 0xfecdfec1}, {.w = 0xfee4fed8}, {.w = 0xfefafeef}, {.w = 0xff11ff06},
    {.w = 0xff28ff1c}, {.w = 0xff3fff33}, {.w = 0xff55ff4a}, {.w = 0xff6cff61}, {.w = 0xff83ff77},
    {.w = 0xff9aff8e}, {.w = 0xffb0ffa5}, {.w = 0xffc7ffbc}, {.w = 0xffdeffd2}, {.w = 0xfff5ffe9},
    {.w = 0x00000000}};
