//cpp
extern "C" int _ZN10BigBooIcon13InitResourcesEv(void *c);

struct dActor_c {
    void *vtable;
    int field4;
    int field8;
    char padd4[0xd4 - 0xc];
    signed char field_d4;
    unsigned char field_d5;
    unsigned int TrackStar(unsigned int idx, unsigned int count);
};

extern "C" int _ZN10BigBooIcon13InitResourcesEv(void *c)
{
    dActor_c *a = (dActor_c *)c;
    unsigned int val = (unsigned int)a->field8;
    a->field_d5 = (unsigned char)((val >> 8) & 0xf);
    a->field_d4 = ~0;
    unsigned int idx = a->field_d5;
    unsigned char r0 = (unsigned char)a->TrackStar(idx, 2);
    a->field_d4 = (signed char)r0;
    return 1;
}
