// @symbol func_ov079_02123b60
// @emits Whomp_OnAimedAtWithEgg
/* recovered: renamed to Class_Method */
/* daBtn_c::OnAimedAtWithEgg - recovered from vtable slot identity */
extern short data_02082214[];
int Whomp_OnAimedAtWithEgg(char* c){
    int idx;
    if(*(unsigned char*)(c+0x414)!=0){
        idx = ((int)*(unsigned short*)(c+0x8c) >> 4);
        return data_02082214[idx*2+1] * 0x11d + 0x4b000;
    }
    idx = ((int)*(unsigned short*)(c+0x8c) >> 4);
    return data_02082214[idx*2+1] * 0x90 + 0x24000;
}
