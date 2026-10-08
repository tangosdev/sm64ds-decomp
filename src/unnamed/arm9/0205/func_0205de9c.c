void func_0205a588(void* p, int v, int n);
void CpuCopy8(int a, void* b, int c);
void func_0205e0f4(void* a, int b, int c, void* d, int e);

extern int data_0209a04c;
extern int data_0209a050;

int func_0205de9c(char* arg0, int arg1, int arg2)
{
    int buf1[5];
    int buf2[16];
    unsigned int i;
    int* p;
    func_0205a588(buf1, 0, 0x14);
    CpuCopy8(data_0209a04c, buf2, data_0209a050);
    func_0205e0f4(buf1, arg1, arg2, buf2, data_0209a050);
    p = buf1;
    for (i = 0; i < 0x14; i += 4) {
        if (*p != *(int*)(arg0 + i))
            break;
        p++;
    }
    return i == 0x14;
}
