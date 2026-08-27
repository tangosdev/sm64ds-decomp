int func_02048a1c(int a);
void* func_02011934(char* table, int id);
void func_02048c30(void* a, int b, int c);
int func_020116c4(char* table, int a, int b, int c, int d, short e);

extern char data_0209b53c[];

int func_0201226c(int a0, int a1, int a2, int a3, int a4, short a5)
{
    void* p;
    if (func_02048a1c(a3) == 0) return 0;
    p = func_02011934(data_0209b53c, a0);
    if (p != 0) {
        *(unsigned char*)((char*)p + 6) = 1;
        func_02048c30((char*)p + 8, a3, a4);
        return *(int*)p;
    }
    return func_020116c4(data_0209b53c, a1, a2, a3, a4, a5);
}
