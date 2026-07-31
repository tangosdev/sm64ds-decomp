typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct {
    u32 kind : 4;
    u32 chan : 4;
    u32 flag : 24;
    u32 word4;
    u8 byte8;
} Cmd;

typedef struct {
    Cmd entries[833];   /* 0x000..0x270b */
    int f270c;
    int f2710;
    int f2714;
    int f2718;
    int f271c;
    int f2720;
    int f2724;
    int f2728;
    int f272c;
} Sub;

typedef struct {
    int status;         /* 0x00 */
    int pad04[9];       /* 0x04..0x27 */
    int unk28;          /* 0x28 */
    int unk2c;          /* 0x2c */
    char pad30[0x410];  /* 0x30..0x43f */
    Sub sub;            /* 0x440 */
} Globals;

extern int func_020659a0(int a, int b);
extern int func_020656d4(u16 arg0, const void *arg1, int arg2, u32 arg3);
extern void func_02042160();
extern Globals data_020a1fc0;

void func_02041e14(void)
{
    Sub *s = (Sub *)(((int)&data_020a1fc0 + 0x440));
    Cmd cmd;
    Cmd cmd2;

    if (data_020a1fc0.status == 0) {
        return;
    }
    if (s->f2714 != 0) {
        return;
    }

    *(int *)(((int)s + 0x270c)) &= s->f2718;
    *(int *)(((int)s + 0x2710)) &= s->f2718;
    *(int *)(((int)s + 0x271c)) &= s->f2718;
    *(int *)(((int)s + 0x2728)) &= s->f2718;

    if (s->f2720 != 0 && s->f2724 == 0) {
        int v = s->f272c;
        int a1 = data_020a1fc0.unk2c;
        int snd;

        s->f2720 = 0;
        data_020a1fc0.unk28 = v;
        func_020659a0(v, a1);

        snd = s->f2728;
        if (snd == 0) {
            return;
        }
        cmd.kind = 1;
        cmd.flag = 0;
        cmd.word4 = 0;
        cmd.chan = (u8)(data_020a1fc0.unk28 >> 8);
        cmd.byte8 = (u8)data_020a1fc0.unk28;
        func_020656d4((u16)snd, &cmd, 9, (u32)func_02042160);
        s->f2714 = 1;
        s->f2728 = 0;
    } else {
        int mask;
        Cmd *found;
        int cond;
        int i;
        int pri;
        int bit;
        Cmd *p;
        u8 k;
        int c;
        int w4;

        mask = s->f270c;
        if (mask == 0) {
            return;
        }
        pri = s->f2710;
        found = 0;
        cond = (pri != 0 && pri == (pri & mask));
        if (cond) {
            mask = pri;
        } else {
            mask &= ~pri;
        }

        p = s->entries;
        i = 0;
        for (;;) {
            bit = 1 << i;
            if (bit > mask) {
                break;
            }
            if (bit & mask) {
                if (found == 0) {
                    found = p;
                } else if (found->kind != p->kind || found->word4 != p->word4) {
                    mask &= ~bit;
                }
            }
            p++;
            i++;
        }

        if (cond && mask != pri) {
            s->f2710 = 0;
        }
        if (found == 0) {
            return;
        }

        switch (found->kind) {
        case 0:
            found->kind = 1;
            break;
        case 2:
            found->kind = 3;
            *(int *)(((long long)(int)((char *)s + 0x271c))) &= ~mask;
            break;
        }

        k = found->kind;
        w4 = found->word4;
        c = data_020a1fc0.unk28;
        cmd2.kind = k;
        cmd2.flag = 1;
        cmd2.word4 = w4;
        cmd2.chan = (u8)(c >> 8);
        cmd2.byte8 = (u8)c;
        func_020656d4((u16)mask, &cmd2, 9, (u32)func_02042160);
        s->f2714 = 1;
        *(int *)(((long long)(int)((char *)s + 0x270c))) &= ~mask;
    }
}
