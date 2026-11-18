/*
 * Minigame-overlay data at 0x0213f9e4..0x0213fa04.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

int sTeresaPaintMask[8] = {
    0x00111100, 0x01111110, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x01111110, 0x00111100,
};
/* _ZTV13dScMgTeresa_c starts 8 bytes past this table; those bytes are its
   {offset-to-top, type_info} header. */
