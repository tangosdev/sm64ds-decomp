#include "types.h"
extern char data_0209b53c[];
int func_02048a1c(int a0, int kind, int id);
void* func_02011934(char* table, int id);
void func_02048d80(void* p, int v);
void func_0204f7fc(void* c, int a, int b);
void* func_0201179c(char* table, int a1, int a2, int s0, short s1);

int func_02012194(char* c, int a1, int a2, int a3, int s0, int s1, short s2)
{
    char* e;
    if (func_02048a1c(s1, a1, a2) == 0) return 0;
    e = (char*)func_02011934(data_0209b53c, (int)c);
    if (e != 0) {
        char* slot = e + 8;
        e[6] = 1;
        func_02048d80(slot, s1);
        func_0204f7fc(slot, a3, s0);
        return *(int*)e;
    } else {
        char* n = (char*)func_0201179c(data_0209b53c, a1, a2, s1, s2);
        char* e2 = (char*)func_02011934(data_0209b53c, (int)n);
        if (e2 != 0)
            func_0204f7fc(e2 + 8, a3, s0);
        return (int)n;
    }
}
