//cpp
extern "C" {
extern signed char LEVEL_ID;
extern unsigned char CURRENT_GAMEMODE;
extern int _ZN6Player13ST_ENDING_FLYE;
int _ZN6Player7IsStateERNS_5StateE(void* c, void* s);
int _ZN8SaveData16HasPlayerLostCapEv(void);
int _ZN6Player8HasNoCapEv(char* c){
    if(LEVEL_ID == 0x1f) return 0;
    int r = (CURRENT_GAMEMODE == 2);
    if(!r){
        if(!_ZN6Player7IsStateERNS_5StateE(c, &_ZN6Player13ST_ENDING_FLYE)){
            r = (CURRENT_GAMEMODE == 1);
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
