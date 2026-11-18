/*
 * Minigame-overlay data at 0x0213bb4c..0x0213bbb4.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

extern u8 _ZTI14dScMgBomroom_c[];
extern char data_ov006_02133c70[];
extern char data_ov006_02133c78[];
extern char data_ov006_02133c80[];
extern char data_ov006_02133c88[];
extern char data_ov006_02133c90[];
extern char data_ov006_02133c98[];
extern char data_ov006_02133ca0[];
extern char data_ov006_02133ca8[];
extern char data_ov006_02133cb0[];
extern char data_ov006_02133cb8[];
extern char data_ov006_02133cc0[];
extern char data_ov006_02133cc8[];
extern char data_ov006_02133ce8[];
extern char data_ov006_02133d50[];
extern char data_ov006_02133db8[];
extern char data_ov006_02133dc0[];
extern char data_ov006_02133dc8[];
extern char data_ov006_02133dd0[];
extern char data_ov006_02133dd8[];
extern char data_ov006_02133de0[];
extern char data_ov006_02133de8[];
extern char data_ov006_02133df0[];
extern char data_ov006_02133df8[];
extern char data_ov006_02133e00[];

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
void * sBomroomBombsSprites[26] = {
    (void *)&data_ov006_02133dd0,
    (void *)&data_ov006_02133dc8,
    (void *)&data_ov006_02133dc0,
    (void *)&data_ov006_02133db8,
    (void *)&data_ov006_02133e00,
    (void *)&data_ov006_02133df8,
    (void *)&data_ov006_02133df0,
    (void *)&data_ov006_02133de8,
    (void *)&data_ov006_02133de0,
    (void *)&data_ov006_02133dd8,
    (void *)&data_ov006_02133c98,
    (void *)&data_ov006_02133c90,
    (void *)&data_ov006_02133c88,
    (void *)&data_ov006_02133c80,
    (void *)&data_ov006_02133c78,
    (void *)&data_ov006_02133c70,
    (void *)&data_ov006_02133ca8,
    (void *)&data_ov006_02133cb8,
    (void *)&data_ov006_02133ca0,
    (void *)&data_ov006_02133cc0,
    (void *)&data_ov006_02133cc8,
    (void *)&data_ov006_02133cb0,
    (void *)&data_ov006_02133ce8,
    (void *)&data_ov006_02133d50,
    0,
    (void *)&_ZTI14dScMgBomroom_c,
};
#pragma section obj end
