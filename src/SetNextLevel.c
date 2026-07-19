typedef unsigned char u8;
typedef signed char s8;

extern s8 data_0209f2f8;
extern s8 data_0209211c;
extern u8 data_0209f200;
extern u8 data_02092110;
extern u8 data_0209f268;
extern u8 data_0209f220;
extern u8 data_0209f1f0;
extern u8 data_0209f26c;

struct Entry {
    s8 a;
    s8 b;
    u8 c;
};
extern struct Entry data_02075638[];

int SublevelToLevel(int i);
void SetNextStar(void);

void SetNextLevel(int arg) {
    int v = data_0209f2f8;
    unsigned int i;
    struct Entry *p;

    if (v == 0x22) {
        if (arg != 1) {
            data_0209211c = 1;
            data_0209f200 = 0xa;
        }
    } else {
        p = data_02075638;
        for (i = 0; i < 8; i++) {
            if (v == p->a) {
                data_0209211c = p->b;
                data_0209f200 = p->c;
                break;
            }
            p++;
        }
    }

    if (data_0209211c < 0 || SublevelToLevel(v) == 0x1d) {
        data_02092110 = 1;
        data_0209f268 = 0xd;
    } else {
        data_02092110 = data_0209211c;
        data_0209f268 = data_0209f200;
    }

    data_0209211c = -1;
    data_0209f1f0 = data_0209f220;
    SetNextStar();
    data_0209f26c = arg;
}
