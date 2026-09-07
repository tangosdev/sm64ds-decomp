//cpp
struct Vector3;
struct Vector3_16;

struct dActor_c {
    static dActor_c* Spawn(unsigned int a, unsigned int b, const Vector3& pos,
                        const Vector3_16* rot, signed char e, short f);
};
struct fBase_c {
    void MarkForDestruction();
};

extern "C" void func_0201267c(int a, char* p);

extern unsigned short data_ov098_0213bf90[];

extern "C" void func_ov098_0213b63c(char* c)
{
    int i;
    for (i = 0; i < 5; i++) {
        dActor_c* a = dActor_c::Spawn(0xd0, 2, *(const Vector3*)(c + 0x5c),
                                (const Vector3_16*)0,
                                *(signed char*)(c + 0xcc), -1);
        *(int*)((char*)a + 0xa4) = 0;
        *(int*)((char*)a + 0xa8) = 0x14000;
        *(int*)((char*)a + 0xac) = 0;
        *(int*)((char*)a + 0x98) = 0x14000;
        *(short*)((char*)a + 0x92) = 0;
        *(unsigned short*)((char*)a + 0x94) = data_ov098_0213bf90[i];
        *(short*)((char*)a + 0x96) = 0;
    }
    func_0201267c(0xda, c + 0x74);
    ((fBase_c*)c)->MarkForDestruction();
}
