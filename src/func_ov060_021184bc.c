extern void ClearSpikeBomb(int idx);
extern char *_ZN5Actor15FindWithActorIDEjPS_(unsigned int id, char *p);

void func_ov060_021184bc(char *c)
{
    int i;
    int j;
    unsigned int id;
    char *a;

    ClearSpikeBomb(*(int *)(c + 0x1a8));
    *(int *)(((int)c + 0x13c) & 0xFFFFFFFFFFFFFFFF) |= 1;
    *(int *)(c + 0x170) = 3;
    a = 0;
    for (i = 0; i < 8; i++)
        ((int *)(c + 0x188))[i] = 0;
    j = 0;
    id = 0x11c;
    while (1) {
        a = _ZN5Actor15FindWithActorIDEjPS_(id, a);
        if (a == 0)
            break;
        if (a != c) {
            ((int *)(c + 0x188))[j] = *(int *)(a + 4);
            j++;
            if (j == 8)
                break;
        }
    }
}
