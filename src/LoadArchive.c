typedef unsigned char u8;

struct Entry {
    int field_0;     /* 0x0 */
    int field_4;     /* 0x4 */
    int field_8;     /* 0x8 */
    int field_c;     /* 0xc */
    int field_10;    /* 0x10 */
};

extern struct Entry data_0208ecf4[];
extern int data_020a0ea0;

extern int func_02018934(int a, int b, int c);

int LoadArchive(int idx)
{
    struct Entry *e = &data_0208ecf4[idx];
    if (e->field_0 == 0) {
        e->field_4 = data_020a0ea0;
        e->field_0 = func_02018934(e->field_c, e->field_10, e->field_4);
    }
    return e->field_0 != 0;
}
