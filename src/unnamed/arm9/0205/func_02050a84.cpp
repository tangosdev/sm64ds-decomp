//cpp
extern "C" {
extern char* data_020a5bb8;

int func_02050a84(unsigned int id)
{
    char* root = data_020a5bb8;
    char* sub = *(char**)(root + 0x84);
    int off = *(int*)(sub + 0x1c);
    char* tbl = (off == 0) ? 0 : sub + off;
    if (tbl == 0) return 0;
    if ((int)id < 0) return 0;
    if (id >= *(unsigned*)tbl) return 0;
    int v = ((int*)(tbl + id * 4))[1];
    char* base = *(char**)(root + 0x84);
    return (v == 0) ? 0 : (int)(base + v);
}
}
