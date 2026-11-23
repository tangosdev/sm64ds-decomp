#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 f000;
    void *f004;
    u32 f008;
} _ZTI16daObjCtMecha10_c_Layout;
typedef char _ZTI16daObjCtMecha10_c_size[sizeof(_ZTI16daObjCtMecha10_c_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[5];
} _ZTS16daObjCtMecha10_c_Layout;
typedef char _ZTS16daObjCtMecha10_c_size[sizeof(_ZTS16daObjCtMecha10_c_Layout) == 0x14 ? 1 : -1];

extern _ZTI16daObjCtMecha10_c_Layout _ZTI16daObjCtMecha10_c;
extern _ZTS16daObjCtMecha10_c_Layout _ZTS16daObjCtMecha10_c;

#pragma define_section sName ".data.m35Mid2"                                                       \
                             ".data.m35Mid2"                                                       \
                             ".data.m35Mid2" far_data
#pragma section sName begin

_ZTI16daObjCtMecha10_c_Layout _ZTI16daObjCtMecha10_c = {
    0x0209a764,
    &_ZTS16daObjCtMecha10_c,
    0x021089ec,
};

_ZTS16daObjCtMecha10_c_Layout _ZTS16daObjCtMecha10_c = {{
    0x61643631,
    0x436a624f,
    0x63654d74,
    0x30316168,
    0x0000635f,
}};
