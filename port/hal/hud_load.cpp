// THE HUD's LOADER SEAM (lane HUD-INITRES).
//
// src/_ZN3HUD13InitResourcesEv.cpp is linked as the matched TU, through
// hostgen (--extern-data plus the CALLEE_SEAM table, gate 27 in
// CMakeLists.txt). Its body is the ROM's loader shape, twenty-five times:
//
//     h = LoadFile(0x229);                  the file, STILL COMPRESSED
//     DecompressLZ16(h, vram_b + 0x2000);   expanded into VRAM by the caller
//     Deallocate(h);
//
// and for the palettes, LoadFile then GX::LoadOBJPltt / GXS::LoadOBJPltt on
// the raw bytes. Three of the names that body calls resolve HERE rather than
// to the port's general definitions, for two reasons that belong to the port
// and not to the body:
//
//  1. DOUBLE DECOMPRESSION. The port's LoadFile (hal/level_boot.cpp) is
//     SharedFilePtr-backed and hands back DECODED bytes, so the DecompressLZ16
//     that follows it would LZ-decode an already-decoded buffer straight into
//     VRAM. func_0201817c (hal/fs.cpp, gate 25) is the port's raw loader: the
//     file's bytes on the game heap, still compressed, exactly the ROM's
//     func_0201818c contract -- and a fresh allocation per call, which is what
//     makes the body's trailing Deallocate correct rather than a free of a
//     cached SharedFilePtr image.
//
//  2. A MISSING FILE IS NOT FATAL HERE. This extraction is short of the
//     catalog, and the HUD's ARCHIVE members belong to the gap: 0xa003/a007/
//     a00a/a00e, 0xac03, 0xb00e, 0x8000..0x8003 and 0x980f (cee.narc and the
//     VS set). The raw loader returns null for them, and the matched body --
//     correctly, on a cartridge -- has no null path, so a plain link faults on
//     the first level boot inside DecompressLZ16. The seam says so once and
//     hands back null; the two consumers that would read it skip a null and
//     otherwise call the port's own DecompressLZ16 / LoadOBJPltt unchanged.
//     The bottom screen's own tiles and palette (0x229, 0x22a) are present, so
//     the panel keeps real graphics rather than no game. Deallocate is NOT
//     routed: src/Deallocate.c already tolerates a null.
//
// These are, line for line, the file-static helpers that
// port/unmatched/HUD_InitResources.cpp carried (hud_load / hud_decomp /
// hud_gx_pal / hud_gxs_pal); the HUD body they wrapped has gone back to src/.
// Both notes retire together the day the extraction is complete: then the
// LoadFile seam is func_0201817c and nothing else, and the null checks go.
#include <cstdio>

extern "C" {
void *func_0201817c(unsigned handle);                    /* hal/fs.cpp */
void DecompressLZ16(void *src, void *dst);               /* hal/fs.cpp */
void _ZN2GX11LoadOBJPlttEPKvjj(const void *, unsigned, unsigned);   /* src */
void _ZN3GXS11LoadOBJPlttEPKvjj(const void *, unsigned, unsigned);  /* src */

/* LoadFile, as the HUD's body spells it: the file still compressed, or null
   with one line when the extraction does not carry it. */
void *port_hud_loadfile(int handle)
{
    void *p = func_0201817c((unsigned)handle);
    if (!p) {
        static int said;
        if (said < 8) {
            ++said;
            std::printf("  [hud] file 0x%x is not in this extraction; skipped\n",
                        handle);
        }
    }
    return p;
}

/* DecompressLZ16(h, addr), a skipped load being a skipped decode. */
void port_hud_lz16(void *h, int addr)
{
    if (h)
        DecompressLZ16(h, (void *)addr);
}
} /* extern "C" */

/* GX::LoadOBJPltt / GXS::LoadOBJPltt, a skipped load being a skipped palette.
   The TU declares its callee inside `namespace GX { }` (C++ linkage, the
   shape hal/sub_actors.cpp aliases for the render leaves), so the seam is
   spelled in the same namespaces and calls the ROM-named C definitions. */
namespace GX {
void port_hud_objpltt(const void *p, unsigned offset, unsigned size)
{
    if (p)
        _ZN2GX11LoadOBJPlttEPKvjj(p, offset, size);
}
}
namespace GXS {
void port_hud_objpltt(const void *p, unsigned offset, unsigned size)
{
    if (p)
        _ZN3GXS11LoadOBJPlttEPKvjj(p, offset, size);
}
}

/* The TU's shadow declaration of Player::GetHealth returns u8; the matched
   definition (src/_ZN6Player9GetHealthEv.cpp) returns int, and MSVC puts the
   return type in a member's decorated name. Same receiver (ECX, both
   __thiscall), no arguments, the value lands in eax either way and the body
   stores it to a u8 -- a name bridge, which is what an /alternatename is for
   (hal/cxx_aliases.cpp). On ARM both spellings are r0. */
#pragma comment(linker, "/alternatename:?GetHealth@Player@@QAEEXZ=?GetHealth@Player@@QAEHXZ")
