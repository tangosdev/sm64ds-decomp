extern int _ZNK6Player14GetBodyModelIDEjb(char *c, unsigned int a, int b);
extern int _ZNK9Animation12WillHitFrameEi(char *anim, int frame);
extern void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(int x, int y, int z);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, void *v);
extern void func_ov002_020c37a4(char *self, char *arg);
extern int _ZN6Player12FinishedAnimEv(char *c);
extern void func_ov002_020c3cf0(char *self);
extern void Player_AdvanceAnims(char *self);

int func_ov002_020c3a48(char *self)
{
    unsigned int r5;
    char *anim;
    int id;

    if (*(unsigned char *)(self + 0x6e5) == 0) {
        *(int *)(self + 0x688) = 0;
        id = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        anim = *(char **)(self + (id << 2) + 0xdc) + 0x50;
        if (_ZNK9Animation12WillHitFrameEi(anim, 0x5a)) {
            _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(*(int *)(self + 0x5c), *(int *)(self + 0x60), *(int *)(self + 0x64));
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char *)(self + 0x6d9), 0, self + 0x74);
            *(unsigned char *)(((long long)(int)(self + 0x6e5))) += 1;
        }
    } else {
        int buf[6];
        buf[0] = 0;
        buf[1] = 0;
        buf[2] = 0;
        buf[3] = 0;
        buf[4] = 0;
        buf[5] = 0;
        func_ov002_020c37a4(self, (char *)(buf + 3));
        id = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        anim = (char *)(((long long)(int)(*(char **)(self + (id << 2) + 0xdc) + 0x50)));
        r5 = (unsigned int)(*(int *)(anim + 8) << 4) >> 0x10;
        if (r5 == 0x6d || r5 == 0x8d) {
            _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(*(int *)(self + 0x5c), *(int *)(self + 0x60), *(int *)(self + 0x64));
        }
        switch (r5) {
        case 0x77:
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char *)(self + 0x6d9), 2, self + 0x74);
            break;
        case 0x97:
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char *)(self + 0x6d9), 4, self + 0x74);
            break;
        }
        *(int *)(((long long)(int)(self + 0x688))) -= 0x21000;
        if (_ZN6Player12FinishedAnimEv(self)) {
            func_ov002_020c3cf0(self);
            return 1;
        }
    }

    *(int *)(self + 0x5c) = 0;
    *(int *)(self + 0x64) = *(int *)(self + 0x688);
    Player_AdvanceAnims(self);
    return 0;
}
