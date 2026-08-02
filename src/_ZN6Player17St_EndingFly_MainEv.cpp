//cpp
// @symbol _ZN6Player17St_EndingFly_MainEv
/* NOT a Player method. ov007, 0x020c3d1c -- the only ov007 symbol carrying a
 * Player name. It reads no fields at all; it just calls a function pointer held
 * in ov007 data, so nothing here identifies the object as a Player.
 *
 * Detached from Player.h; see _ZN6Player23St_InYoshiMouth_CleanupEv.cpp.
 */
extern "C" {
typedef void (*FP)(void*);
extern FP data_ov007_02103254;

void _ZN6Player17St_EndingFly_MainEv(void *self)
{
    data_ov007_02103254(self);
}
}
