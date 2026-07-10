//cpp
struct Vector3 { int x, y, z; };

struct VObj {
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30();
    virtual void OnHit(); /* slot 31 = 0x7c */
};

extern "C" {
extern void _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(void* c, int a, int b);
extern int _ZN4cstd4fdivEii(int a, int b);
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern void _ZN9ActorBase18MarkForDestructionEv(void* c);
extern unsigned int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned int a, unsigned int b, int x, int y, int z, void* v, void* cb);
extern void* _ZN8Particle6System12FromUniqueIDEj(unsigned int id);
extern void func_02012694(int a, void* p);
extern void func_ov081_02127be0(void* c);
extern void _ZN12CylinderClsn5ClearEv(void* p);
extern void _ZN12CylinderClsn6UpdateEv(void* p);
}

extern "C" int _ZN8IceBlock8BehaviorEv(char* c)
{
    Vector3 v;
    int yo;
    void* s;
    char* p;
    unsigned int id;

    _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(c, 0, 0);

    if (*(unsigned char*)(c + 0x354) != 0) {
        *(int*)(c + 0x358) = _ZN4cstd4fdivEii(*(unsigned char*)(c + 0x354) << 12, 0x1e000);

        if (DecIfAbove0_Byte((unsigned char*)(c + 0x354)) == 0) {
            p = *(char**)(c + 0x364);
            if (p != 0) {
                if (*(unsigned short*)(p + 0xc) == 0xb2)
                    *(unsigned char*)(p + 0x49f) = 0;
            }
            _ZN9ActorBase18MarkForDestructionEv(c);
        } else {
            ((int*)&v)[0] = *(int*)(c + 0x5c);
            ((int*)&v)[1] = *(int*)(c + 0x60);
            ((int*)&v)[2] = *(int*)(c + 0x64);
            yo = (int)(((long long)*(int*)(c + 0x358) * 0x96000 + 0x800) >> 12);
            ((int*)&v)[1] = v.y + yo;

            *(unsigned int*)(c + 0x35c) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                *(unsigned int*)(c + 0x35c), 0x77, v.x, v.y, v.z, 0, 0);
            *(unsigned int*)(c + 0x360) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                *(unsigned int*)(c + 0x360), 0x78, v.x, v.y, v.z, 0, 0);

            id = *(unsigned int*)(c + 0x35c);
            if (id != 0) {
                s = _ZN8Particle6System12FromUniqueIDEj(id);
                if (s != 0)
                    *(int*)((char*)s + 0x44) = *(int*)(c + 0x358) * 0xf;
            }
            id = *(unsigned int*)(c + 0x360);
            if (id != 0) {
                s = _ZN8Particle6System12FromUniqueIDEj(id);
                if (s != 0)
                    *(int*)((char*)s + 0x44) = *(int*)(c + 0x358) * 0xf;
            }
        }
        return 1;
    }

    if (*(int*)(c + 0x344) != 0) {
        if ((*(int*)(c + 0x340) & 0x40000) != 0) {
            *(unsigned char*)(c + 0x354) = 0x1e;
            func_02012694(0x7a, c + 0x74);
            func_ov081_02127be0(c);
        }
        if ((*(int*)(c + 0x340) & 0x4000) != 0) {
            ((VObj*)c)->OnHit();
        }
    }

    _ZN12CylinderClsn5ClearEv(c + 0x320);
    _ZN12CylinderClsn6UpdateEv(c + 0x320);
    return 1;
}
