extern void (*data_ov002_0210cbb8[])(void*, int, unsigned int);
extern unsigned char STAR_ID;

void _Z11LoadObjectsRN11LVL_Overlay8ObjTableEij(char* t, int a1, unsigned int a2)
{
    unsigned char* e = *(unsigned char**)(t + 4);
    int i;
    for (i = 0; i < *(unsigned short*)t; i++) {
        unsigned char b = *e;
        int type = (b >> 5) & 7;
        if (type == 0 || type == STAR_ID) {
            void (*h)(void*, int, unsigned int) = data_ov002_0210cbb8[b & 0x1f];
            if (h != 0) h(e, a1, a2);
        }
        e += 8;
    }
}
