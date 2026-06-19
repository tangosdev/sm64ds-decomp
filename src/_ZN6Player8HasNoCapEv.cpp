//cpp
extern "C" {
extern signed char data_0209f2f8;
extern unsigned char data_0209f2d8;
extern int data_ov002_0211058c;
int _ZN6Player7IsStateERNS_5StateE(void* c, void* s);
int _ZN8SaveData16HasPlayerLostCapEv(void);
int _ZN6Player8HasNoCapEv(char* c){
    if(data_0209f2f8 == 0x1f) return 0;
    int r = (data_0209f2d8 == 2);
    if(!r){
        if(!_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_0211058c)){
            r = (data_0209f2d8 == 1);
            if(!r){
                if(_ZN8SaveData16HasPlayerLostCapEv()) return 1;
            }
        }
    }
    if(*(int*)(c+8) == 3) return 0;
    r = *(unsigned char*)(c+0x71a) != 0;
    return r;
}
}
