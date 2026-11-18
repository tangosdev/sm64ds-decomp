/*
 * Minigame-overlay data at 0x0213ef8c..0x0213f4fc.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

char sSmartballLayoutNcgFmtSpanish[44] = "/MG/d_2d_mg_smart_ball_lay0%1d_S_ncg.bin";
