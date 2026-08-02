// @symbol _ZN15MaterialChangerC1Ev
/* Stays a mangled free function: the destructor is the key function.
 * See include/ModelBase.h. */
struct MaterialChanger {
    unsigned int *vtable;      /* 0x00 */
    char pad[0xc];
    unsigned int file;         /* 0x10 */
};
extern void _ZN9AnimationC2Ev(void *self);
extern unsigned int _ZTV15MaterialChanger[];

struct MaterialChanger *_ZN15MaterialChangerC1Ev(struct MaterialChanger *self) {
    _ZN9AnimationC2Ev(self);
    self->vtable = _ZTV15MaterialChanger;
    self->file = 0;
    return self;
}
