typedef struct { int x, y, z; } Vec3;

extern int Vec3_Dist(void *a, void *b);
extern int _ZN6Player9StartTalkER9ActorBaseb(void *actor, void *self, int b);
extern short Vec3_HorzAngle(void *a, void *b);
extern int _Z14ApproachLinearRsss(short *p, short target, short step);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *self, void *btp, int a, int fix, unsigned int b);
extern int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(void *actor, void *ab, unsigned int id, void *pos, unsigned int a, unsigned int b);
extern int _ZN6Player12GetTalkStateEv(void *actor);
extern void _ZN5Actor19UntrackAndSpawnStarERajRK7Vector3j(void *self, signed char *a, unsigned int b, void *pos, unsigned int d);
extern int _ZN6Player18HasFinishedTalkingEv(void *actor);
extern int func_0201267c(int a, void *pos);

extern void *data_ov019_021134a8[];
extern void *data_ov019_02113468[];

#define LB(off) (*(unsigned char *)(((int)c + (off)) & 0xFFFFFFFFFFFFFFFF))

int func_ov019_02111558(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;

    switch (*(unsigned char *)(c + 0x38f)) {
    case 0: {
        int r5 = *(int *)(c + 0x178);
        int d = Vec3_Dist(c + 0x5c, (char *)*(void **)(c + 0x378) + 0x5c);
        if (d < r5 + 0x78000) {
            if (_ZN6Player9StartTalkER9ActorBaseb(*(void **)(c + 0x378), c, 1) != 0) {
                LB(0x38f) = LB(0x38f) + 1;
            }
        }
        break;
    }
    case 1: {
        short ang = Vec3_HorzAngle(c + 0x5c, (char *)*(void **)(c + 0x378) + 0x5c);
        if (_Z14ApproachLinearRsss((short *)(c + 0x8e), ang, 0x514) != 0) {
            unsigned int id;
            Vec3 v;
            int z = *(int *)(c + 0x64);
            int y = *(int *)(c + 0x60) + 0x190000;
            int x = *(int *)(c + 0x5c);
            v.x = x;
            v.y = y;
            v.z = z;
            if (*(unsigned char *)(c + 0x394) != 0) {
                id = 0xa9;
                _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x138, data_ov019_021134a8[1], 0, 0x1000, 0);
                *(unsigned char *)(c + 0x395) = 1;
            } else {
                if (*(unsigned char *)(c + 0x395) != 0)
                    id = 0xaa;
                else
                    id = 0xa8;
            }
            if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(*(void **)(c + 0x378), c, id, &v, 1, 2) != 0) {
                func_0201267c(0xdf, c + 0x74);
                LB(0x38f) = LB(0x38f) + 1;
            }
        }
        break;
    }
    case 2:
        if (_ZN6Player12GetTalkStateEv(*(void **)(c + 0x378)) == 2) {
            if (*(unsigned char *)(c + 0x395) == 0) {
                _ZN5Actor19UntrackAndSpawnStarERajRK7Vector3j(
                    c, (signed char *)(((int)c + 0x396) & 0xFFFFFFFFFFFFFFFF),
                    (unsigned int)(unsigned char)((*(unsigned int *)(c + 8) >> 8) & 0xf),
                    c + 0x5c, 4);
            }
            _ZN6Player18HasFinishedTalkingEv(*(void **)(c + 0x378));
            _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x138, data_ov019_02113468[1], 0, 0x1000, 0);
            LB(0x38f) = LB(0x38f) + 1;
        }
        break;
    }
    return 1;
}
