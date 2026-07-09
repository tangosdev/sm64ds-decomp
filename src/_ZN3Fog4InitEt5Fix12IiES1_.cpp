//cpp
extern "C" void _ZN3Fog4InitEt5Fix12IiES1_(char* self, unsigned short color, int nearv, int farv) {
    int dist;
    int step;
    int i;
    char* p;

    *(unsigned char*)(self + 0x20) = 1;
    dist = 0x28000 - (nearv << 4);
    *(unsigned short*)(self + 0x24) = color;
    *(unsigned char*)(self + 0x21) = 6;
    *(unsigned short*)(self + 0x22) = 0;
    if (dist >= 0x80000)
        dist = 0x7ffff;
    step = (0x100000 - (farv << 4) - dist) >> 5;
    i = 0;
    p = self;
    while (i < 0x20) {
        if (dist <= 0) {
            *p = 0;
        } else if (dist > 0x80000) {
            *p = 0x7f;
        } else {
            *p = (unsigned char)(dist >> 12);
        }
        i++;
        p++;
        dist += step;
    }
}