//cpp
// @symbol _ZN5dBgCh21ShouldPassThroughImplEPvRK4CLPSRKS_b
/* dBgCh::ShouldPassThroughImpl -- given a surface's CLPS and the query that hit
 * it, decide whether the query passes straight through. Each clause pairs a
 * surface property with the query's willingness to detect it: the moment a
 * property is present, the result is "solid" unless this query wants it.
 *
 * STATIC: four parameters in r0-r3, no `this', no stack argument.
 *
 * Both reference parameters are walked through raw pointers because neither
 * class is spelled out where this file can see it -- CLPS is only forward
 * declared, and the accessors on the dBgCh side are still unnamed func_ bodies
 * taking unsigned char *.
 *
 * The return type is free (mangling omits it) and both int and bool reproduce;
 * bool is the one that says what the function means.
 */
#include "decl_common.h"
#include "dBgCh.h"

extern "C" int SurfaceInfo_TestFlag0x20(int *p);
extern "C" int func_02037e38(int *p);

bool dBgCh::ShouldPassThroughImpl(void *p, const CLPS &clps_, const dBgCh &bg_, bool flag)
{
    int *clps = (int *)&clps_;
    unsigned char *bg = (unsigned char *)&bg_;
    int r4 = 0;

    if (SurfaceInfo_TestFlag0x20(clps)) {
        r4 = 1;
        if (func_0203547c(bg) == 0) return r4;
    }
    if (func_02037e20(clps)) {
        r4 = 1;
        if (func_0203543c(bg) == 0) return r4;
    }
    if (func_02037e2c(clps)) {
        r4 = 1;
        if (func_0203545c(bg) != 0) return r4;
    }
    if (func_02037e14(clps)) {
        r4 = 1;
        if (func_0203545c(bg) == 0) return r4;
    }
    if (func_02037e38(clps) == 0x11) {
        r4 = 1;
        if (func_02035408(bg) != 0) return r4;
    }
    if (flag != 0 && func_02037e38(clps) == 0x14) {
        r4 = 1;
        if (func_020353d4(bg) != 0) return r4;
    }
    if (r4 == 0) {
        if (func_020354b0(bg) == 0) return 1;
    }
    return 0;
}
