extern int RandomIntInternal(void* seed);
extern int RNG_STATE[];
extern int data_ov006_0213d5e0[];
int func_ov006_020f7a00(void){
    unsigned char pick = 0;
    int total = 0;
    int i;
    int r;
    for(i=0;i<8;i++) total += data_ov006_0213d5e0[i];
    r = (int)(((unsigned int)RandomIntInternal(RNG_STATE) & 0x7fffffff) >> 0x13);
    total = (total * r) >> 0xc;
    for(i=0;i<8;i++){
        total -= data_ov006_0213d5e0[i];
        if(total < 0){
            pick = (unsigned char)i;
            data_ov006_0213d5e0[i]--;
            break;
        }
    }
    return pick;
}
