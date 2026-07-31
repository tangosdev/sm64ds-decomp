// @symbol func_ov080_02124eb0
// @emits MontyMole_Kill
/* recovered: renamed to Class_Method */
/* daChoropu_c::Kill - recovered from vtable slot identity */
extern void _ZN12CylinderClsn5ClearEv(void *);
int MontyMole_Kill(char *c)
{
    *(int *)(c + 0x98) = 0;
    _ZN12CylinderClsn5ClearEv((char *)c + 0x14c);
    *(int *)(c + 0x370) = 2;
    return 1;
}
