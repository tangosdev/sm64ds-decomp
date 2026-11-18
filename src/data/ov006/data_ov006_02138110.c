/* Pointers, 4 entries. */
#include "types.h"

extern struct OamAttr data_ov006_02138120[];
extern struct OamAttr data_ov006_02138138[];
extern struct OamAttr data_ov006_02138150[];
extern struct OamAttr data_ov006_02138168[];

#pragma define_section obj ".data.gap31" ".data.gap31" ".data.gap31" far_data
#pragma section obj begin
struct OamAttr * sSmartballLineSpritesFrench[4] = {
    (struct OamAttr *)&data_ov006_02138150,
    (struct OamAttr *)&data_ov006_02138138,
    (struct OamAttr *)&data_ov006_02138120,
    (struct OamAttr *)&data_ov006_02138168,
};
#pragma section obj end
