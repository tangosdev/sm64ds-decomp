//cpp
extern "C" void _ZN5Sound12PlayBank2_2DEj(unsigned int);
struct Pair { int v[2]; };
extern Pair data_ov006_0213c96c;
extern Pair data_ov006_0213c9c4;
extern Pair data_ov006_0213c9bc;

extern "C" void func_ov006_020eb8f0(char *c)
{
    Pair *g = &data_ov006_0213c96c;
    Pair *p = (Pair *)c;
    if (p->v[0] == g->v[0] && (p->v[1] == g->v[1] || p->v[0] == 0)) {
        *(unsigned char *)(c + 0x93) = 0;
        *(unsigned short *)(c + 0x78) = 0x60;
        if (*(int *)(c + 0x70) == 0)
            _ZN5Sound12PlayBank2_2DEj(0x1e9);
        *p = data_ov006_0213c9c4;
    } else {
        *(unsigned char *)(c + 0x95) = 5;
        *(int *)(c + 0x60) = 0;
        *(Pair *)(c + 0x10) = *(Pair *)(c + 8);
        *p = data_ov006_0213c9bc;
    }
}
