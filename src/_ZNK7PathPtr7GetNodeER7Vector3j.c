extern int func_0203acbc(void);
void _ZNK7PathPtr7GetNodeER7Vector3j(void* thiz, int* vec, unsigned int idx) {
    char* base = (char*)func_0203acbc();
    unsigned short cnt = *(unsigned short*)(*(int*)thiz);
    char* row = base + cnt * 6;
    int off = idx * 6;
    vec[0] = ((int)*(short*)(row + off)) << 12;
    vec[1] = ((int)*(short*)(row + off + 2)) << 12;
    vec[2] = ((int)*(short*)(row + off + 4)) << 12;
}
