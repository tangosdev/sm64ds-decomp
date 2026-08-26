/* func_02037c40 at 0x02037c40 - a deleting destructor (D0) for a class with
 * several subobjects.
 *   [this+0]    = vtable (0x02099338)
 *   [this+0x10] = vtable (0x02099348)
 *   [this+0x38] = vtable (0x02099358)
 *   dBgPi::~dBgPi(this+0xc4)   (0x02038144)
 *   dBgPi::~dBgPi(this+0x9c)
 *   dBgPi::~dBgPi(this+0x74)
 *   _ZN8dM3dGSphD2Ev(this+0x38)
 *   _ZN5dBgPiD2Ev(this+0x10)
 *   _ZN5dBgChD2Ev(this)                  (base dtor)
 *   Memory::operator_delete2(this)       (0x0203cbcc)
 * returns this.
 */

struct Obj {
    void **vtable;     /* 0x00 */
    char pad0[0x10 - 4];
    void **vtable10;   /* 0x10 */
    char pad1[0x38 - 0x10 - 4];
    void **vtable38;   /* 0x38 */
};

extern void *data_02099338[];
extern void *data_02099348[];
extern void *data_02099358[];

extern void _ZN5dBgPiD1Ev(void *thiz);   /* 0x02038144 */
extern void _ZN8dM3dGSphD2Ev(void *thiz);          /* 0x0203ac1c */
extern void _ZN5dBgPiD2Ev(void *thiz);          /* 0x020380ec */
extern void _ZN5dBgChD2Ev(struct Obj *thiz);    /* 0x020354d0 */
extern void _ZN6Memory16operator_delete2EPv(void *ptr); /* 0x0203cbcc */

struct Obj *func_02037c40(struct Obj *thiz)
{
    thiz->vtable = (void **)data_02099338;
    thiz->vtable10 = (void **)data_02099348;
    thiz->vtable38 = (void **)data_02099358;
    _ZN5dBgPiD1Ev((char *)thiz + 0xc4);
    _ZN5dBgPiD1Ev((char *)thiz + 0x9c);
    _ZN5dBgPiD1Ev((char *)thiz + 0x74);
    _ZN8dM3dGSphD2Ev((char *)thiz + 0x38);
    _ZN5dBgPiD2Ev((char *)thiz + 0x10);
    _ZN5dBgChD2Ev(thiz);
    _ZN6Memory16operator_delete2EPv(thiz);
    return thiz;
}
