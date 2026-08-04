extern int RandomIntInternal(int *seed);
extern void func_02012718(void *a, int b);
extern int data_0209d4b8;
extern int data_ov006_0212e370[];
extern int data_ov006_0212e388[];
extern int data_ov006_0212e344[];

typedef struct {
    int field_00;
    int field_04;
    int field_08;
    int field_0c;
    unsigned short field_10;
    unsigned short field_12;
    unsigned char field_14;
    unsigned char field_15;
    unsigned char field_16;
    unsigned char field_17;
    unsigned char field_18;
    unsigned char field_19;
    unsigned char field_1a;
    unsigned char field_1b;
} Sub;

struct Obj {
    unsigned char pad[0x51a8];
    Sub arr[1];
};

void func_ov006_020dbf7c(struct Obj *sb, int i)
{
    int v;
    int k;

    sb->arr[i].field_00 += sb->arr[i].field_08;
    sb->arr[i].field_04 += sb->arr[i].field_0c;
    sb->arr[i].field_0c += 0x400;
    if (sb->arr[i].field_08 > 0) {
        sb->arr[i].field_10 += 0x400;
    } else {
        sb->arr[i].field_10 -= 0x400;
    }

    v = sb->arr[i].field_04 >> 12;
    k = sb->arr[i].field_19;

    if (k >= 6) {
        if (v >= 0xd0) {
            sb->arr[i].field_15 = 0;
            sb->arr[i].field_14 = 0;
        }
        return;
    }

    if (sb->arr[i].field_18 != 0) {
        unsigned int r;
        if (v < (data_ov006_0212e370[k] - data_ov006_0212e388[k])) {
            return;
        }
        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        sb->arr[i].field_08 = data_ov006_0212e344[((r >> 16 & 0x7fff) << 1) >> 15];
        sb->arr[i].field_0c = -0x3000;
        sb->arr[i].field_18--;
        func_02012718((void *)0xef, sb->arr[i].field_00);
        if (sb->arr[i].field_18 == 0) {
            sb->arr[i].field_19++;
        }
    } else {
        sb->arr[i].field_18 = 1;
    }
}
