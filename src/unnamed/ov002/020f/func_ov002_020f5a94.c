extern int _ZN8SaveData19IsCharacterUnlockedEj(unsigned int idx);

int func_ov002_020f5a94(void){
    unsigned char count = 0;
    int i;
    for (i = 0; i < 4; i++) {
        if (_ZN8SaveData19IsCharacterUnlockedEj(i))
            count = count + 1;
    }
    return count;
}
