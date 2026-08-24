/* PORT_HOST_ABI. dScMgD3DBase_c vtable SLOT 24, func_ov006_020e6e78, which MSVC
 * refuses to compile from src for a reason that has nothing to do with the body.
 *
 * SHARED FILE, NOT A LANE FILE, and the second half of a convergence: lanes BNP
 * (0x174) and BNT (0x175) of run mg11 each hit this independently. The filename
 * and the symbol are BNT's so the two trees dedupe at the merge; the ROM
 * adjudication behind it is recorded in port/tools/inferred_stub_adjudicated.txt
 * as a BASE row so 0x180 and 0x181 inherit it too.
 *
 * TWO THINGS IN ONE FILE, both mechanical:
 *
 *   1. include/decl_common.h:1516 declares func_ov006_020e6e78 `(void*)` inside
 *      its extern "C" block while src/func_ov006_020e6e78.cpp defines it
 *      `(char*)`. One register on the ROM, C2733 to MSVC -- the
 *      func_ov102_0214b248 case exactly. Taking the parameter as void* and
 *      casting inside is the whole of the fix.
 *   2. the TU writes *(int*)0x4001000 directly. That is safe on this host
 *      without a rewrite: port/ntr/include/ntr/mmio.h's mechanism 1 maps real
 *      memory across the whole DS I/O range, and this register is a plain latch
 *      whose write triggers no hardware behaviour, so the raw store lands.
 *
 * BNP's FIRST ROUTE THROUGH THIS WAS tools/hostgen.py's HEADER_SHADOW list plus
 * its MMIO rewrite, and that route is RETIRED in favour of this file. It worked,
 * and it was worse for the merge in two ways worth writing down: a HEADER_SHADOW
 * row is an edit to a SHARED TOOL that all four family lanes would have had to
 * add and then de-duplicate, and hostgen's generated object lands at a FIXED
 * output path, so two lanes each adding the add_custom_command for this symbol
 * is a CMake error rather than a dedupe.
 *
 * Everything below is src/func_ov006_020e6e78.cpp verbatim except the parameter
 * type and the one cast that follows from it. Every offset, constant, branch and
 * call is src's.
 */

#include "decl_common.h"

extern "C" int func_ov004_020ae140(void *self);
extern "C" void Camera_UpdateMatrices(int arg);

extern unsigned char data_0209f5f8;

extern "C" int func_ov006_020e6e78(void *cv)
{
    char *self = (char *)cv;

    if (func_ov004_020ae140(self) == 0) return 0;
    if (*(int *)(self + 0x4628) == 0) {
        if (data_0209d464 == 0) return 0;
        if (*(unsigned short *)(self + 0x4664) == 0)
            *(unsigned short *)(self + 0x4664) = 1;
        else
            *(unsigned short *)(self + 0x4664) = 0;
        int v = (int)(self + 0x466c + (*(unsigned short *)(self + 0x4664)) * 0xbc);
        data_ov006_02141a44 = v;
        Camera_UpdateMatrices(v);
        if (*(unsigned short *)(self + 0x4664) == 1) {
            func_ov006_020e7508();
        } else {
            func_ov006_020e759c();
            if (data_0209f5f8 == 0) {
                *(int *)0x4001000 &= ~0xe000;
                data_0209f5f8 = 1;
            }
        }
    }
    return 1;
}
