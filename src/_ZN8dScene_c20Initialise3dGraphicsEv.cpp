//cpp
/* dScene_c::Initialise3dGraphics() at 0x0202e6c8, 0x74 bytes.
 *
 * Static: BeforeInitResources reaches it with a bare `bl`, and it in turn calls
 * ResetHardwareRegisters the same way. Wipes the hardware back to a known state,
 * puts the 3D engine in mode 1, then sets the clear colour, clear depth and
 * viewport before handing off to the shared initialiser at 0x0205583c. */
#include "dScene_c.h"

extern "C" {
extern void func_0205583c(void);
extern void _ZN2GX15SetGraphicsModeEiii(int mode, int bg0_3d, int display);
extern void Initialise3dGraphics(int arg);
}

void dScene_c::Initialise3dGraphics()
{
    ResetHardwareRegisters();
    func_0205583c();
    _ZN2GX15SetGraphicsModeEiii(1, 0, 1);
    *(volatile u32 *)0x40004c8 = 0x296a5800;
    *(volatile u32 *)0x40004cc = 0x7fff;
    *(volatile u32 *)0x40004c0 = 0x7fff;
    *(volatile u32 *)0x40004c4 = 0;
    ::Initialise3dGraphics(0);
}
