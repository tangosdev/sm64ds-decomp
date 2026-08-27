struct Ent {
    char pad44[0x44];
    unsigned char f44;   /* 0x44 */
    char pad45;          /* 0x45 */
    unsigned char f46;   /* 0x46 */
    unsigned char f47;   /* 0x47 */
    char pad48[2];       /* 0x48 */
    unsigned char f4a;   /* 0x4a */
    char pad4b[0x4c - 0x4b];
};

void func_ov002_020f5b98(struct Ent *e)
{
    int i;
    for (i = 0; i < 4; i++) {
        if (e->f44 != 0) {
            e->f46 = 2;
            e->f47 = 0;
            e->f4a = 0;
        }
        e++;
    }
}
