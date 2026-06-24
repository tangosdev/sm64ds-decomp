//cpp
extern "C" {
extern void _Z14ApproachLinearRiii(int* p, int target, int step);
extern void _ZN8PathLift12BaseBehaviorEv(void* c);
extern void _ZN9Animation7AdvanceEv(void* a);
extern void func_ov036_021122c0(char* c);
extern void func_ov036_0211224c(char* c);
extern void _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void* t, int a, int b);
}

extern "C" int func_ov036_021123c8(char* c){
    if (*(unsigned char*)(c + 0x42a) != 0)
        _Z14ApproachLinearRiii((int*)(c + 0x4bc), -0x14000, 0x5000);
    else
        _Z14ApproachLinearRiii((int*)(c + 0x4bc), 0, 0x5000);
    _ZN8PathLift12BaseBehaviorEv(c);
    _ZN9Animation7AdvanceEv(c + 0x4a0);
    func_ov036_021122c0(c);
    func_ov036_0211224c(c);
    _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0);
    return 1;
}
