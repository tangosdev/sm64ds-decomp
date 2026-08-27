extern char* data_ov007_0210342c;

void func_ov007_020b257c(void) {
    int* p;
    p = *(int**)(*(char**)((char*)data_ov007_0210342c + 0xf4) + 0x18);
    if (*p == 0) *p = 1;
    p = *(int**)(*(char**)((char*)data_ov007_0210342c + 0xf4) + 0x24);
    if (*p == 0) *p = 1;
    p = *(int**)(*(char**)((char*)data_ov007_0210342c + 0xf4) + 0x30);
    if (*p == 0) *p = 1;
    *(int*)*(char**)(*(char**)(*(char**)((char*)data_ov007_0210342c + 0x100) + 4) + 0x18) = 2;
    *(int*)*(char**)(*(char**)(*(char**)((char*)data_ov007_0210342c + 0x104) + 4) + 0x18) = 2;
}
