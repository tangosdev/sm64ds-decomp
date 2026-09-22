typedef struct Entry {
    int id;
    int ptr;
} Entry;

extern int GetGameLanguage(void);
extern int LoadFile(int handle);

extern int data_ov004_020b9f54;
extern int data_ov004_020bc42c[];
extern int data_ov004_020bc65c[];
extern int data_ov004_020bc418[];
extern int data_ov004_020bc648[];
extern int data_ov004_020bc3c8[];
extern int data_ov004_020bc5e4[];
extern int data_ov004_020bc4f4[];
extern int data_ov004_020bc300[];
extern int data_ov004_020bc3dc[];
extern int data_ov004_020bc5f8[];
extern int data_ov004_020bc3f0[];
extern int data_ov004_020bc60c[];
extern int data_ov004_020bc404[];
extern int data_ov004_020bc634[];
extern int data_ov004_020bc3b4[];
extern int data_ov004_020bc5bc[];
extern int data_ov004_020bc468[];
extern int data_ov004_020bc6ac[];
extern int data_ov004_020bc454[];
extern int data_ov004_020bc698[];
extern int data_ov004_020bc378[];
extern int data_ov004_020bc594[];
extern int data_ov004_020bc620[];
extern int data_ov004_020bc670[];
extern int data_ov004_020bc440[];
extern int data_ov004_020bc684[];
extern int data_ov004_020bc5d0[];
extern int data_ov004_020bc3a0[];
extern int data_ov004_020bc47c[];
extern int data_ov004_020bc288[];
extern int data_ov004_020bc490[];
extern int data_ov004_020bc29c[];
extern int data_ov004_020bc580[];
extern int data_ov004_020bc350[];
extern int data_ov004_020bc4a4[];
extern int data_ov004_020bc2b0[];
extern int data_ov004_020bc4b8[];
extern int data_ov004_020bc2c4[];
extern int data_ov004_020bc4cc[];
extern int data_ov004_020bc2d8[];
extern int data_ov004_020bc4e0[];
extern int data_ov004_020bc2ec[];
extern int data_ov004_020bc38c[];
extern int data_ov004_020bc508[];
extern int data_ov004_020bc314[];
extern int data_ov004_020bc51c[];
extern int data_ov004_020bc328[];
extern int data_ov004_020bc530[];
extern int data_ov004_020bc6d4[];
extern int data_ov004_020bc544[];
extern int data_ov004_020bc6c0[];
extern int data_ov004_020bc5a8[];
extern int data_ov004_020bc33c[];
extern int data_ov004_020bc558[];
extern int data_ov004_020bc364[];
extern int data_ov004_020bc56c[];

extern int data_ov004_020bf560[];
extern int data_ov004_020bf5d4[];

#pragma opt_strength_reduction off
void func_ov004_020b2cb8(void)
{
    int i;
    /* The ROM's template for this array is at 0x020bc6e8 and is all zero except
       entry 1. What stood here was 29 invented pairs {1,100}, {2,101} ... {29,128};
       none of them were ever compared to anything, because mwccarm puts a local
       initializer in an anonymous .rodata object (`@7`) that objisolate drops and
       match.py wildcards the relocation to.

       It matters for exactly one entry. Every index except 1 is overwritten below
       before the loop reads it -- entry 1 is not, so its initial value is live, and
       it was wrong. */
    Entry entries[29] = {
        { 0, 0 },
        { 0x140, (int)&data_ov004_020b9f54 },
    };

    entries[0].id = data_ov004_020bc42c[GetGameLanguage()];
    entries[0].ptr = data_ov004_020bc65c[GetGameLanguage()];
    entries[2].id = data_ov004_020bc418[GetGameLanguage()];
    entries[2].ptr = data_ov004_020bc648[GetGameLanguage()];
    entries[3].id = data_ov004_020bc3c8[GetGameLanguage()];
    entries[3].ptr = data_ov004_020bc5e4[GetGameLanguage()];
    entries[4].id = data_ov004_020bc4f4[GetGameLanguage()];
    entries[4].ptr = data_ov004_020bc300[GetGameLanguage()];
    entries[5].id = data_ov004_020bc3dc[GetGameLanguage()];
    entries[5].ptr = data_ov004_020bc5f8[GetGameLanguage()];
    entries[6].id = data_ov004_020bc3f0[GetGameLanguage()];
    entries[6].ptr = data_ov004_020bc60c[GetGameLanguage()];
    entries[7].id = data_ov004_020bc404[GetGameLanguage()];
    entries[7].ptr = data_ov004_020bc634[GetGameLanguage()];
    entries[8].id = data_ov004_020bc3b4[GetGameLanguage()];
    entries[8].ptr = data_ov004_020bc5bc[GetGameLanguage()];
    entries[9].id = data_ov004_020bc468[GetGameLanguage()];
    entries[9].ptr = data_ov004_020bc6ac[GetGameLanguage()];
    entries[10].id = data_ov004_020bc454[GetGameLanguage()];
    entries[10].ptr = data_ov004_020bc698[GetGameLanguage()];
    entries[11].id = data_ov004_020bc378[GetGameLanguage()];
    entries[11].ptr = data_ov004_020bc594[GetGameLanguage()];
    entries[12].id = data_ov004_020bc620[GetGameLanguage()];
    entries[12].ptr = data_ov004_020bc670[GetGameLanguage()];
    entries[13].id = data_ov004_020bc440[GetGameLanguage()];
    entries[13].ptr = data_ov004_020bc684[GetGameLanguage()];
    entries[14].id = data_ov004_020bc5d0[GetGameLanguage()];
    entries[14].ptr = data_ov004_020bc3a0[GetGameLanguage()];
    entries[15].id = data_ov004_020bc47c[GetGameLanguage()];
    entries[15].ptr = data_ov004_020bc288[GetGameLanguage()];
    entries[16].id = data_ov004_020bc490[GetGameLanguage()];
    entries[16].ptr = data_ov004_020bc29c[GetGameLanguage()];
    entries[17].id = data_ov004_020bc580[GetGameLanguage()];
    entries[17].ptr = data_ov004_020bc350[GetGameLanguage()];
    entries[18].id = data_ov004_020bc4a4[GetGameLanguage()];
    entries[18].ptr = data_ov004_020bc2b0[GetGameLanguage()];
    entries[19].id = data_ov004_020bc4b8[GetGameLanguage()];
    entries[19].ptr = data_ov004_020bc2c4[GetGameLanguage()];
    entries[20].id = data_ov004_020bc4cc[GetGameLanguage()];
    entries[20].ptr = data_ov004_020bc2d8[GetGameLanguage()];
    entries[21].id = data_ov004_020bc4e0[GetGameLanguage()];
    entries[21].ptr = data_ov004_020bc2ec[GetGameLanguage()];
    entries[22].id = data_ov004_020bc38c[GetGameLanguage()];
    entries[22].ptr = data_ov004_020bc508[GetGameLanguage()];
    entries[23].id = data_ov004_020bc314[GetGameLanguage()];
    entries[23].ptr = data_ov004_020bc51c[GetGameLanguage()];
    entries[24].id = data_ov004_020bc328[GetGameLanguage()];
    entries[24].ptr = data_ov004_020bc530[GetGameLanguage()];
    entries[25].id = data_ov004_020bc6d4[GetGameLanguage()];
    entries[25].ptr = data_ov004_020bc544[GetGameLanguage()];
    entries[26].id = data_ov004_020bc6c0[GetGameLanguage()];
    entries[26].ptr = data_ov004_020bc5a8[GetGameLanguage()];
    entries[27].id = data_ov004_020bc33c[GetGameLanguage()];
    entries[27].ptr = data_ov004_020bc558[GetGameLanguage()];
    entries[28].id = data_ov004_020bc364[GetGameLanguage()];
    entries[28].ptr = data_ov004_020bc56c[GetGameLanguage()];

    for (i = 0; i < 29; i++) {
        int t;
        t = LoadFile(entries[i].id);
        ((volatile int *)data_ov004_020bf560)[i] = t;
        t = (int)&entries[i];
        t = *(volatile int *)(t + 4);
        t = *(int *)t;
        data_ov004_020bf5d4[i] = t;
    }
}
