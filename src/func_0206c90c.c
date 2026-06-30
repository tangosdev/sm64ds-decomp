typedef unsigned short u16;
typedef struct { int a, b, c, d; } S;
int func_0206c90c(S s){
    char* q = (char*)&s + 7;
    int hi;
    if ((int)q & 1)
        hi = (*(u16*)(q - 1) & 0xff00) >> 8;
    else
        hi = *(u16*)q & 0xff;
    return hi & 0x80;
}
