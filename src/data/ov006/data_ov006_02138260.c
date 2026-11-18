/* Pointers, 4 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02138270[];
extern struct OamAttr data_ov006_02138288[];
extern struct OamAttr data_ov006_021382a0[];
extern struct OamAttr data_ov006_021382b8[];

#pragma define_section obj ".data.gap31" ".data.gap31" ".data.gap31" far_data
#pragma section obj begin
struct OamAttr * sSmartballLineSpritesSpanish[4] = {
    (struct OamAttr *)&data_ov006_021382a0,
    (struct OamAttr *)&data_ov006_02138288,
    (struct OamAttr *)&data_ov006_02138270,
    (struct OamAttr *)&data_ov006_021382b8,
};
#pragma section obj end
