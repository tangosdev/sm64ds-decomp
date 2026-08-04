void func_ov006_02106048(char* c){
    int i;
    for (i = 0; i < *(int*)(c + 0x4cb8); i++){
        *(unsigned char*)(c + i + 0x4f66) = 1;
    }
}
