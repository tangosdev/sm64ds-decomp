#ifndef DBGPC_H
#define DBGPC_H

#include "SurfaceInfo.h"

/* Common polygon information carried by collision results.
 *
 * The ROM's RTTI names this type dBgPc and records it as dBgPi's sole base.
 * Because dBgPi introduces the first virtual function in the hierarchy,
 * mwccarm places dBgPi's vptr at 0x00 and this non-polymorphic base at 0x04.
 * Its one data member is the five-word surface record copied into dBgPi by
 * dBgPi::RecordHit and initialized by the constructor below.
 */
struct dBgPc {
    struct SurfaceInfo surface;    /* 0x00 */

    dBgPc();
    ~dBgPc();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dBgPc_size_must_be_0x14[sizeof(struct dBgPc) == 0x14 ? 1 : -1];
#endif

#endif /* DBGPC_H */
