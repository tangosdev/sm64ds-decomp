//cpp
struct SharedFilePtr { void Release(); };
extern SharedFilePtr data_ov002_0210da28;
extern SharedFilePtr data_ov002_0210da08;
extern SharedFilePtr data_ov002_0210d9a8;
extern SharedFilePtr data_ov002_0210d9e8;

extern "C" int func_ov002_020f093c(char *self) {
    if (*(int *)(self + 8) & 0x10) {
        data_ov002_0210da28.Release();
        data_ov002_0210da08.Release();
    } else {
        data_ov002_0210d9a8.Release();
        data_ov002_0210d9e8.Release();
    }
    return 1;
}
