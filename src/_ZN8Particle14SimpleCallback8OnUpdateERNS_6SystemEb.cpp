//cpp
extern "C" int _ZN8Particle14SimpleCallback8OnUpdateERNS_6SystemEb(char *self, char *sys)
{
    *(short *)(self + 4) = *(short *)(sys + 0x3a);
    return 1;
}
