struct B72 {
    unsigned short f0 : 2;
    unsigned short f1 : 2;
    unsigned short f2 : 2;
    unsigned short f3 : 2;
};

int func_ov006_020ebc08(char* c, int n) {
    switch (n) {
    case 0:
        return *(unsigned char*)(c + 0x95);
    case 1:
        return ((struct B72*)(c + 0x74))->f0 + 7;
    case 2:
        return ((struct B72*)(c + 0x74))->f1 + 7;
    case 3:
        return ((struct B72*)(c + 0x74))->f2 + 7;
    default:
        return ((struct B72*)(c + 0x74))->f3 + 7;
    }
}
