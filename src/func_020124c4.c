struct Obj125 { int first; unsigned char pad[2]; unsigned char flag6; };
extern char data_0209b53c;
extern struct Obj125 *func_02011934(char *table, int id);
extern int func_0201186c(char *table, int b, int c, int d);

int func_020124c4(int a, int b, int c, int d) {
    struct Obj125 *o = func_02011934(&data_0209b53c, a);
    if (o) {
        o->flag6 = 1;
        return o->first;
    }
    return func_0201186c(&data_0209b53c, b, c, d);
}
