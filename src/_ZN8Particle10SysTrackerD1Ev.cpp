//cpp
// @symbol _ZN8Particle10SysTrackerD1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Particle.h"
extern "C" void func_0203cbc0(void *p);
extern void *data_0209ee74;

extern "C" void *_ZN8Particle10SysTrackerD1Ev(struct Particle *self) {
    if (data_0209ee80 != 0) {
        func_02021b98((char *)&self->unk_008);
        func_0203cbc0(data_0209ee80);
        data_0209ee80 = 0;
    }
    if (*(void **)((char *)self) != (void *)data_02075f14) {
        _ZN6Memory10DeallocateEPv(*(void **)((char *)self));
    }
    data_0209ee74 = 0;
    return ((char *)self);
}
