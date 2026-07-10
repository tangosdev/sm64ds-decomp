//cpp
struct Actor { int DistToCPlayer(); };
namespace cstd { int fdiv(int a, int b); }
struct ModelBase { void ApplyOpacity(unsigned int o, int x); };

extern "C" int _ZN13PeachPainting8BehaviorEv(char *c)
{
    int d = ((Actor *)c)->DistToCPlayer();
    if (d >= 0xe10000) {
        *(unsigned char *)(c + 0x124) = 0xff;
    } else if (d <= 0xbf4000) {
        *(unsigned char *)(c + 0x124) = 0;
    } else {
        int q = cstd::fdiv(d - 0xbf4000, 0x21c000);
        int o = (int)(((long long)q * 0xff + 0x800) >> 12);
        *(unsigned char *)(c + 0x124) = (unsigned char)(o >> 3);
    }
    ((ModelBase *)(c + 0xd4))->ApplyOpacity(*(unsigned char *)(c + 0x124), 1);
    return 1;
}
