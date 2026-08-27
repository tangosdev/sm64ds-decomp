//cpp
// @symbol _ZN3G3X11SetFogTableEPv
/* G3X::SetFogTable(void*) at 0x020555bc -- upload the 32-entry fog density
 * table to the 3D engine.
 *
 * 0x04000360 is FOG_TABLE in the DS I/O map, and the copy is exactly 32 bytes,
 * which is the whole table. The destination stays a bare address because the
 * I/O registers have no header in this tree yet; naming it is a claim of its
 * own and migration is per-reference.
 *
 * Verified layout-free against the ROM, not just against the audit: this
 * function's address is never stored as a word anywhere in arm9_dec.bin. */
extern "C" void Copy32Bytes(void* src, void* dst);

namespace G3X {

void SetFogTable(void* table)
{
    Copy32Bytes(table, (void*)0x4000360);
}

}
