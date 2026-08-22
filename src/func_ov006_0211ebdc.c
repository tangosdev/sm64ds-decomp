/* dScMgTeresa_c, "Hide and Boo Seek". Slot 7 of the sixteen-record state table
   data_ov006_02142ed8, dispatched by func_ov006_0211f6fc once per live record
   per frame with (this, record index). This is a Boo's ordinary tick: it ages
   the record's timers, walks the animation frame, moves and bounces the Boo,
   picks which way it faces, and runs the fade-out that retires it.

   THE RECORD IS 0x24 BYTES AT this + 0x4660 + i * 0x24, and the fields this
   body touches are the same ones src/func_ov006_0211ee34.c (slot 6 of the same
   table) and src/func_ov006_0211f5d4.c touch:

     +0x00 / +0x04  x and y, 20.12, read by the render loop
     +0x0c          the angle, 16-bit; func_ov006_0211f454 moves along it and
                    func_ov006_0211f34c reflects it off the screen edges
     +0x0e          the four-frame fade tick
     +0x10          the animation tick, 0..0xe
     +0x12          the turn timer
     +0x16          the fade level, 0..0x10
     +0x17          the tick gate func_ov006_0211f6fc reads
     +0x18          the state index func_ov006_0211f6fc dispatches on
     +0x1a          the RENDER gate src/func_ov006_0211e72c reads
     +0x1d          the animation frame, the sprite table's column
     +0x1e          the facing, the sprite table's row
     +0x21          the "this Boo may play its turn sound" flag

   THE FACING TEST IS THE SINE TABLE. data_02082214 is the arm9 sin/cos pair
   table src/Matrix4x3_FromRotationX.c and src/Camera_UpdateMatrices.c index
   the same way; entry 2*(angle >> 4) + 1 is the sine, so +0x1e is 1 while the
   Boo is drifting one way and 0 while it drifts the other. That byte is the
   ROW of the fourteen-pointer sprite table data_ov006_0213a964, which is two
   rows of seven, so this is the only thing that turns the Boo around on
   screen.

   THE FADE RETIRES THE BOO. Once the turn timer is spent the four-frame tick
   raises the fade level, the ONE record whose index equals the byte at
   this + 0x4c22 also drives the sub screen's blend register, and at 0x10 the
   body clears BOTH gates -- the tick gate at +0x17 and the render gate at
   +0x1a -- so the record stops being ticked and stops being drawn in the same
   frame.

   ---- TWO SPELLINGS ARE LOAD-BEARING AND BOTH ARE MEASURED ----------------

   1. THE THREE (this + i * 0x24) GROUPS ARE SPELT THREE DIFFERENT WAYS ON
      PURPOSE. The ROM computes `add rX, this, i*0x24` FOUR separate times
      (0x0211ebf4, 0x0211eca8, 0x0211ece8 and the conditional one at
      0x0211edec). Written with one spelling, mwcc commons the address into a
      fifth callee-saved register, keeps it alive across the two calls, and the
      body comes out two words short with a six-register push instead of the
      ROM's five. Three spellings -- `c + k`, `c + i * 0x24`, and the one cast
      -- give the compiler three expressions instead of one and it rebuilds the
      address at each group, which is what the ROM does. The cast is the only
      one in the file and it is here for that reason alone.

   2. THE LAST BLOCK USES `i * 0x24` RATHER THAN `k`. That is what puts the
      product in r4 and the state-byte address in r5, the way the ROM has them;
      with `k` throughout, the two are swapped and thirty-three words differ by
      a register number with every instruction otherwise identical.

   G2x::SetBlendAlpha's fourth argument is declared int here, not the unsigned
   short src/func_ov006_0211ee34.c uses, and that is not a discrepancy the
   reader should try to reconcile: ee34 passes a byte straight through, so no
   narrowing is needed, while this body passes 0x10 minus a byte. Under the
   all-unsigned-short prototype mwcc emits an lsl/lsr pair to truncate it and
   the ROM has none. src/func_ov002_020f1fcc.c already declares it exactly this
   way for the same reason. */
#include "types.h"
extern void func_ov006_0211f454(char *c, int i);
extern void func_ov006_0211f34c(char *c, int i);
extern void _ZN3G2x13SetBlendAlphaEPVttttt(volatile void *reg,
    unsigned short a, unsigned short b, int c, int d);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int id);
extern short data_02082214[];

void func_ov006_0211ebdc(char *c, int i)
{
    int k = i * 0x24;

    if (*(u16 *)(c + k + 0x4674) != 0) {
        (*(u16 *)(c + 0x4674 + k))--;
        if (*(u16 *)(c + 0x4674 + k) == 0)
            _ZN5Sound12PlayBank2_2DEj(0x1f1);
    }

    if (*(u8 *)(c + 0x467d + k) != 6) {
        (*(u16 *)(c + 0x4670 + k))++;
        if (*(u16 *)(c + 0x4670 + k) >= 0xf) {
            *(u16 *)(c + 0x4670 + k) = 0;
            (*(u8 *)(c + 0x467d + k))++;
            if (*(u8 *)(c + 0x467d + k) >= 6)
                *(u8 *)(c + 0x467d + k) = 3;
        }
    }

    func_ov006_0211f454(c, i);
    func_ov006_0211f34c(c, i);

    if (data_02082214[2 * (*(u16 *)(c + i * 0x24 + 0x466c) >> 4) + 1] >= 0)
        *(u8 *)(c + i * 0x24 + 0x467e) = 1;
    else
        *(u8 *)(c + i * 0x24 + 0x467e) = 0;

    if (*(u16 *)((char *)((int)c + k) + 0x4672) != 0) {
        (*(u16 *)(c + 0x4672 + k))--;
        if (*(u16 *)(c + 0x4672 + k) != 0)
            return;
        *(u16 *)(c + 0x466c + k) = *(u16 *)(c + 0x466c + k) + 0x8000;
        *(u8 *)(c + 0x467d + k) = 6;
        if (*(u8 *)((char *)((int)c + k) + 0x4681) != 0)
            _ZN5Sound12PlayBank2_2DEj(0x1f4);
        return;
    }

    (*(u16 *)(c + 0x466e + k))++;
    if (*(u16 *)(c + 0x466e + i * 0x24) < 4)
        return;

    (*(u8 *)(c + 0x4676 + i * 0x24))++;
    *(u16 *)(c + 0x466e + i * 0x24) = 0;

    if (i == *(u8 *)(c + 0x4c22))
        _ZN3G2x13SetBlendAlphaEPVttttt((volatile void *)0x4001050, 0, 4,
            0x10 - *(u8 *)(c + 0x4676 + i * 0x24), 0x10);

    if (*(u8 *)(c + 0x4676 + i * 0x24) >= 0x10) {
        *(u8 *)(c + i * 0x24 + 0x4677) = 0;
        *(u8 *)(c + i * 0x24 + 0x467a) = 0;
    }
}
