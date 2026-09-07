typedef struct { int x, y, z; } Vec3;

extern int _ZN6Player12GetTalkStateEv(void *player);
extern int _ZN6Player9StartTalkER7fBase_cb(void *player, void *actor, int b);
extern int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(void *player, void *actor, unsigned int msg, const Vec3 *pos, unsigned int a, unsigned int b);
extern short Vec3_HorzAngle(const Vec3 *a, const Vec3 *b);
extern void func_0201267c(unsigned int id, void *p);
extern void _ZN5Sound22StopLoadedMusic_Layer3Ev(void);
extern void func_02011cfc(void);
extern void func_ov079_02123d4c(Vec3 *out, void *actor);
extern void _ZN8dActor_c16TriplePoofDustAtERK7Vector3(void *actor, const Vec3 *p);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int a, int x, int y, int z);
extern void _ZN8dActor_c19UntrackAndSpawnStarERajRK7Vector3h(void *actor, signed char *flag, unsigned int id, const Vec3 *pos, unsigned int j);
extern int _ZN4dBgW9IsEnabledEv(void *self);
extern void _ZN4dBgW7DisableEv(void *self);
extern void _ZN7fBase_c18MarkForDestructionEv(void *self);
extern void _ZN8dActor_c10PoofDustAtERK7Vector3(void *actor, const Vec3 *p);

extern short data_02082214[];

void func_ov079_021246dc(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    Vec3 pos;
    Vec3 starPos;
    Vec3 v;
    Vec3 v2;
    Vec3 dp;
    Vec3 dp2;

    if (*(unsigned char *)(c + 0x414) != 0) {
        switch (*(unsigned char *)(c + 0x40c)) {
        case 0:
            if (_ZN6Player9StartTalkER7fBase_cb(*(void **)(c + 0x3ac), c, 1) == 0)
                return;
            (*(unsigned char *)(((int)c + 0x40c)))++;
            return;
        case 1:
            if (_ZN6Player12GetTalkStateEv(*(void **)(c + 0x3ac)) != 0)
                return;
            {
                int idx = *(int *)(c + 0x3ec);
                unsigned char *other = *(unsigned char **)(c + idx * 4 + 0x39c);
                Vec3 *op = (Vec3 *)((unsigned char *)(((int)other + 0x5c)));
                unsigned short ang;
                int i;
                short mag = 0x10e;
                pos.x = op->x;
                pos.y = op->y;
                pos.z = op->z;
                ang = Vec3_HorzAngle(&pos, (Vec3 *)(c + 0x5c));
                i = ang >> 4;
                pos.y += 0x32000;
                pos.x = data_02082214[i * 2] * mag + pos.x;
                pos.z = data_02082214[i * 2 + 1] * mag + pos.z;
                if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(*(void **)(c + 0x3ac), c, 0xa5, &pos, 0, 0) == 0)
                    return;
                (*(unsigned char *)(((int)c + 0x40c)))++;
                func_0201267c(0x133, c + 0x74);
            }
            return;
        case 2:
            if (_ZN6Player12GetTalkStateEv(*(void **)(c + 0x3ac)) != -1)
                return;
            (*(unsigned char *)(((int)c + 0x40c)))++;
            return;
        case 3:
            *(int *)(c + 0x3b0) = 9;
            starPos.x = *(int *)(c + 0x5c);
            starPos.y = *(int *)(c + 0x60);
            starPos.z = *(int *)(c + 0x64);
            starPos.y += 0x64000;
            func_0201267c(0xc6, c + 0x74);
            _ZN5Sound22StopLoadedMusic_Layer3Ev();
            func_02011cfc();
            func_ov079_02123d4c(&v, c);
            v.y += 0x50000;
            dp.x = v.x;
            dp.y = v.y;
            dp.z = v.z;
            _ZN8dActor_c16TriplePoofDustAtERK7Vector3(c, &dp);
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(4, v.x, v.y, v.z);
            *(unsigned char *)(c + 0x404) = 0;
            _ZN8dActor_c19UntrackAndSpawnStarERajRK7Vector3h(c, (signed char *)(c + 0x408), *(unsigned char *)(c + 0x409), &starPos, 4);
            if (_ZN4dBgW9IsEnabledEv(c + 0x418) != 0)
                _ZN4dBgW7DisableEv(c + 0x418);
            _ZN7fBase_c18MarkForDestructionEv(c);
            return;
        }
    } else {
        func_0201267c(0xc6, c + 0x74);
        func_ov079_02123d4c(&v2, c);
        v2.y += 0x28000;
        dp2.x = v2.x;
        dp2.y = v2.y;
        dp2.z = v2.z;
        _ZN8dActor_c10PoofDustAtERK7Vector3(c, &dp2);
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(5, v2.x, v2.y, v2.z);
        _ZN7fBase_c18MarkForDestructionEv(c);
    }
}
