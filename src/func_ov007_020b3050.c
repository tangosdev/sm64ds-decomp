extern char* data_ov007_02103360;
extern void func_ov007_020c937c(void* a);
extern int func_ov007_020c44c4(char* a);
extern void func_ov007_020c3d1c(void* self);

void func_ov007_020b3050(void) {
    if (data_ov007_02103360 == 0) return;
    func_ov007_020c937c(*(void**)data_ov007_02103360);
    func_ov007_020c44c4(((char**)data_ov007_02103360)[1]);
    func_ov007_020c3d1c(data_ov007_02103360);
    data_ov007_02103360 = 0;
}
