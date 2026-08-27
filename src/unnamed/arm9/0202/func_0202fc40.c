extern void func_0202ed14(char *o);
extern void *data_0208eafc;
extern void *data_0208eacc;
extern void *data_0208eb2c;
extern void *data_020926f0;

void *func_0202fc40(char *self) {
    *(void**)self = &data_0208eafc;
    *(void**)self = &data_0208eacc;
    *(int*)(self + 4) = 0;
    *(int*)(self + 8) = 0;
    *(void**)self = &data_0208eb2c;
    *(short*)(self + 0xc) = 0;
    *(void**)self = &data_020926f0;
    func_0202ed14(self);
    return self;
}
