struct S{int w[3];}; extern struct S data_ov066_0211ad18;
void func_ov066_021162e8(int* c){
    int* p=(int*)(((long long)(int)((char*)c+0x338))&0xFFFFFFFFFFFFFFFFLL);
    *p|=2;
    *(int*)((char*)c+0x324)=0x64000;
    *(int*)((char*)c+0x328)=0x64000;
    data_ov066_0211ad18.w[0]=0;
    data_ov066_0211ad18.w[1]=0x20000;
    data_ov066_0211ad18.w[2]=-0x10000;
}
