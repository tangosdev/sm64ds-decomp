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
        if (*p90 > 0) {
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
