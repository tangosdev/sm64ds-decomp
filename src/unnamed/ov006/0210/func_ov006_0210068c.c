extern void func_ov004_020afdd0(int a, int b, int c, int d, int e);
extern int data_ov006_0213386c;

struct E {
    int x;                          /* +0x00 (abs 0x5260) */
    int y;                          /* +0x04 (abs 0x5264) */
    unsigned char pad0[0x35 - 0x08];
    unsigned char flag;             /* +0x35 (abs 0x5295) */
    unsigned char pad1[0x40 - 0x36];
};

struct Obj {
    unsigned char pad[0x5260];
    struct E arr[3];
};

void func_ov006_0210068c(struct Obj *o){
    int i;
    for(i=0;i<3;i++){
        if(o->arr[i].flag){
            func_ov004_020afdd0(
                data_ov006_0213386c,
                o->arr[i].x >> 12,
                o->arr[i].y >> 12,
                -1, -1);
        }
    }
}
