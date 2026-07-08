typedef unsigned char u8;

struct Entry {
    int field_0;     /* 0x0 */
    int field_4;     /* 0x4 */
    int field_8;     /* 0x8 */
    int field_c;     /* 0xc */
    int field_10;    /* 0x10 */
};

extern struct Entry ARCHIVE_INFOS[];
extern int _ZN6Memory14defaultHeapPtrE;

extern int func_02018934(int a, int b, int c);

int LoadArchive(int idx)
{
    struct Entry *e = &ARCHIVE_INFOS[idx];
    if (e->field_0 == 0) {
        e->field_4 = _ZN6Memory14defaultHeapPtrE;
        e->field_0 = func_02018934(e->field_c, e->field_10, e->field_4);
    }
    return e->field_0 != 0;
}
