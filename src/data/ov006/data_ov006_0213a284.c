/*
 * Minigame-overlay data at 0x0213a284..0x0213a5f4.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

extern struct OamAttr data_ov006_02139e2c[];
extern struct OamAttr data_ov006_02139eec[];
extern struct OamAttr data_ov006_02139f1c[];
extern struct OamAttr data_ov006_02139f4c[];
extern struct OamAttr data_ov006_02139f7c[];
extern struct OamAttr data_ov006_02139fac[];
extern struct OamAttr data_ov006_0213a18c[];

#pragma define_section obj ".data.gap38" ".data.gap38" ".data.gap38" far_data
#pragma section obj begin
u32 data_ov006_0213a284[18] = {
    (u32)&data_ov006_02139e2c,
    0x00000003,
    (u32)&data_ov006_0213a18c,
    0x00000003,
    (u32)&data_ov006_02139f4c,
    0x00000003,
    (u32)&data_ov006_02139f7c,
    0x00000005,
    (u32)&data_ov006_02139fac,
    0x00000014,
    (u32)&data_ov006_02139eec,
    0x00000005,
    (u32)&data_ov006_02139f1c,
    0x00000005,
    (u32)&data_ov006_02139eec,
    0x00000005,
    (u32)&data_ov006_02139fac,
    0x00000005,
};
#pragma section obj end
