typedef int Fix12;
typedef unsigned int u32;
typedef unsigned char u8;

struct Entry { char pad[4]; void *file; };
extern struct Entry *data_ov062_0211cee8[];
extern int data_ov062_0211cf0c[];

extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char *anim, void *file, int idx, Fix12 speed, u32 flags);

void func_ov062_02117994(char *c, int idx) {
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        c + 0x300,
        data_ov062_0211cee8[idx]->file,
        data_ov062_0211cf0c[idx],
        *(Fix12*)(c + 0x3bc),
        0
    );
    *(u8*)(c + 0x398) = (u8)idx;
}
