//cpp
extern "C" void *Actor_FindEgg(void *self, void *clsn);
extern "C" void Actor_SpawnCoins(void *self, void *pos, unsigned int a, int fix, short b);
extern "C" void Actor_PoofDust(void *self);
extern "C" void func_02012694(int a, void *pos);
extern "C" void ActorBase_MarkForDestruction(void *self);
extern "C" void *Actor_FindWithID(unsigned int id);
extern "C" void func_ov077_02125e94(void *c, int i);
extern "C" int Player_IsOnShell(void *p);
extern "C" void Sound_PlayBank0(unsigned int id, void *pos);
extern "C" short Vec3_HorzAngle(void *a, void *b);
extern "C" void Player_IncMegaKillCount(void *p);
extern "C" void Player_Hurt(void *self, void *pos, unsigned int a, int fix, unsigned int b, unsigned int cc, unsigned int d);

extern "C" void func_ov077_02124eb0(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    unsigned char *r4;
    int b;

    if (Actor_FindEgg(c, c + 0x1b0) != 0) {
        int v[3];
        v[0] = *(int *)(c + 0x5c);
        v[1] = *(int *)(c + 0x60);
        v[2] = *(int *)(c + 0x64);
        Actor_SpawnCoins(c, v, 1, 0x2000, 0);
        Actor_PoofDust(c);
        func_02012694(0xc4, c + 0x74);
        ActorBase_MarkForDestruction(c);
        return;
    }

    {
        unsigned int id = *(unsigned int *)(c + 0x1d4);
        if (id == 0)
            return;
        r4 = (unsigned char *)Actor_FindWithID(id);
    }
    if (r4 == 0)
        return;

    b = (int)(*(unsigned short *)(r4 + 0xc) == 0xbf);
    if (b == 0)
        return;

    b = (int)((*(int *)(c + 0xb0) & 0x20000) != 0);
    if (b != 0) {
        func_ov077_02125e94(c, 3);
        return;
    }

    if ((*(int *)(c + 0x1d0) & 0x403c0)
        || Player_IsOnShell(r4) != 0
        || *(unsigned char *)(r4 + 0x6f9) != 0) {
        if (*(int *)(c + 0x3d8) != 1)
            return;
        Sound_PlayBank0(0xb5, c + 0x74);
        *(unsigned char **)(c + 0x3d4) = r4;
        func_ov077_02125e94(c, 2);
        return;
    }

    if (*(int *)(c + 0x1d0) & 0x10) {
        *(short *)(c + 0x94) = Vec3_HorzAngle(r4 + 0x5c, c + 0x5c);
        Player_IncMegaKillCount(r4);
        func_ov077_02125e94(c, 5);
        return;
    }

    if (*(int *)(c + 0x3d8) == 4)
        return;

    {
        int v[3];
        v[0] = *(int *)(c + 0x5c);
        v[1] = *(int *)(c + 0x60);
        v[2] = *(int *)(c + 0x64);
        Player_Hurt(r4, v, 2, 0xc000, 1, 0, 1);
    }
}
