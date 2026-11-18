/*
 * Minigame-overlay data at 0x0213bb08..0x0213bb38.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

extern u32 data_ov006_0213b9b4;
extern u32 data_ov006_0213b9c0;
extern u32 data_ov006_0213b9cc;
extern u32 data_ov006_0213b9d4[];

#pragma define_section obj ".data.w13b814" ".data.w13b814" ".data.w13b814" far_data
#pragma section obj begin
unsigned char * sBomroomBombFrameTimes[4] = {
    (unsigned char *)&data_ov006_0213b9b4,
    (unsigned char *)&data_ov006_0213b9c0,
    (unsigned char *)&data_ov006_0213b9cc,
    (unsigned char *)&data_ov006_0213b9d4,
};
#pragma section obj end
