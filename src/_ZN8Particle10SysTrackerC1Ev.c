// @symbol _ZN8Particle10SysTrackerC1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_Particle.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Particle.h"

extern char data_0208f3b4[];
extern void *data_0209ee74;

void *_ZN8Particle10SysTrackerC1Ev(struct Particle *self) {
    func_02021c90((char *)&self->unk_008);
    _ZN8Particle14SimpleCallbackC2Ev((char *)&self->mParticle1);
    _ZN8Particle14SimpleCallbackC2Ev((char *)&self->mParticle2);
    func_020226a4((char *)&self->unk_76c);
    *(void **)((char *)&self->unk_76c) = data_0208f3e4;
    func_020226a4((char *)&self->unk_778);
    *(void **)((char *)&self->unk_778) = data_0208f3e4;
    func_020226a4((char *)&self->unk_784);
    *(void **)((char *)&self->unk_784) = data_0208f3a4;
    func_020226a4((char *)&self->unk_790);
    *(void **)((char *)&self->unk_790) = data_0208f444;
    _ZN8Particle14SimpleCallbackC2Ev((char *)&self->mParticle3);
    _ZN8Particle14SimpleCallbackC2Ev((char *)&self->mParticle4);
    func_020225fc((char *)&self->unk_7b4);
    func_020225fc((char *)&self->unk_7c4);
    func_020225fc((char *)&self->unk_7d4);
    func_020225fc((char *)&self->unk_7e4);
    *(volatile void **)((char *)&self->unk_7f0) = data_0208f3b4;
    *(volatile void **)((char *)&self->unk_7f0) = data_0208f3f4;
    *(volatile void **)((char *)&self->unk_7f4) = data_0208f3b4;
    *(volatile void **)((char *)&self->unk_7f4) = data_0208f424;
    *(volatile void **)((char *)&self->unk_7f8) = data_0208f3b4;
    *(volatile void **)((char *)&self->unk_7f8) = data_0208f454;
    self->unk_7fc = 0x3000;
    *(volatile void **)((char *)&self->unk_800) = data_0208f3b4;
    *(volatile void **)((char *)&self->unk_800) = data_0208f454;
    self->unk_804 = 0x3000;
    func_020226a4((char *)&self->unk_808);
    *(void **)((char *)&self->unk_808) = data_0208f404;
    *(volatile void **)((char *)&self->unk_810) = data_0208f3b4;
    *(volatile void **)((char *)&self->unk_810) = data_0208f434;
    self->unk_814 = 1;
    *(volatile void **)((char *)&self->unk_818) = data_0208f3b4;
    *(volatile void **)((char *)&self->unk_818) = data_0208f464;
    data_0209ee74 = ((char *)self);
    self->unk_004 = 0;
    self->unk_750 = 0;
    self->unk_75c = 0;
    self->unk_768 = 0;
    self->unk_774 = 0;
    self->unk_780 = 0;
    self->unk_78c = 0;
    self->unk_798 = 0;
    self->unk_7a4 = 0;
    self->unk_7b0 = 0;
    self->unk_7c0 = 0;
    self->unk_804 = 0x4b000;
    return ((char *)self);
}
