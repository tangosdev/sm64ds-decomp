//cpp
struct Actor;
struct MovingCylinderClsn {
    void Init(Actor *a, int b, int c, unsigned int d, unsigned int e);
};
extern "C" void func_ov022_02112790(void *c, void *p);
extern "C" char data_ov022_02114690;

extern "C" int func_ov022_021128b8(Actor *a)
{
    char *c = (char*)a;
    *(int*)(c + 0xa0) = -0xc8000;
    ((MovingCylinderClsn*)(c + 0xd4))->Init(a, 0x1e000, 0x1e000, 0x200002, 0);
    func_ov022_02112790((void*)a, (void*)&data_ov022_02114690);
    return 1;
}
