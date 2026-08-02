//cpp
// @symbol _ZN6Player23St_InYoshiMouth_CleanupEv
/* NOT a Player method, despite the name. ov006, 0x020d6084.
 *
 * sizeof(Player) is 0x768 (its allocating constructor _ZN6PlayerC3Ev asks
 * operator new for exactly that). This function writes this+0x62ad and
 * this+0x62af -- roughly 0x5b00 bytes past the end of the object. It is some
 * other ov006 class that happens to sit at a shared RAM address; the Player
 * name is a community label applied to the wrong overlay's bytes.
 *
 * Detached from Player.h so it stops contributing false evidence to that
 * header. Kept under the mangled name because renaming the symbol is a config
 * change; this commit is src-only.
 */
extern "C" void _ZN6Player23St_InYoshiMouth_CleanupEv(char *self)
{
    *(char *)(self + 0x62ad) = 0;
    *(char *)(self + 0x62af) = 0;
}
