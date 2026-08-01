extern int _ZN9Animation8FinishedEv(char* a);
extern void _ZN9ActorBase18MarkForDestructionEv(char* c);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    char* t, void* f, int a, int fix, unsigned int j);
extern int* data_ov002_020ff0c4[];

int func_ov002_020b7200(char* c)
{
    short* sp;
    int* ip;

    if (*(int*)(c + 0x3f0) == 0xa || *(int*)(c + 0x3f0) == 0xf) {
        if (*(int*)(c + 0x3c0) != 0) {
            sp = (short*)((long long)(int)(
                (char*)*(int*)(c + 0x3c0) + 0x8c));
            *(short*)(c + 0x92) = sp[0];
            *(short*)(c + 0x94) = sp[1];
            *(short*)(c + 0x96) = sp[2];
            *(short*)(c + 0x8c) = *(short*)(c + 0x92);
            *(short*)(c + 0x8e) = *(short*)(c + 0x94);
            *(short*)(c + 0x90) = *(short*)(c + 0x96);

            ip = (int*)((long long)(int)(
                (char*)*(int*)(c + 0x3c0) + 0x5c));
            *(int*)(c + 0x5c) = ip[0];
            *(int*)(c + 0x60) = ip[1];
            *(int*)(c + 0x64) = ip[2];
        }
    }

    if (_ZN9Animation8FinishedEv(c + 0x350) != 0) {
        switch (*(int*)(c + 0x3f0)) {
        case 0xa:
        case 0xf:
        case 0x16:
            _ZN9ActorBase18MarkForDestructionEv(c);
            break;

        case 0x15:
            break;

        case 0x14:
            if (*(unsigned char*)(c + 0x3fe) == 0) {
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
                    c + 0x300,
                    (void*)data_ov002_020ff0c4[*(int*)(c + 0x3f4)][1],
                    0, 0x1000, 0);
                *(unsigned char*)(c + 0x3fe) = 1;
            }
            break;
        }
    }

    return 1;
}
