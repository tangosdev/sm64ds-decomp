// @symbol func_ov013_021112a8
// recovered name: daObjClockHuriko_c_Behavior
/* recovered: renamed to Class_Method */
/* daObjClockHuriko_c::Behavior - recovered from vtable slot identity */
void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int, void*);
void func_ov013_02111238(char* t);
extern signed char data_02092110[];

int func_ov013_021112a8(char* c)
{
    if (data_02092110[0] <= 0) {
        short* p90 = (short*)(c + 0x90);
        /* The test re-derives +0x90 instead of reading through p90, and that is
           load-bearing under 2004/b56: the ROM tests the field directly
           (`ldrsh r0,[r4,#0x90]`) and only then materialises the pointer
           (`add r3,r4,#0x90`) for the read-modify-write below. Spelled as `*p90`,
           b56 folds the test into the pointer and emits the two in the other order. */
        if (*(short*)(c + 0x90) > 0) {
            *(short*)(((long long)(int)(c + 0x124))) -= 8;
        } else {
            *(short*)(((long long)(int)(c + 0x124))) += 8;
        }
        *p90 = (short)(*p90 + *(short*)(c + 0x124));
        short w = *(short*)(c + 0x124);
        if (w == 0x10 || w == -0x10) {
            _ZN5Sound9PlayBank3EjRK7Vector3(0x16, c + 0x74);
        }
    }
    func_ov013_02111238(c);

    return 1;
}
