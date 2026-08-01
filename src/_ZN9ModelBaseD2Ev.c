/* _ZN9ModelBaseD2Ev at 0x020170b8
 * ModelBase destructor: write own vtable; if member at +4 is non-null,
 * call cleanup(member) (0x02018144); return this.
 */
struct ModelBase {
    void *vtable; /* 0x00 */
    void *res;    /* 0x04 */
};
extern void *data_0208e87c[];
extern void Deallocate(void *res); /* 0x02018144 */
struct ModelBase *_ZN9ModelBaseD2Ev(struct ModelBase *thiz)
{
    thiz->vtable = (void *)data_0208e87c;
    if (thiz->res != 0) {
        Deallocate(thiz->res);
    }
    return thiz;
}
