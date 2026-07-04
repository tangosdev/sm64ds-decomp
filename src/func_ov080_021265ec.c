typedef unsigned int u32;
typedef unsigned char u8;
extern void func_ov080_02126124(void* c);
extern void func_ov080_02125de0(void* c, int a, int b, int d);
struct Cell { int f0, f4, f8, fc, f10, f14; };

void func_ov080_021265ec(char* c){
    int sb=0, r8=0, r7=0, r4=0;
    int n=*(u8*)(c+0x1bb);
    if(n > 0){
        int fp=0x1ff00000;
        do {
            int r6=0;
            int m=*(u8*)(c+0x1ba);
            if(m > 0){
                do {
                    struct Cell* g=(struct Cell*)(*(char**)(c+0x1a0) + (r7*(*(u8*)(c+0x1bb))+r6)*0x18);
                    int r5;
                    g->f0=sb; g->f4=r8; g->f8=r4; g->f10=fp;
                    r5=*(u8*)(c+0x1ba);
                    if(r6==r5-2) sb=((u8)(*(u32*)(c+8)&0xf)+1)*0x64000;
                    else sb += ((u8)(*(u32*)(c+8)&0xf)+1)*0x64000/(r5-1);
                    r6++;
                } while(r6 < *(u8*)(c+0x1ba));
            }
            {
                int r5=*(u8*)(c+0x1bb);
                sb=0;
                if(r7==r5-2) r8=((u8)((*(u32*)(c+8)>>4)&0xf)+1)*0x64000;
                else r8 += ((u8)((*(u32*)(c+8)>>4)&0xf)+1)*0x64000/(r5-1);
                r7++;
                if(r7 >= r5) break;
            }
        } while(1);
    }
    func_ov080_02126124(c);
    {
        int a2=((u8)(*(u32*)(c+8)&0xf)+1)*0x64000;
        int a1=((u8)((*(u32*)(c+8)>>4)&0xf)+1)*0x64000;
        func_ov080_02125de0(c, a2/2, a1/2, 0);
    }
}
