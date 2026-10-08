extern int* data_ov007_0210342c;
extern short* func_ov007_020b8f8c(void);
extern void func_ov007_020c1d78(int i);
extern void func_ov007_020b9640(void);

void func_ov007_020af12c(void){
    int* base = (int*)((int**)data_ov007_0210342c)[2];
    int st = *(short*)base;
    int x = base[3];
    switch(st){
    case 0:
        if(x==0){
            short* r=func_ov007_020b8f8c();
            r[1]=1;
        }
        {
            short* r=func_ov007_020b8f8c();
            if(r[0]==2){
                short* o = (short*)((int**)data_ov007_0210342c)[2];
                o[1]=1;
            }
        }
        break;
    case 1:
        if(x==0){
            short* r=func_ov007_020b8f8c();
            r[1]=2;
            func_ov007_020c1d78(0);
        }
        {
            short* r=func_ov007_020b8f8c();
            if(r[0]==3){
                int* p = (int*)((int**)data_ov007_0210342c)[0x48];
                int* q = (int*)p[0];
                short* o = (short*)q[1];
                o[1]=2;
            }
        }
        break;
    }
    func_ov007_020b9640();
}
