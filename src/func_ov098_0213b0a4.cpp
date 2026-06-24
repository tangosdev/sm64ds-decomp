//cpp
extern "C" {
void _Z14ApproachLinearRsss(short&, short, short);
struct Vector3;
namespace Sound { void PlayBank3(unsigned int, const Vector3&); }

void func_ov098_0213b0a4(char *c)
{
    short r5 = *(short*)(c + 0x94);
    _Z14ApproachLinearRsss(*(short*)(c + 0x92), *(short*)(c + 0x100 + 0x78), 0x100);
    _Z14ApproachLinearRsss(*(short*)(c + 0x94), *(short*)(c + 0x100 + 0x7a), 0x100);
    *(short*)(c + 0x8c) = *(short*)(c + 0x92);
    *(short*)(c + 0x8e) = *(short*)(c + 0x94);
    {
        short t = *(short*)(c + 0x100 + 0x7a);
        if (r5 != t && *(short*)(c + 0x94) == t)
            Sound::PlayBank3(0x14d, *(const Vector3*)(c + 0x74));
    }
    if (*(short*)(c + 0x92) != *(short*)(c + 0x100 + 0x78)) return;
    if (*(short*)(c + 0x94) == *(short*)(c + 0x100 + 0x7a)) {
        *(int*)(c + 0x180) = 1;
        *(int*)(c + 0x174) = 0x78;
    }
}
}
