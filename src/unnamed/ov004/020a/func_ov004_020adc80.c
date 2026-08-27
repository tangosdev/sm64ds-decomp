extern int GetGameLanguage(void);
extern int *data_ov004_020bbfa8[];
extern int func_ov004_020b1d60(int,int,int,long long);
void func_ov004_020adc80(int *c){
    int idx=GetGameLanguage();
    int *t=data_ov004_020bbfa8[idx];
    func_ov004_020b1d60(t[2], c[0]>>0xc, c[1]>>0xc, -1);
}
