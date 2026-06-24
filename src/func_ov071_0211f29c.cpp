//cpp
extern "C" void *Actor_FindEgg(void *self, void *clsn);
extern "C" void Sound_PlayBank0(unsigned int id, void *pos);
extern "C" void func_ov071_0211f498(void *c);
extern "C" void *Actor_FindWithID(unsigned int id);
extern "C" int Player_IsOnShell(void *p);
extern "C" short Vec3_HorzAngle(void *a, void *b);
extern "C" void Player_IncMegaKillCount(void *p);
extern "C" int Actor_JumpedOnByPlayer(void *self, void *clsn, void *player);
extern "C" void Player_Bounce(void *p, int fix);
extern "C" int Player_Hurt(void *self, void *pos, unsigned int a, int fix, unsigned int b, unsigned int cc, unsigned int d);
extern "C" int func_ov071_021202ec(void *c, int i);

extern "C" void func_ov071_0211f29c(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    unsigned char *r4;
    int b;

    if (Actor_FindEgg(c, c + 0x160) != 0) {
        Sound_PlayBank0(9, c + 0x74);
        func_ov071_0211f498(c);
        return;
    }

    {
        unsigned int id = *(unsigned int *)(c + 0x184);
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
        func_ov071_021202ec(c, 6);
        return;
    }

    if ((*(int *)(c + 0x180) & 0x66fe0)
        || Player_IsOnShell(r4) != 0
        || *(unsigned char *)(r4 + 0x6f9) != 0) {
        Sound_PlayBank0(9, c + 0x74);
        func_ov071_0211f498(c);
        return;
    }

    if (*(int *)(c + 0x180) & 0x10) {
        *(short *)(c + 0x94) = Vec3_HorzAngle(r4 + 0x5c, c + 0x5c);
        *(short *)(c + 0x8e) = (short)(*(short *)(c + 0x94) + 0x8000);
        Player_IncMegaKillCount(r4);
        func_ov071_021202ec(c, 8);
        return;
    }

    if (Actor_JumpedOnByPlayer(c, c + 0x160, r4) != 0) {
        Player_Bounce(r4, 0x28000);
        func_ov071_0211f498(c);
        return;
    }

    if (*(int *)(c + 0x39c) == 7)
        return;

    {
        int v[3];
        v[0] = *(int *)(c + 0x5c);
        v[1] = *(int *)(c + 0x60);
        v[2] = *(int *)(c + 0x64);
        if (Player_Hurt(r4, v, 1, 0xc000, 1, 0, 1) != 0)
            func_ov071_021202ec(c, 4);
    }
}
