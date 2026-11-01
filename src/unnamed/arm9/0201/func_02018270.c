/* recovered: the handle rides through r0. The body saves r1 and r2 and calls
 * func_02018a24 without touching r0, so the handle its caller (LoadFileAt) left
 * there is the one translated to a file id. The no-argument call was byte-exact
 * and passed nothing. */
extern unsigned int func_02018a24(unsigned int handle);
extern int func_020185c0(void *buf, int x);
extern int func_020184e0(void *buf, int a, int b);
/* Read up to b bytes of the asset handle's file into a (func_020184e0). */
int func_02018270(unsigned int handle, int a, int b) {
    char buf[0x44];
    unsigned int r = func_02018a24(handle);
    func_020185c0(buf, r);
    return func_020184e0(buf, a, b);
}
