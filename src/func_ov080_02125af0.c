/* walk entry pointer: p starts at base, p += 1 each (struct size 0x18) */
typedef struct { int a,b,c,pad,e,pad2; } Ent;
void func_ov080_02125af0(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    int i;
    int cnt = *(unsigned short *)((c + 0x100) + 0xb8);
    i = 0;
    if (cnt <= 0) return;
    do {
        Ent *e = *(Ent **)(c + 0x1a0) + i;
        int v = e->a;
        if (v != 0) {
            unsigned int t = *(unsigned int *)(c + 8);
            int a = ((unsigned char)(t & 0xf) + 1) * 0x64000;
            if (v != a) {
                int v2 = e->b;
                if (v2 != 0) {
                    int b = ((unsigned char)((t >> 4) & 0xf) + 1) * 0x64000;
                    if (v2 != b) goto next;
                }
            }
        }
        e->c = 0;
        {
            Ent *e2 = *(Ent **)(c + 0x1a0) + i;
            e2->e = 0x1ff00000;
        }
    next:
        i++;
    } while (i < *(unsigned short *)((c + 0x100) + 0xb8));
}
