extern int data_ov006_0212ec08[];
#pragma opt_strength_reduction off
void func_ov006_020fffec(char *c){
    int i;
    for(i=0;i<2;i++){
        *(unsigned char*)(c+(i<<5)+0x5000+0x634) = 1;
        *(unsigned char*)(c+(i<<5)+0x5000+0x639) = 0;
        *(unsigned char*)(c+(i<<5)+0x5000+0x635) = 0;
        *(unsigned char*)(c+(i<<5)+0x5000+0x636) = 0;
        *(unsigned char*)(c+(i<<5)+0x5000+0x637) = 0;
        *(unsigned char*)(c+(i<<5)+0x5000+0x638) = 0;
        *(unsigned char*)(c+(i<<5)+0x5000+0x63a) = 0;
        *(int*)(c+(i<<5)+0x5000+0x620) = data_ov006_0212ec08[i] << 12;
        *(int*)(c+(i<<5)+0x5000+0x624) = 0x68000;
    }
}
