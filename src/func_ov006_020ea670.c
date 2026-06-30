extern int func_ov004_020ad674(void);
extern void func_ov004_020af948(void* a, int b, int c, void* m);

typedef struct {
    int x;
    int y;
    unsigned char pad0[7];
    unsigned char flag;
    int pad1;
} Ent;

extern Ent data_ov006_02142044[];
extern char data_ov006_02137cd8[];
extern void* data_ov006_0213ca9c[];

void func_ov006_020ea670(void){
    void *a=0, *b=0, *c=0;
    int i;
    for(i=0;i<16;i++){
        Ent *e = &data_ov006_02142044[i];
        if(e->flag!=0){
            int sb=e->x>>12;
            int r8=e->y>>12;
            int idx=func_ov004_020ad674();
            void *obj=data_ov006_0213ca9c[idx];
            func_ov004_020af948(*(void**)((char*)obj+0x38), sb-0x10, r8, a);
            func_ov004_020af948(*(void**)(data_ov006_02137cd8+0xa4), sb, r8, b);
            func_ov004_020af948(*(void**)(data_ov006_02137cd8+0xa0), sb+0x10, r8, c);
        }
    }
}
