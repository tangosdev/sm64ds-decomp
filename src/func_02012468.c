// Creates/registers an entry via func_020124c4(a,b,c,h), looks it up in the
// data_0209b53c table via func_02011934, and if found forwards (d,e,f,g) to
// func_02012860 on the entry's +8 sub-object. Returns the id from func_020124c4.
// The u64-mask launder on d pins it to r4 (birth-order class) as in the ROM.
extern int func_020124c4(int a, int b, int c, int d);
extern char *func_02011934(char *table, int id);
extern void func_02012860(void *a, int flags, int c, int d, int e);

extern char data_0209b53c;

int func_02012468(int a, int b, int c, int d, int e, int f, int g, short h)
{
    int dd = (int)(((long long)d) & 0xFFFFFFFFFFFFFFFFLL);
    int x;
    char *p;

    x = func_020124c4(a, b, c, h);
    p = func_02011934(&data_0209b53c, x);
    if (p != 0)
        func_02012860(p + 8, dd, e, f, g);
    return x;
}
