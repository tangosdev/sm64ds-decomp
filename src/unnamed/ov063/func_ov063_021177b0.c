typedef struct { int x, y, z; } Vec3;

typedef struct {
    unsigned char pad[0xd4];
    unsigned short lo8 : 8;
    unsigned short flag : 1;
    unsigned short hi7 : 7;
} Sub500;

extern void* _ZN5Actor13ClosestPlayerEv(void* self);
extern int Vec3_Dist(const void* a, const void* b);
extern short Vec3_HorzAngle(const void* a, const void* b);
extern void func_020092c4(void* cam, void* out, void* tgt);
extern void _ZN6Camera9SetFlag_3Ev(void* cam);
extern int func_ov063_0211a3d0(char* c);
extern void func_ov063_0211adfc(char* p);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern int func_ov063_0211a564(char* c, int a);
extern void func_02012694(int a, void* b);
extern void* data_0209f318;
extern short data_02082214[];

void func_ov063_021177b0(char* c)
{
    if (*(unsigned short*)(c + 0x100) == 0) {
        unsigned char* p = (unsigned char*)(c + 0x5ca);
        *p = (unsigned char)(*p - 1);
    }

    if (*(unsigned char*)(c + 0x5ca) == 0) {
        void* cam = data_0209f318;
        unsigned short flags = *(unsigned short*)(c + 0x5d4);

        if (((unsigned)(flags << 21)) >> 31) {
            void* pl = _ZN5Actor13ClosestPlayerEv(c);
            if (pl != 0) {
                Vec3 v;
                Vec3 mid;
                Vec3 look;
                int dist;
                int scaled;
                short ang;
                short sn;
                int t;

                {
                    int* src = (int*)((char*)pl + 0x5c);
                    v.x = src[0];
                    v.y = src[1];
                    v.z = src[2];
                }
                dist = Vec3_Dist((char*)c + 0x5c, &v);
                if (((Sub500*)(c + 0x500))->flag) {
                    mid.x = (v.x - *(int*)(c + 0x5c)) / 2;
                } else {
                    mid.x = (*(int*)(c + 0x5c) + v.x) / 2;
                }
                mid.y = (*(int*)(c + 0x60) + v.y) / 2;
                mid.z = (*(int*)(c + 0x64) + v.z) / 2;
                scaled = (int)((((long long)dist << 12) + 0x800) >> 12);
                look.x = mid.x;
                look.y = mid.y;
                look.z = mid.z;
                look.x = mid.x - scaled;

                if (look.x <= (int)0xFF894000) {
                    ang = Vec3_HorzAngle(&v, (char*)c + 0x5c);
                    sn = data_02082214[(((unsigned short)ang >> 4) * 2) + 1];
                    t = (int)0xFF894000 - look.x;
                    mid.z = mid.z + (int)(((long long)t * sn + 0x800) >> 12);
                    look.x = (int)0xFF894000;
                }

                func_020092c4(cam, (char*)cam + 0x8c, &look);
                func_020092c4(cam, (char*)cam + 0x80, &mid);
            }
        } else {
            *(unsigned short*)(c + 0x5d4) |= 0x400;
            _ZN6Camera9SetFlag_3Ev(cam);
        }

        if (func_ov063_0211a3d0(c) == 0)
            return;

        {
            unsigned short* pf = (unsigned short*)(c + 0x5d4);
            *pf = (unsigned short)(*pf & ~8);
        }
        *(unsigned char*)(c + 0x5cc) = 8;
        func_ov063_0211adfc(c);
        *(unsigned short*)(c + 0x92) = 0;
        *(unsigned short*)(c + 0x94) = 0;
        *(unsigned short*)(c + 0x96) = 0;
        *(unsigned char*)(c + 0x5ce) = 0;

        {
            volatile Vec3 pos;
            char *b500 = c + 0x500;
            int px = *(int*)(c + 0x5c);
            pos.x = px;
            int py = *(int*)(c + 0x60);
            pos.y = py;
            pos.z = *(int*)(c + 0x64);
            pos.y = py + 0xc8000;
            {
                int yarg = pos.y;
                if (((unsigned)(*(unsigned short*)(b500 + 0xd4) << 23)) >> 31) {
                    int m = ~0;
                    pos.x = px * (volatile int)m;
                }
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x93, pos.x, yarg, pos.z);
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x94, pos.x, pos.y, pos.z);
            }
        }
        return;
    }

    if (*(unsigned short*)(c + 0x100) == 0) {
        int* p584 = (int*)(c + 0x584);
        int dec = 0x255;
        *p584 = *p584 - dec;
    }
    if (func_ov063_0211a564(c, 0x28) == 0)
        return;
    *(unsigned char*)(c + 0x5cc) = 6;
    {
        int* p19c = (int*)(c + 0x19c);
        *p19c |= 1;
    }
    *(unsigned char*)(c + 0x5c9) = 0;
    func_ov063_0211adfc(c);
    *(unsigned short*)(c + 0x5c2) = 0x78;
    func_02012694(0x153, c + 0x74);
}
