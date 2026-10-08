struct T867bc { char pad[0xe]; unsigned short h; };

extern struct T867bc *func_02060398(int arg);
extern struct T867bc *data_020a8760;
extern unsigned int data_020a8764;

void func_02060364(int arg)
{
    struct T867bc *p = func_02060398(arg);
    data_020a8760 = p;
    data_020a8764 = p->h;
}
