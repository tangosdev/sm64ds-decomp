extern char* data_ov007_0210342c;
extern int _ZN4cstd3modEii(int,int);
extern int _ZN4cstd3divEii(int,int);
extern int func_ov007_020b8fb4(void);
extern int func_ov007_020af260(int,int,int*,int*);
extern int func_ov007_020b8f64(void);
extern int func_ov007_020b8f50(void);
extern int func_ov007_020b9640(void);

void func_ov007_020aefb4(void){
    int r6=0;
    int n=*(int*)(*(int*)(data_ov007_0210342c+8)+0xc)-0x3c;
    if(n>=0){
        int rem=_ZN4cstd3modEii(n,0xf0);
        int quo=_ZN4cstd3divEii(n-rem,0xf0);
        if(rem==0){
            char* p=*(char**)(*(char**)(data_ov007_0210342c+0xa8));
            *(int*)(p+0x1c)=0x3c;
            *(int*)(p+0x18)=*(int*)(p+0x1c);
        }
        int r7 = (*(short*)(*(int*)(data_ov007_0210342c+4)+0xa) == 3) ? 1 : 0;
        if(func_ov007_020b8fb4()!=0 && quo>=r7){
            int a=-1, b=-1;
            func_ov007_020af260(quo, rem, &a, &b);
            if(a!=-1) *(short*)((char*)func_ov007_020b8f64()+2) = (short)a;
            if(b!=-1) *(short*)((char*)func_ov007_020b8f50()+2) = (short)b;
        } else { r6=1; }
    } else { r6=1; }
    if(r6){
        *(short*)((char*)func_ov007_020b8f64()+2)=0;
        *(short*)((char*)func_ov007_020b8f50()+2)=0;
    }
    func_ov007_020b9640();
}
