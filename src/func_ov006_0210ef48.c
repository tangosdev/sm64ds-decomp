typedef unsigned char u8;
typedef unsigned int u32;

typedef struct {
    int f0;          /* 0x00 */
    void* f4;        /* 0x04 */
    int f8;          /* 0x08 */
    int fc;          /* 0x0c */
    u8 pad10[0x21];  /* 0x10 */
    u8 cells[9];     /* 0x31 */
    u8 pad3a[0x32];  /* 0x3a */
    int lines[8];    /* 0x6c */
    u8 marks[8];     /* 0x8c */
    int f94;         /* 0x94 */
    int f98;         /* 0x98 */
} S;

extern int data_ov006_02142c1c[];
extern int data_ov006_0212edfc;

extern int func_ov006_02115598(void* c, int* src, int v2, int v3, int v5);
extern void func_ov006_02115060(void* p);
extern void func_ov006_0210eca4(S* c, int i, int* p);
extern void _ZN5Sound12PlayBank2_2DEj(u32 b);
extern int func_ov006_02115040(void* base, int i);
extern void func_ov006_02115024(void* p);
extern void func_ov006_02114dfc(void* c);

void func_ov006_0210ef48(void* arg, int i)
{
    int count;
    int bingo;
    int m;
    int tmp;
    int slot;
    S* c;
    int v;
    int a[2];
    int b[2];
    int d[2];
    int e[2];

    c = (S*)arg;
    slot = data_ov006_02142c1c[i];
    if (c->cells[slot] == 1)
        return;
    c->cells[slot] = 1;
    v = i * 0x18000 + 0x10000;
    a[0] = v;
    a[1] = 0x78000;
    func_ov006_02115598(c->f4, a, 0x64, 0, 1);
    func_ov006_02115060(c->f4);
    b[0] = v;
    b[1] = 0x78000;
    func_ov006_0210eca4(c, i, b);
    _ZN5Sound12PlayBank2_2DEj(0x173);
    count = 0;
    if (slot < 3) {
        if (c->cells[slot + 3] == 1 && c->cells[slot + 6] == 1) {
            c->lines[slot] = 0x21d;
            c->f94 = 0xb5;
            c->marks[slot] = 1;
            count += 1;
            (*(int*)(((int)c + 0x98) & 0xFFFFFFFFFFFFFFFFLL))++;
        }
    } else if (slot < 6) {
        if (c->cells[slot - 3] == 1 && c->cells[slot + 3] == 1) {
            c->lines[slot - 3] = 0x21d;
            c->f94 = 0xb5;
            c->marks[slot - 3] = 1;
            count += 1;
            (*(int*)(((int)c + 0x98) & 0xFFFFFFFFFFFFFFFFLL))++;
        }
    } else {
        if (c->cells[slot - 6] == 1 && c->cells[slot - 3] == 1) {
            c->lines[slot - 6] = 0x21d;
            c->f94 = 0xb5;
            c->marks[slot - 6] = 1;
            count += 1;
            (*(int*)(((int)c + 0x98) & 0xFFFFFFFFFFFFFFFFLL))++;
        }
    }
    i = slot % 3;
    if (i == 0) {
        if (c->cells[slot + 1] == 1 && c->cells[slot + 2] == 1) {
            c->lines[slot / 3 + 3] = 0x21d;
            c->f94 = 0xb5;
            c->marks[slot / 3 + 3] = 1;
            count += 1;
            (*(int*)(((int)c + 0x98) & 0xFFFFFFFFFFFFFFFFLL))++;
        }
    } else if (i == 1) {
        if (c->cells[slot - 1] == 1 && c->cells[slot + 1] == 1) {
            c->lines[slot / 3 + 3] = 0x21d;
            c->f94 = 0xb5;
            c->marks[slot / 3 + 3] = 1;
            count += 1;
            (*(int*)(((int)c + 0x98) & 0xFFFFFFFFFFFFFFFFLL))++;
        }
    } else {
        if (c->cells[slot - 2] == 1 && c->cells[slot - 1] == 1) {
            c->lines[slot / 3 + 3] = 0x21d;
            c->f94 = 0xb5;
            c->marks[slot / 3 + 3] = 1;
            count += 1;
            (*(int*)(((int)c + 0x98) & 0xFFFFFFFFFFFFFFFFLL))++;
        }
    }
    switch (slot) {
    case 0:
        if (c->cells[4] == 1 && c->cells[8] == 1) {
            c->lines[6] = 0x21d;
            c->f94 = 0xb5;
            c->marks[6] = 1;
            count += 1;
            (*(int*)(((int)c + 0x98) & 0xFFFFFFFFFFFFFFFFLL))++;
        }
        break;
    case 2:
        if (c->cells[4] == 1 && c->cells[6] == 1) {
            c->lines[7] = 0x21d;
            c->f94 = 0xb5;
            c->marks[7] = 1;
            count += 1;
            (*(int*)(((int)c + 0x98) & 0xFFFFFFFFFFFFFFFFLL))++;
        }
        break;
    case 4:
        if (c->cells[0] == 1 && c->cells[8] == 1) {
            c->lines[6] = 0x21d;
            c->f94 = 0xb5;
            c->marks[6] = 1;
            count += 1;
            (*(int*)(((int)c + 0x98) & 0xFFFFFFFFFFFFFFFFLL))++;
        }
        if (c->cells[2] == 1 && c->cells[6] == 1) {
            c->lines[7] = 0x21d;
            c->f94 = 0xb5;
            c->marks[7] = 1;
            count += 1;
            (*(int*)(((int)c + 0x98) & 0xFFFFFFFFFFFFFFFFLL))++;
        }
        break;
    case 6:
        if (c->cells[2] == 1 && c->cells[4] == 1) {
            c->lines[7] = 0x21d;
            c->f94 = 0xb5;
            c->marks[7] = 1;
            count += 1;
            (*(int*)(((int)c + 0x98) & 0xFFFFFFFFFFFFFFFFLL))++;
        }
        break;
    case 8:
        if (c->cells[0] == 1 && c->cells[4] == 1) {
            c->lines[6] = 0x21d;
            c->f94 = 0xb5;
            c->marks[6] = 1;
            count += 1;
            (*(int*)(((int)c + 0x98) & 0xFFFFFFFFFFFFFFFFLL))++;
        }
        break;
    }
    bingo = 1;
    {
    int j;
    for (j = 0; j < 9; j++) {
        if (j == slot)
            continue;
        if (c->cells[j] == 0) {
            bingo = 0;
            break;
        }
    }
    }
    if (count <= 0)
        return;
    m = 0;
    if (count > 0) {
        tmp = data_ov006_0212edfc;
        do {
            d[0] = c->f8;
            d[1] = c->fc;
            func_ov006_02115598(c->f4, d, tmp, 0, 1);
            func_ov006_02115040(c->f4, 1);
            m++;
        } while (m < count);
    }
    if (bingo != 1)
        return;
    e[0] = c->f8;
    e[1] = c->fc;
    func_ov006_02115598(c->f4, e, 0x2710, 0x3c, 1);
    func_ov006_02115024(c->f4);
    func_ov006_02114dfc(c->f4);
    _ZN5Sound12PlayBank2_2DEj(0x174);
}
