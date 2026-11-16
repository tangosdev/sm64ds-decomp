#include "types.h"
extern int func_0206cf7c(int arg0);

/* optimize_for_size is this library's own build setting, not a matching lever:
   the ISD debug console and its soft-float helpers were shipped size-optimized. */
#pragma optimize_for_size on
/* Debug console byte sink, cartridge-slot form: store val in a 0x8000-byte
   ring at 0x09fd0000 (flag 0x2000 set) or 0x08fd0000, then rewrite the head
   until it reads back. The byte is dropped when the ring is full.
   func_0206cbc0 picks it when flag 1 is set. */
void func_0206cb0c(void *obj, int val) {
    volatile u16 *self = obj;
    u16 idx = self[0];
    u16 next = (idx + 1) & 0x7fff;
    unsigned char *bank;
    if (func_0206cf7c(0x2000)) {
        bank = (unsigned char *)0x09fd0000;
    } else {
        bank = (unsigned char *)0x08fd0000;
    }
    if (next != self[2]) {
        u16 *p = (u16 *)(bank + (idx & ~1));
        u16 cur = *p;
        u16 out;
        if ((idx & 1) != 0) {
            out = (cur & 0x00ff) | ((unsigned int)(val << 16) >> 8);
        } else {
            out = (cur & 0xff00) | ((unsigned int)(val << 16) >> 16);
        }
        *p = out;
        while (self[0] != next) {
            self[0] = next;
        }
    }
}
