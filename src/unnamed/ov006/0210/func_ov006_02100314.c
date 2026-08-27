extern void func_ov004_020afdd0(int a, int b, int c, int d, int e);
extern int data_ov006_02136bd4[];
void func_ov006_02100314(char *c){
    int i;
    for(i=0;i<0x10;i++){
        if(*(unsigned char*)(c+0x5000+0x334) != 0){
            func_ov004_020afdd0(
                data_ov006_02136bd4[*(unsigned char*)(c+0x5000+0x335)],
                *(int*)(c+0x5000+0x320) >> 12,
                *(int*)(c+0x5000+0x324) >> 12,
                -1, -1);
        }
        c += 0x18;
    }
}
