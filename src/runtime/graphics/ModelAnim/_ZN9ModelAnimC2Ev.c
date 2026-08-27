// @symbol _ZN9ModelAnimC2Ev
/* Stays a mangled free function: the destructor is ModelAnim's key
 * function, and with MI both vtables AND the _ZThn80_ thunks are emitted
 * with it, so the structors staying C files keeps all four ROM-supplied.
 * See include/ModelAnim.h. */
struct ModelAnim {
    unsigned int *vtable;          /* 0x00 */
    char pad_model[0x4c];
    unsigned int *anim_vtable;     /* 0x50 */
    char pad_anim[0xc];
    unsigned int file;             /* 0x60 */
};
extern struct ModelAnim *_ZN5ModelC2Ev(struct ModelAnim *self);
extern void _ZN9AnimationC2Ev(void *anim);
extern unsigned int _ZTV9ModelAnim[];
extern unsigned int VTable_Animation_ModelAnimThunk[];

struct ModelAnim *_ZN9ModelAnimC2Ev(struct ModelAnim *self) {
    _ZN5ModelC2Ev(self);
    _ZN9AnimationC2Ev((char *)self + 0x50);
    self->vtable = _ZTV9ModelAnim;
    self->anim_vtable = VTable_Animation_ModelAnimThunk;
    self->file = 0;
    return self;
}
