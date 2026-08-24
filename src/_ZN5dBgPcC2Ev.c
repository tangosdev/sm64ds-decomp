#include "dBgPc.h"

void _ZN5dBgPcC2Ev(struct dBgPc *self)
{
    self->surface.clps.w0 = 0xfc0;
    self->surface.clps.w1 = 0xff;
    self->surface.normal.z = 0;
    self->surface.normal.y = self->surface.normal.z;
    self->surface.normal.x = self->surface.normal.y;
}
