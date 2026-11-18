#include "types.h"
#include "nnsys/snd/player.h"
extern u16 data_020a4c48[];
extern int data_020a4c4c[];
extern int data_020a4c54[];
/* The volume deltas below are declared in mwcc's .rodata emission order
   (size-ordered, last-declared last), which is the ROM's address order. */
extern const int data_0208220c;
extern const int data_02082208;
extern const int data_02082204;
extern const int data_02082200;
extern const int data_02082210;

/* func_0204921c's twin, for the same sequences and modes: the target volume
   (data_020a4c54) and per-step delta (data_020a4c4c, from the constants
   data_02082200..data_02082210) of each track mask. The per-frame sound
   update func_020132d8 (Sound.cpp) calls it on the handle at sMusicHandle
   with func_02013078's mode. */
void func_020494cc(NNSSndHandle *a, int mode) {
    NNSSndSeqPlayer *p;
    int id;

    if (a == 0)
        return;
    p = a->player;
    if (p == 0)
        return;

    id = p->seqNo;

    if (mode == 0) {
        if (id != 0x3b)
            return;
        data_020a4c54[0] = 0x7f000;
        data_020a4c4c[0] = data_02082200;
        data_020a4c48[0] = 3;
        data_020a4c54[1] = 0;
        data_020a4c4c[1] = -data_02082200;
        data_020a4c48[1] = 0x3ffc;
    } else if (mode == 1) {
        if (id != 0x3b)
            return;
        data_020a4c54[0] = 0;
        data_020a4c4c[0] = -data_02082200;
        data_020a4c48[0] = 3;
        data_020a4c54[1] = 0;
        data_020a4c4c[1] = -data_02082200;
        data_020a4c48[1] = 0x3ffc;
    } else if (mode == 2) {
        if (id != 0x3b)
            return;
        data_020a4c54[0] = 0x7f000;
        data_020a4c4c[0] = data_02082204;
        data_020a4c48[0] = 3;
        data_020a4c54[1] = 0x7f000;
        data_020a4c4c[1] = data_02082204;
        data_020a4c48[1] = 0x3ffc;
    } else if (mode == 3) {
        if (id != 0x3d)
            return;
        data_020a4c54[0] = 0x7f000;
        data_020a4c4c[0] = data_02082210;
        data_020a4c48[0] = 0x70;
    } else if (mode == 4) {
        if (id != 0x3d)
            return;
        data_020a4c54[0] = 0;
        data_020a4c4c[0] = -data_0208220c;
        data_020a4c48[0] = 0x70;
    } else if (mode == 5) {
        if (id != 0x3e)
            return;
        data_020a4c54[0] = 0;
        data_020a4c4c[0] = -data_02082208;
        data_020a4c48[0] = 8;
        data_020a4c54[1] = 0x7f000;
        data_020a4c4c[1] = data_02082208;
        data_020a4c48[1] = 0x100;
    } else {
        if (mode != 6)
            return;
        if (id != 0x3e)
            return;
        data_020a4c54[0] = 0x7f000;
        data_020a4c4c[0] = data_02082208;
        data_020a4c48[0] = 8;
        data_020a4c54[1] = 0;
        data_020a4c4c[1] = -data_02082208;
        data_020a4c48[1] = 0x100;
    }
}

/* The per-track volume deltas, one constant per ramp above: 0xba (mode 0/1),
   0x1451, 0x1451, 0x1451, 0xcb33. */
const int data_0208220c = 0x1451;
const int data_02082208 = 0x1451;
const int data_02082204 = 0x0a28;
const int data_02082200 = 0x1451;
const int data_02082210 = 0xcb33;
