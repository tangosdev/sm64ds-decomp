//cpp
// @symbol _ZN3OAM4LoadEv
/* OAM::Load -- push the assembled OAM buffers to the hardware. STATIC: no
 * `this', the buffers are file globals.
 *
 * The single-buffer case (data_0209e660 set) sends one buffer to the main
 * engine and stops. Otherwise the two buffers swap roles per frame, selected by
 * bit 15 of DISPCAPCNT at 0x04000304, so that the one being displayed is never
 * the one being written.
 */
#include "OAM.h"

extern "C" void func_020566dc(const void *src, unsigned int offset, unsigned int count);
extern "C" void func_02056674(const void *src, unsigned int offset, unsigned int count);
extern "C" unsigned char data_0209e660;
extern "C" int data_0209e674[];
extern "C" int data_0209ea74[];

void OAM::Load()
{
    if (data_0209e660) {
        func_020566dc(data_0209e674, 0, 0x400);
        return;
    }
    if (((*(unsigned short *)0x4000304 & 0x8000) >> 15) == 1) {
        func_020566dc(data_0209e674, 0, 0x400);
        func_02056674(data_0209ea74, 0, 0x400);
    } else {
        func_020566dc(data_0209ea74, 0, 0x400);
        func_02056674(data_0209e674, 0, 0x400);
    }
}
