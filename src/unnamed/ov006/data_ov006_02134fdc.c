#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
    void *f004;
    void *f008;
} data_ov006_02134fdc_Layout;
typedef char data_ov006_02134fdc_size[sizeof(data_ov006_02134fdc_Layout) == 0xc ? 1 : -1];

extern data_ov006_02134fdc_Layout data_ov006_02134fdc;
extern u8 data_ov006_02134ff8[];
extern u8 data_ov006_02134fe8[];
extern u8 data_ov006_02135008[];

#pragma define_section sName ".data.gap18"                                                         \
                             ".data.gap18"                                                         \
                             ".data.gap18" far_data
#pragma section sName begin

data_ov006_02134fdc_Layout data_ov006_02134fdc = {
    &data_ov006_02134ff8[0],
    &data_ov006_02134fe8[0],
    &data_ov006_02135008[0],
};
