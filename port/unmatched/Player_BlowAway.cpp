// PORT_HOST_ABI. Player::BlowAway, transcribed from the ROM.
// Run rel0215 wave 2, lane cast-ov027.
//
// ---- WHY A HOST COPY, AND WHY THIS IS THE SMALLEST REFUSAL ----------------
// src/func_ov027_02112170.cpp -- SNOWMAN_BREATH's per-element behaviour, the
// body that knocks the player back when the snowman's wind hits -- calls
//
//     extern void _ZN6Player8BlowAwayEs(void *o, short s);
//
// as a plain cdecl C name. Nothing in the port defines it, so the ov027 slice
// arrived at the link with one unresolved external (measured off the first
// link's own list, not guessed).
//
// The matched TU EXISTS -- src/_ZN6Player8BlowAwayEs.cpp, ov002 0x020d4fe4,
// size 0x54 -- and it is in no slice in the tree. Two reasons it cannot simply
// be added, both checked rather than assumed:
//
//   (1) It defines a real C++ METHOD, `void Player::BlowAway(short)`, so it
//       would put only ?BlowAway@Player@@QAEXF@Z in the link -- a __thiscall
//       symbol -- and the caller wants the cdecl C name. That alone is just a
//       face, the MotherPenguin/OneUpLogo shape.
//   (2) It does not compile in this tree. It reads `mPrevAngleY` and
//       `mAngleY` off include/Player.h, and Player.h declares NEITHER: that
//       header duplicates Actor's layout inline rather than inheriting it (its
//       own comment says so, "the fields below 0x0d0 are being reconciled with
//       Actor.h/ActorBase.h"), and it stops at mAngleYSpeed (0x69c). The two
//       angle fields live only in Actor.h, at 0x08e and 0x094.
//
// (2) is a DECOMP-SIDE HEADER GAP, not this lane's to fix -- this lane does not
// edit src/ or the shared headers -- so the entry point is refused and hosted.
// Refusing the whole call instead (a loud face that returns without doing
// anything) would have been ABI-safe, since the ROM body returns void, but it
// would silently delete the knock-back from Snowman's Land. The VS lane's
// correction is exactly this shape: refuse the SMALLEST subtree that is
// genuinely unmodellable. Six instructions of Player state are not it.
//
// ---- THE ROM BODY, ov002 0x020d4fe4, read out of ------------------------
// extracted/overlays/overlay_0002.bin at base 0x020ad660 (the .text start in
// config/arm9/overlays/ov002/delinks.txt), never dsd's copy, trap T4:
//
//   020d4fe4  e92d4000  push {lr}
//   020d4fe8  e24dd004  sub  sp,sp,#4
//   020d4fec  e5d026f9  ldrb r2,[r0,#0x6f9]      mIsMetal
//   020d4ff0  e3520000  cmp  r2,#0
//   020d4ff4  128dd004  addne sp,sp,#4           }
//   020d4ff8  18bd4000  popne {lr}               }  return if metal
//   020d4ffc  112fff1e  bxne  lr                 }
//   020d5000  e5d026fd  ldrb r2,[r0,#0x6fd]      mIsBalloon
//   020d5004  e3520000  cmp  r2,#0
//   020d5008  128dd004  addne sp,sp,#4           }
//   020d500c  18bd4000  popne {lr}               }  return if ballooned
//   020d5010  112fff1e  bxne  lr                 }
//   020d5014  e1c019b4  strh r1,[r0,#0x94]       mPrevAngleY = v
//   020d5018  e2812902  add  r2,r1,#0x8000       (imm 0x2 ror 18)
//   020d501c  e59f1010  ldr  r1,[pc,#16]         -> 0x0211037c
//   020d5020  e1c028be  strh r2,[r0,#0x8e]       mAngleY = v + 0x8000
//   020d5024  eb00381d  bl   020e30a0            Player::ChangeState
//   020d5028  e28dd004  add  sp,sp,#4
//   020d502c  e8bd4000  pop  {lr}
//   020d5030  e12fff1e  bx   lr
//   020d5034  0211037c  .word (the literal pool)
//
// Every name in it is confirmed rather than inferred:
//   * 0x020e30a0 is _ZN6Player11ChangeStateERNS_5StateE
//     (config/arm9/overlays/ov002/symbols.txt:1010), and
//     config/arm9/overlays/ov002/relocs.txt:4921 says the call at 0x020d5024
//     goes there. The port already carries that body as a cdecl C name.
//   * 0x0211037c is data_ov002_0211037c, the State record, and it is already
//     in port/ov002_syms.txt's mount.
//   * 0x6f9 mIsMetal and 0x6fd mIsBalloon are include/Player.h:236 and :240.
//   * 0x08e mAngleY and 0x094 mPrevAngleY are include/Actor.h:92 and :95.
// The ORDER is the ROM's: mPrevAngleY is written first, then mAngleY, then the
// state change -- which matters, because ChangeState reads both.
//
// The store is a strh, so the +0x8000 wraps in sixteen bits; that is written
// as an explicit truncation rather than left to promotion.

extern "C" {

void _ZN6Player11ChangeStateERNS_5StateE(void *self, void *state);
extern unsigned char data_ov002_0211037c[];

void _ZN6Player8BlowAwayEs(void *self, short v)
{
    char *c = (char *)self;
    if (*(unsigned char *)(c + 0x6f9))      /* mIsMetal */
        return;
    if (*(unsigned char *)(c + 0x6fd))      /* mIsBalloon */
        return;
    *(short *)(c + 0x94) = v;                                /* mPrevAngleY */
    *(short *)(c + 0x8e) = (short)((unsigned short)v + 0x8000u); /* mAngleY */
    _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211037c);
}

}
