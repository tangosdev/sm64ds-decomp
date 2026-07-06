unsigned char IsAreaShowing(int idx);
void func_ov063_0211c82c(char* c){
    if (*(unsigned char*)(c + 0x152) == 0) {
        *(unsigned char *)(((int)c + 0x152) & 0xFFFFFFFFFFFFFFFF) += 1;
        return;
    }
    if (*(int*)(c + 0x124) != 0 || IsAreaShowing(*(signed char*)(c + 0xcc))) {
        *(unsigned char*)(c + 0x153) = 1;
    } else {
        *(unsigned char*)(c + 0x153) = 0;
    }
}
