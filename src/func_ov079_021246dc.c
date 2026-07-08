typedef struct { int x, y, z; } Vec3;

extern int Player_GetTalkState(void *player);
extern int Player_StartTalk(void *player, void *actor, int b);
extern int Player_ShowMessage(void *player, void *actor, unsigned int msg, const Vec3 *pos, unsigned int a, unsigned int b);
extern short Vec3_HorzAngle(const Vec3 *a, const Vec3 *b);
extern void func_0201267c(unsigned int id, void *p);
extern void Sound_StopLoadedMusic_Layer3(void);
extern void func_02011cfc(void);
extern void func_ov079_02123d4c(Vec3 *out, void *actor);
extern void Actor_TriplePoofDustAt(void *actor, const Vec3 *p);
extern void Particle_System_NewSimple(unsigned int a, int x, int y, int z);
extern void Actor_UntrackAndSpawnStar(void *actor, signed char *flag, unsigned int id, const Vec3 *pos, unsigned int j);
extern int MeshColliderBase_IsEnabled(void *self);
extern void MeshColliderBase_Disable(void *self);
extern void ActorBase_MarkForDestruction(void *self);
extern void Actor_PoofDustAt(void *actor, const Vec3 *p);

extern short SINE_TABLE[];

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
            if (Player_StartTalk(*(void **)(c + 0x3ac), c, 1) == 0)
                return;
            (*(unsigned char *)(((int)c + 0x40c) & 0xFFFFFFFFFFFFFFFF))++;
            return;
        case 1:
            if (Player_GetTalkState(*(void **)(c + 0x3ac)) != 0)
                return;
            {
                int idx = *(int *)(c + 0x3ec);
                unsigned char *other = *(unsigned char **)(c + idx * 4 + 0x39c);
                Vec3 *op = (Vec3 *)((unsigned char *)(((int)other + 0x5c) & 0xFFFFFFFFFFFFFFFF));
                unsigned short ang;
                int i;
                short mag = 0x10e;
                pos.x = op->x;
                pos.y = op->y;
                pos.z = op->z;
                ang = Vec3_HorzAngle(&pos, (Vec3 *)(c + 0x5c));
                i = ang >> 4;
                pos.y += 0x32000;
                pos.x = SINE_TABLE[i * 2] * mag + pos.x;
                pos.z = SINE_TABLE[i * 2 + 1] * mag + pos.z;
                if (Player_ShowMessage(*(void **)(c + 0x3ac), c, 0xa5, &pos, 0, 0) == 0)
                    return;
                (*(unsigned char *)(((int)c + 0x40c) & 0xFFFFFFFFFFFFFFFF))++;
                func_0201267c(0x133, c + 0x74);
            }
            return;
        case 2:
            if (Player_GetTalkState(*(void **)(c + 0x3ac)) != -1)
                return;
            (*(unsigned char *)(((int)c + 0x40c) & 0xFFFFFFFFFFFFFFFF))++;
            return;
        case 3:
            *(int *)(c + 0x3b0) = 9;
            starPos.x = *(int *)(c + 0x5c);
            starPos.y = *(int *)(c + 0x60);
            starPos.z = *(int *)(c + 0x64);
            starPos.y += 0x64000;
            func_0201267c(0xc6, c + 0x74);
            Sound_StopLoadedMusic_Layer3();
            func_02011cfc();
            func_ov079_02123d4c(&v, c);
            v.y += 0x50000;
            dp.x = v.x;
            dp.y = v.y;
            dp.z = v.z;
            Actor_TriplePoofDustAt(c, &dp);
            Particle_System_NewSimple(4, v.x, v.y, v.z);
            *(unsigned char *)(c + 0x404) = 0;
            Actor_UntrackAndSpawnStar(c, (signed char *)(c + 0x408), *(unsigned char *)(c + 0x409), &starPos, 4);
            if (MeshColliderBase_IsEnabled(c + 0x418) != 0)
                MeshColliderBase_Disable(c + 0x418);
            ActorBase_MarkForDestruction(c);
            return;
        }
    } else {
        func_0201267c(0xc6, c + 0x74);
        func_ov079_02123d4c(&v2, c);
        v2.y += 0x28000;
        dp2.x = v2.x;
        dp2.y = v2.y;
        dp2.z = v2.z;
        Actor_PoofDustAt(c, &dp2);
        Particle_System_NewSimple(5, v2.x, v2.y, v2.z);
        ActorBase_MarkForDestruction(c);
    }
}
