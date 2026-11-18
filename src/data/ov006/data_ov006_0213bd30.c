/* Pointers, 6 entries. */
#include "types.h"

extern void * data_ov006_02134fdc[];
extern struct OamAttr * data_ov006_02135018[];
extern struct OamAttr * data_ov006_0213509c[];
extern struct OamAttr * data_ov006_02136974[];
extern struct OamAttr * data_ov006_021369b0[];
extern struct OamAttr * data_ov006_021369ec[];

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
void ** sCardPayoutHighlightSprites[6] = {
    (void **)&data_ov006_021369ec,
    (void **)&data_ov006_021369b0,
    (void **)&data_ov006_02136974,
    (void **)&data_ov006_0213509c,
    (void **)&data_ov006_02135018,
    (void **)&data_ov006_02134fdc,
};
#pragma section obj end
