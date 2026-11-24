#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef union {
    u32 w;
    void *p;
} Word;

extern Word data_ov006_02133600[];
extern Word data_ov006_02133610[];
extern Word data_ov006_02133620[];
extern Word data_ov006_02133630[];
extern Word data_ov006_02133648[];
extern Word data_ov006_02133660[];
extern Word data_ov006_02133678[];
extern Word data_ov006_02133690[];
extern Word data_ov006_021336a8[];
extern Word data_ov006_021336c0[];
extern Word data_ov006_021336d8[];
extern Word data_ov006_021336f0[];
extern Word data_ov006_02133708[];
extern Word data_ov006_02133720[];
extern Word data_ov006_02133738[];
extern Word data_ov006_02133750[];
extern Word data_ov006_02133768[];
extern Word data_ov006_02133780[];
extern Word data_ov006_02133798[];
extern Word data_ov006_021337b0[];
extern Word data_ov006_021337c8[];
extern Word data_ov006_021337e0[];
extern Word data_ov006_021337f8[];

#pragma define_section sRun ".data.gap0"                                                           \
                            ".data.gap0"                                                           \
                            ".data.gap0" far_data
#pragma section sRun begin

Word data_ov006_02133620[4] = {
    {.w = 0x81f000e8}, {.w = 0x00000000}, {.w = 0x81f04008}, {.w = 0xffff0080}};

Word data_ov006_02133610[4] = {
    {.w = 0x81f880e8}, {.w = 0x00000004}, {.w = 0x41f80008}, {.w = 0xffff0084}};

Word data_ov006_02133600[4] = {
    {.w = 0x41fc80e8}, {.w = 0x0000011f}, {.w = 0x01fc8008}, {.w = 0xffff019f}};

Word data_ov006_02133798[6] = {{.w = 0x81f880f0}, {.w = 0x0000200a}, {.w = 0x81f880e8},
                               {.w = 0x00000116}, {.w = 0x41f80008}, {.w = 0xffff0196}};

Word data_ov006_02133780[6] = {{.w = 0x81f000f0}, {.w = 0x0000200c}, {.w = 0x81f000e8},
                               {.w = 0x00000112}, {.w = 0x81f04008}, {.w = 0xffff0192}};

Word data_ov006_02133768[6] = {{.w = 0x81f880f0}, {.w = 0x00002010}, {.w = 0x81f880e8},
                               {.w = 0x00000116}, {.w = 0x41f80008}, {.w = 0xffff0196}};

Word data_ov006_02133738[6] = {{.w = 0x81f000f0}, {.w = 0x00001012}, {.w = 0x81f000e8},
                               {.w = 0x00000112}, {.w = 0x81f04008}, {.w = 0xffff0192}};

Word data_ov006_02133750[6] = {{.w = 0x81f880f0}, {.w = 0x00001016}, {.w = 0x81f880e8},
                               {.w = 0x00000116}, {.w = 0x41f80008}, {.w = 0xffff0196}};

Word data_ov006_02133720[6] = {{.w = 0x81f000f0}, {.w = 0x00001018}, {.w = 0x81f000e8},
                               {.w = 0x00000112}, {.w = 0x81f04008}, {.w = 0xffff0192}};

Word data_ov006_02133708[6] = {{.w = 0x81f780f0}, {.w = 0x0000101c}, {.w = 0x81f880e8},
                               {.w = 0x00000116}, {.w = 0x41f80008}, {.w = 0xffff0196}};

Word data_ov006_021336f0[6] = {{.w = 0x81ef00f0}, {.w = 0x00001086}, {.w = 0x81f000e8},
                               {.w = 0x00000112}, {.w = 0x81f04008}, {.w = 0xffff0192}};

Word data_ov006_021336d8[6] = {{.w = 0x81f780f0}, {.w = 0x0000108a}, {.w = 0x81f880e8},
                               {.w = 0x00000116}, {.w = 0x41f80008}, {.w = 0xffff0196}};

Word data_ov006_021336c0[6] = {{.w = 0x81f000f0}, {.w = 0x0000208c}, {.w = 0x81f000e8},
                               {.w = 0x00000112}, {.w = 0x81f04008}, {.w = 0xffff0192}};

Word data_ov006_021336a8[6] = {{.w = 0x81f880f0}, {.w = 0x00002090}, {.w = 0x81f880e8},
                               {.w = 0x00000116}, {.w = 0x41f80008}, {.w = 0xffff0196}};

Word data_ov006_02133690[6] = {{.w = 0x81f000f0}, {.w = 0x00002092}, {.w = 0x81f000e8},
                               {.w = 0x00000112}, {.w = 0x81f04008}, {.w = 0xffff0192}};

Word data_ov006_021337b0[6] = {{.w = 0x81f000f0}, {.w = 0x00002006}, {.w = 0x81f000e8},
                               {.w = 0x00000112}, {.w = 0x81f04008}, {.w = 0xffff0192}};

Word data_ov006_02133660[6] = {{.w = 0x81f000f0}, {.w = 0x00001098}, {.w = 0x81f000e8},
                               {.w = 0x00000112}, {.w = 0x81f04008}, {.w = 0xffff0192}};

Word data_ov006_02133648[6] = {{.w = 0x81f780f0}, {.w = 0x0000109c}, {.w = 0x81f880e8},
                               {.w = 0x00000116}, {.w = 0x41f80008}, {.w = 0xffff0196}};

Word data_ov006_02133678[6] = {{.w = 0x81f000f0}, {.w = 0x00002109}, {.w = 0x81f000e8},
                               {.w = 0x00000112}, {.w = 0x81f04008}, {.w = 0xffff0192}};

Word data_ov006_021337c8[6] = {{.w = 0x81f880f0}, {.w = 0x00002096}, {.w = 0x81f880e8},
                               {.w = 0x00000116}, {.w = 0x41f80008}, {.w = 0xffff0196}};

Word data_ov006_02133630[6] = {{.w = 0x81f000f0}, {.w = 0x0000110d}, {.w = 0x81f000e8},
                               {.w = 0x00000112}, {.w = 0x81f04008}, {.w = 0xffff0192}};

Word data_ov006_021337e0[6] = {{.w = 0x81f880f0}, {.w = 0x0000201e}, {.w = 0x81f880e8},
                               {.w = 0x00000116}, {.w = 0x41f80008}, {.w = 0xffff0196}};

Word data_ov006_021337f8[29] = {
    {.w = 0x81f780f0},          {.w = 0x0000109e},          {.w = 0x81f880e8},
    {.w = 0x00000116},          {.w = 0x41f80008},          {.w = 0xffff0196},
    {.p = data_ov006_02133620}, {.p = data_ov006_02133610}, {.p = data_ov006_02133600},
    {.p = data_ov006_021337b0}, {.p = data_ov006_02133798}, {.p = data_ov006_02133780},
    {.p = data_ov006_02133768}, {.p = data_ov006_02133738}, {.p = data_ov006_02133750},
    {.p = data_ov006_02133720}, {.p = data_ov006_02133708}, {.p = data_ov006_021336f0},
    {.p = data_ov006_021336d8}, {.p = data_ov006_021336c0}, {.p = data_ov006_021336a8},
    {.p = data_ov006_02133690}, {.p = data_ov006_021337c8}, {.p = data_ov006_02133660},
    {.p = data_ov006_02133648}, {.p = data_ov006_02133678}, {.p = data_ov006_021337e0},
    {.p = data_ov006_02133630}, {.p = data_ov006_021337f8}};

#pragma section sRun end
