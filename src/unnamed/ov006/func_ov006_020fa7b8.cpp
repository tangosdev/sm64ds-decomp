//cpp
// func_ov006_020fa7b8 at 0x020fa7b8
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
extern "C" {
int _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int, void*, int, int, int, int, int, void*, int, int);
void* data_ov006_0213ac24;
}

extern "C" void func_ov006_020fa7b8(char* thiz)
{
    for (int i = 0; i < 3; i++) {
        if (*(unsigned char*)(thiz + 0x4000 + 0xe6d) != 0) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(false, data_ov006_0213ac24,
                *(int*)(thiz + 0x4000 + 0xe58) >> 12,
                *(int*)(thiz + 0x4000 + 0xe5c) >> 12,
                -1, -1, 0x1000, (void*)0x1000,
                *(unsigned short*)(thiz + 0x4e00 + 0x68), -1);
        }
        thiz += 0x18;
    }
}
