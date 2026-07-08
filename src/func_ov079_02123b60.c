extern short SINE_TABLE[];
int func_ov079_02123b60(char* c){
    int idx;
    if(*(unsigned char*)(c+0x414)!=0){
        idx = ((int)*(unsigned short*)(c+0x8c) >> 4);
        return SINE_TABLE[idx*2+1] * 0x11d + 0x4b000;
    }
    idx = ((int)*(unsigned short*)(c+0x8c) >> 4);
    return SINE_TABLE[idx*2+1] * 0x90 + 0x24000;
}
