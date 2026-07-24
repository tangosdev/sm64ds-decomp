//cpp
extern "C" {
struct E { int w[2]; };
extern E data_ov071_02123038;
extern E data_ov071_02123040;
void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void* ts, void* file, int a, int d, unsigned e);
void _ZN9Animation8SetFlagsEi(void* anim, int flags);
int _ZN9Animation8FinishedEv(void* anim);
void _ZN9Animation7AdvanceEv(void* anim);

int func_ov071_02120860(char* c)
{
    unsigned char* st;
    switch (*(unsigned char*)(c + 0x214)) {
    case 0:
        return 0;
    case 1:
    case 4:
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x138, (void*)data_ov071_02123038.w[1], 0, 0x1000, 0);
        _ZN9Animation8SetFlagsEi(c + 0x138, 0x40000000);
        *(int*)(c + 0x144) = 0x1000;
        *(int*)(c + 0x140) = 0;
        st = (unsigned char*)(((int)c + 0x214) & 0xFFFFFFFFFFFFFFFFLL);
        *st = *st + 1;
        /* fall through */
    case 2:
    case 5:
        if (_ZN9Animation8FinishedEv(c + 0x138) != 0) {
            _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x138, (void*)data_ov071_02123040.w[1], 0, 0x1000, 0);
            _ZN9Animation8SetFlagsEi(c + 0x138, 0x40000000);
            *(int*)(c + 0x144) = 0x1000;
            *(int*)(c + 0x140) = 0;
            st = (unsigned char*)(((int)c + 0x214) & 0xFFFFFFFFFFFFFFFFLL);
            *st = *st + 1;
        }
        _ZN9Animation7AdvanceEv(c + 0x138);
        return 0;
    case 3:
    case 6:
        if (_ZN9Animation8FinishedEv(c + 0x138) != 0) {
            st = (unsigned char*)(((int)c + 0x214) & 0xFFFFFFFFFFFFFFFFLL);
            *st = *st + 1;
        }
        _ZN9Animation7AdvanceEv(c + 0x138);
        return 0;
    case 7:
        *(unsigned char*)(c + 0x214) = 0;
        return 1;
    default:
        return 0;
    }
}
}
