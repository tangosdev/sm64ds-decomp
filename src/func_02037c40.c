/* func_02037c40 at 0x02037c40 - a deleting destructor (D0) for a class with
 * several subobjects.
 *   [this+0]    = vtable (0x02099338)
 *   [this+0x10] = vtable (0x02099348)
 *   [this+0x38] = vtable (0x02099358)
 *   ClsnResult::~ClsnResult(this+0xc4)   (0x02038144)
 *   ClsnResult::~ClsnResult(this+0x9c)
 *   ClsnResult::~ClsnResult(this+0x74)
 *   func_0203ac1c(this+0x38)
 *   func_020380ec(this+0x10)
 *   func_020354d0(this)                  (base dtor)
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

extern void _ZN10ClsnResultD1Ev(void *thiz);   /* 0x02038144 */
extern void func_0203ac1c(void *thiz);          /* 0x0203ac1c */
extern void func_020380ec(void *thiz);          /* 0x020380ec */
extern void func_020354d0(struct Obj *thiz);    /* 0x020354d0 */
extern void _ZN6Memory16operator_delete2EPv(void *ptr); /* 0x0203cbcc */

struct Obj *func_02037c40(struct Obj *thiz)
{
    thiz->vtable = (void **)data_02099338;
    thiz->vtable10 = (void **)data_02099348;
    thiz->vtable38 = (void **)data_02099358;
    _ZN10ClsnResultD1Ev((char *)thiz + 0xc4);
    _ZN10ClsnResultD1Ev((char *)thiz + 0x9c);
    _ZN10ClsnResultD1Ev((char *)thiz + 0x74);
    func_0203ac1c((char *)thiz + 0x38);
    func_020380ec((char *)thiz + 0x10);
    func_020354d0(thiz);
    _ZN6Memory16operator_delete2EPv(thiz);
    return thiz;
}
