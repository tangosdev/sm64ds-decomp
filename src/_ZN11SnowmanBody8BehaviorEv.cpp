//cpp
struct CylinderClsn { void Clear(); void Update(); };
extern "C" void func_ov072_0211fc3c(void *c);
extern "C" void func_ov072_0211f3e4(void *c);

extern "C" int _ZN11SnowmanBody8BehaviorEv(char *c)
{
    func_ov072_0211fc3c(c);
    ((CylinderClsn*)(c + 0x14c))->Clear();
    ((CylinderClsn*)(c + 0x14c))->Update();
    func_ov072_0211f3e4(c);
    return 1;
}
