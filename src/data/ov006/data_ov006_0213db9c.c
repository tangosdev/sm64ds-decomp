/*
 * Minigame-overlay data at 0x0213db9c..0x0213dbbc.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

extern u8 _ZTI16dScMgPachinko2_c[];
extern int data_ov006_0212ebf8[];
extern int data_ov006_0212ec10[];
extern int data_ov006_0212ec18[];
extern int data_ov006_0212ec20[];
extern int data_ov006_0212ec28[];
extern int data_ov006_0212ec38[];

int *sPachinko2PaddleGatherTargets[8] = {
    (int *)&data_ov006_0212ec28,
    (int *)&data_ov006_0212ec20,
    (int *)&data_ov006_0212ec18,
    (int *)&data_ov006_0212ebf8,
    (int *)&data_ov006_0212ec10,
    (int *)&data_ov006_0212ec38,
    0,
    (int *)&_ZTI16dScMgPachinko2_c,
};
