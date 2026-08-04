//cpp
struct Actor;
typedef void (Actor::*PMF)(int);
struct Entry { PMF pmf; };
extern "C" Entry data_ov006_02141840[];
extern "C" void _ZN5Sound12PlayBank2_2DEj(unsigned int);
extern "C" void func_ov006_020dde28(void *thiz, int i);

extern "C" void func_ov006_020ddd6c(char *thiz)
{
    Actor *self = (Actor*)thiz;
    int n = 0;
    int i = 0;
    char *p = thiz;
    for (; i < 0x28; i++) {
        if (*(unsigned char*)(p + 0x4000 + 0x677) != 0) {
            (self->*data_ov006_02141840[*(unsigned char*)(p + 0x4000 + 0x675)].pmf)(i);
            if (*(unsigned char*)(p + 0x4000 + 0x675) != 4)
                n++;
            func_ov006_020dde28(thiz, i);
        }
        p += 0x1c;
    }
    if (n != 0)
        return;
    *(int*)(thiz + 0x5000 + 0x1c8) = 2;
    *(unsigned char*)(thiz + 0x4000 + 0xd13) = 1;
    _ZN5Sound12PlayBank2_2DEj(0x151);
}
