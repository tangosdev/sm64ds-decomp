extern void func_020393a4(int* p, int v);
extern int func_ov002_020b6584(void* self, void* p, int x);
extern char data_ov036_021140d4[];
void func_ov022_02112040(char* self){
    switch(*(unsigned short*)(self+0xc)){
    case 0x4b:
        *(unsigned char*)(self+0x32c) = 0;
        func_020393a4((int*)(self+0x124), 0x650000);
        break;
    case 0x4c:
        *(unsigned char*)(self+0x32c) = 1;
        func_020393a4((int*)(self+0x124), 0x1000000);
        break;
    }
    unsigned char idx = *(unsigned char*)(self+0x32c);
    func_ov002_020b6584(self, data_ov036_021140d4 + idx*0xc, 0x1051);
}
