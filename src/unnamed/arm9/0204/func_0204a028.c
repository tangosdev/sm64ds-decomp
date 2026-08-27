extern void _ZN2GX16BeginLoadTexPlttEv(void);
extern void _ZN2GX11LoadTexPlttEPKvjj(void const *p, unsigned int a, unsigned int b);
extern void _ZN2GX14EndLoadTexPlttEv(void);

typedef struct {
    int field_0;     /* 0x0  pointer to Tex */
    int field_4;
    int field_8;
    int field_c;
    int field_10;
} TexEntry;

typedef struct {
    int field_0;
    int field_4;     /* low 4 bits select */
    int field_8;
    int field_c;
    int field_10;
} Tex;

typedef struct {
    char pad[0x20];
    TexEntry *list;  /* field_20 */
    char pad2[2];
    unsigned short count;  /* field_26 */
} Holder;

int func_0204a028(Holder *thiz, unsigned int (*cb)(unsigned int, unsigned int))
{
    int i;

    _ZN2GX16BeginLoadTexPlttEv();

    for (i = 0; i < thiz->count; i++) {
        TexEntry *entry = (TexEntry *)((char *)thiz->list + i * 0x14);
        Tex *p = (Tex *)entry->field_0;
        unsigned int r6 = 0;

        if (p->field_10 != 0) {
            unsigned int arg = ((unsigned)(p->field_4 << 0x1c) >> 0x1c) == 2 ? 1 : 0;
            r6 = cb(p->field_10, arg);
            _ZN2GX11LoadTexPlttEPKvjj((void *)((char *)entry->field_0 + p->field_c), r6, p->field_10);
        }
        entry->field_8 = r6;
    }

    _ZN2GX14EndLoadTexPlttEv();
    return 1;
}
