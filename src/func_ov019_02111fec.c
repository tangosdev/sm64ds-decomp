struct Vec3 { int x, y, z; };
short Vec3_HorzAngle(const struct Vec3 *v0, const struct Vec3 *v1);
int _Z14ApproachLinearRsss(short* dst, short target, short step);
unsigned char NumStars(void);
int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(void* thiz, void* actor, unsigned int msg, const struct Vec3* pos, unsigned int a, unsigned int b);
int _ZN6Player12GetTalkStateEv(void* p);
void func_02012790(int x);
void func_ov019_021122dc(void* c, int n);
int _ZN6Player18HasFinishedTalkingEv(void* p);

extern unsigned char data_0209d684;

#define LB(off) (*(unsigned char *)(((int)c + (off)) & 0xFFFFFFFFFFFFFFFF))

int func_ov019_02111fec(char* c) {
    switch (*(unsigned char*)(c + 0x38f)) {
    case 0: {
        char* tgt = *(char**)(c + 0x378);
        short ang = Vec3_HorzAngle((struct Vec3*)(c + 0x5c), (struct Vec3*)(tgt + 0x5c));
        if (_Z14ApproachLinearRsss((short*)(c + 0x8e), ang, 0x514) != 0) {
            struct Vec3 pos;
            int msg;
            int eq;
            int y;
            int z;
            pos.x = *(int*)(c + 0x5c);
            y = *(int*)(c + 0x60);
            pos.y = y;
            z = *(int*)(c + 0x64);
            pos.z = z;
            pos.y = y + 0x190000;
            eq = (int)(NumStars() == 0x96);
            if (eq != 0) msg = 0xab; else msg = 0xa7;
            if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(
                    *(void**)(c + 0x378), c, (unsigned int)(short)msg, &pos, 1, 2) != 0) {
                LB(0x38f) = LB(0x38f) + 1;
            }
        }
        break;
    }
    case 1:
        if (_ZN6Player12GetTalkStateEv(*(void**)(c + 0x378)) == 2) {
            unsigned char d = data_0209d684;
            if (d == 1) {
                func_02012790(0x98);
                func_ov019_021122dc(c, 2);
            } else if (d == 2) {
                func_02012790(0x63);
                *(unsigned char*)(c + 0x38e) = 0x5a;
                _ZN6Player18HasFinishedTalkingEv(*(void**)(c + 0x378));
                func_ov019_021122dc(c, 0);
            }
        }
        break;
    }
    return 1;
}
