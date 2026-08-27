//cpp
// @symbol func_ov007_020c3d1c
/* NOT a Player method. ov007, 0x020c3d1c -- the only ov007 symbol carrying a
 * Player name. It reads no fields at all; it just calls a function pointer held
 * in ov007 data, so nothing here identifies the object as a Player.
 *
 * Detached from Player.h; see func_ov006_020d6084.cpp.
 */
extern "C" {
typedef void (*FP)(void*);
extern FP data_ov007_02103254;

void func_ov007_020c3d1c(void *self)
{
    data_ov007_02103254(self);
}
}
