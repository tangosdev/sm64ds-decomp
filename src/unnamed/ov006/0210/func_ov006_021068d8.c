void func_ov006_021068d8(char* c){
    int i;
    for (i = 0; i < *(int*)(c + 0x4cbc); i++){
        *(unsigned char*)(c + i + 0x4efa) = 3;
    }
}
