// NONMATCHING: entry-load/00b4 setup; switch schedule; type5 close (div=92)
typedef unsigned char u8;
typedef unsigned int u32;

extern u8 data_0209b304;
extern int data_0208a170;

typedef struct Pair {
    int x, y;
} Pair;

typedef struct Entry {
    int pad0;
    int unk4;
    int unk8;
    int unkC;
    int unk10;
    int arr[5];
    int unk28;
    int unk2c;
    int unk30;
} Entry;

extern Pair data_ov005_020c2280[];
extern Entry data_ov005_020c24d8[];
extern void* data_ov005_020c2f4c[];

extern int func_ov005_020c00b4(void* self, int n);
extern int func_ov005_020bfff4(void* self, int n, int c);
extern void func_ov005_020c1030(void* self, int x, int y, int val);
extern int _ZN3OAM9RenderSubEP7OamAttriiii(void* oam, int x, int y, int a, int b);

void func_ov005_020c1130(char* sl)
{
    int end;
    int i;
    int r8;
    int zero;
    int neg1;
    int type;
    int idx;
    int x, y;
    int val;
    unsigned int u;
    unsigned int major;
    unsigned int rem;
    unsigned int pct;

    if (*(u8*)(sl + 0x54) == 1)
        return;

    if (data_0209b304 == 1) {
        end = 8;
        i = 6;
    } else {
        end = 5;
        i = 0;
    }
    if (i > end)
        return;

    zero = 0;
    neg1 = -1;
    r8 = i * 4;
    do {
        idx = data_0208a170 + r8;
        if (func_ov005_020c00b4(sl, idx) != 0) {
            type = data_ov005_020c24d8[idx].unk8;
            if (type != 0) {
                if (type == 1) {
                    y = data_ov005_020c2280[i].y;
                    x = data_ov005_020c2280[i].x;
                    _ZN3OAM9RenderSubEP7OamAttriiii(data_ov005_020c2f4c[0], x - 0x10, y, neg1, zero);
                    val = func_ov005_020bfff4(sl, data_0208a170 + r8, zero);
                    func_ov005_020c1030(sl, x - 4, y, val);
                } else if (type == 2) {
                    y = data_ov005_020c2280[i].y;
                    x = data_ov005_020c2280[i].x;
                    _ZN3OAM9RenderSubEP7OamAttriiii(data_ov005_020c2f4c[1], x - 0x10, y, neg1, zero);
                    val = func_ov005_020bfff4(sl, data_0208a170 + r8, zero);
                    func_ov005_020c1030(sl, x - 4, y, val);
                } else if (type == 4) {
                    y = data_ov005_020c2280[i].y;
                    x = data_ov005_020c2280[i].x;
                    _ZN3OAM9RenderSubEP7OamAttriiii(data_ov005_020c2f4c[2], x - 0x10, y, neg1, zero);
                    val = func_ov005_020bfff4(sl, data_0208a170 + r8, zero);
                    func_ov005_020c1030(sl, x - 4, y, val);
                } else if (type == 3) {
                    val = func_ov005_020bfff4(sl, idx, zero);
                    x = data_ov005_020c2280[i].x;
                    y = data_ov005_020c2280[i].y;
                    func_ov005_020c1030(sl, x - 0x14, y, val);
                } else if (type == 5) {
                    y = data_ov005_020c2280[i].y;
                    x = data_ov005_020c2280[i].x;
                    val = func_ov005_020bfff4(sl, idx, zero);
                    u = (unsigned int)val;
                    major = u / 60u;
                    func_ov005_020c1030(sl, x - 0x10, y, (int)major);
                    _ZN3OAM9RenderSubEP7OamAttriiii(data_ov005_020c2f4c[4], x, y - 8, neg1, zero);

                    val = func_ov005_020bfff4(sl, data_0208a170 + r8, zero);
                    u = (unsigned int)val;
                    rem = u % 60u;
                    /* rem/6 == (rem*100/60)/10 — ROM uses rem*100 then magic /600 */
                    pct = rem * 100u;
                    func_ov005_020c1030(sl, x + 8, y, (int)(pct / 600u));

                    val = func_ov005_020bfff4(sl, data_0208a170 + r8, zero);
                    u = (unsigned int)val;
                    rem = u % 60u;
                    pct = rem * 100u / 60u;
                    func_ov005_020c1030(sl, x + 0x10, y, (int)(pct % 10u));
                }
            }
        }
        i += 1;
        r8 = i * 4;
    } while (i <= end);
}
