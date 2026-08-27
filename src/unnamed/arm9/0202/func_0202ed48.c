struct Obj { char pad[0xf]; unsigned char f; };
extern struct Obj data_0209f61c;
extern void func_0202fb30(struct Obj *o);
extern void func_0202ed14(struct Obj *o);

void func_0202ed48(void)
{
    if (data_0209f61c.f == 1)
        func_0202fb30(&data_0209f61c);
    func_0202ed14(&data_0209f61c);
}
