extern void func_ov004_020afdd0(int a, int b, int c, int d, int e);
extern int data_ov006_0213def0[];
void func_ov006_021042e8(char *c){
    int i;
    for(i=0;i<0x40;i++){
        if(*(unsigned char*)(c+0x4000+0x6bd) != 0){
            func_ov004_020afdd0(
                data_ov006_0213def0[*(unsigned char*)(c+0x4000+0x6ba)],
                *(int*)(c+0x4000+0x6a8) >> 12,
                *(int*)(c+0x4000+0x6ac) >> 12,
                -1, -1);
        }
        c += 0x18;
    }
}
