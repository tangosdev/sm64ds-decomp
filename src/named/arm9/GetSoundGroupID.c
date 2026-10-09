extern unsigned char data_02075768[];

int GetSoundGroupID(int group, int sub){
    unsigned char v = data_02075768[group * 3];
    if (group != 6) return v;
    if (sub == 1 || sub == 4) return 4;
    if (sub == 2) return 5;
    return v;
}
