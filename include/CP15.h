#ifndef SM64DS_CP15_H
#define SM64DS_CP15_H

/* CP15 -- the ARM946E-S system-control coprocessor primitives that the Nitro
 * SDK sources do not name. The cache, protection-unit, region-1, DTCM and
 * halt primitives carry their SDK names in nitro/os/cache.h,
 * protectionUnit.h, protectionRegion.h, tcm.h and system.h. Parameters stay
 * unnamed: a name here takes an @NNN in every includer. */
namespace CP15 {
/* Protection unit data region 7 (src/engine/cp15/CP154.cpp), inside the
 * IS-NITRO debugger library's run rather than the SDK's os_protectionRegion.c
 * one. */
void MPUDataRegion7(unsigned int);
unsigned int MPUGetDataRegion7(void);
}

#endif
