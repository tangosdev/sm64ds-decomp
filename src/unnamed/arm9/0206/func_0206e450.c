extern void* func_0206e310(void* dst, const void* src, unsigned int n);

int func_0206e450(char* obj, const void* src, unsigned int n)
{
    unsigned int off = *(unsigned int*)(obj + 8);
    unsigned int len = *(unsigned int*)(obj + 4);
    if (off + n > len)
        n = len - off;
    func_0206e310(*(char**)(obj) + off, src, n);
    {
        int* p = (int*)(obj + 8);
        *p = *p + n;
    }
    return 1;
}
