// @symbol func_ov080_02124eb0
// recovered name: MontyMole_Kill
/* recovered: renamed to Class_Method */
/* daChoropu_c::Kill - name recovered from the vtable slot it fills.
   The body is a decompilation verified against the ROM, not an
   inferred stub. */
extern void _ZN5dCc_c5ClearEv(void *);
int func_ov080_02124eb0(char *c)
{
    *(int *)(c + 0x98) = 0;
    _ZN5dCc_c5ClearEv((char *)c + 0x14c);
    *(int *)(c + 0x370) = 2;
    return 1;
}
