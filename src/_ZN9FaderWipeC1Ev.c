#include "types.h"
struct Model {
    u32 data[0x14]; /* 0x50 bytes */
};

struct FaderWipe {
    u32* vtable;    /* 0x00 */
    Fix12i currInterp; /* 0x04 */
    Fix12i speed;      /* 0x08 */
    u16 color;         /* 0x0c */
    u16 unk0e;         /* 0x0e */
    struct Model model;/* 0x10 */
};

extern u32 data_0208eafc[];
extern u32 data_0208eacc[];
extern u32 data_0208eb2c[];
extern u32 _ZTV9FaderWipe[];
extern void _ZN5ModelC1Ev(struct Model*);

struct FaderWipe* _ZN9FaderWipeC1Ev(struct FaderWipe* this) {
    this->vtable = (u32*)data_0208eafc;
    this->vtable = (u32*)data_0208eacc;
    this->currInterp = 0x1000;
    this->speed = 0;
    this->vtable = (u32*)data_0208eb2c;
    this->color = 0;
    this->vtable = (u32*)_ZTV9FaderWipe;
    _ZN5ModelC1Ev(&this->model);
    return this;
}
