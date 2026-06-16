struct S3 { int w0; int w1; int w2; };
extern struct S3 data_ov019_021135d8;
int func_ov019_0211271c(void* c) {
    *(int*)((char*)c + 0x5c) = data_ov019_021135d8.w0;
    *(int*)((char*)c + 0x60) = data_ov019_021135d8.w1;
    *(int*)((char*)c + 0x64) = data_ov019_021135d8.w2;
    *(short*)((char*)c + 0xd4) = 0x78;
    return 1;
}
