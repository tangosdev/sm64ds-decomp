extern void* _ZN7fBase_cnwEj(unsigned int size);
extern void _ZN7fBase_cC2Ev(void* self);
extern int data_0208e4b8[];
extern int _ZTV8dScene_c[];
extern int data_02091528[];

void* func_02023624(void)
{
    char* self = (char*)_ZN7fBase_cnwEj(0x58);
    if (self != 0) {
        _ZN7fBase_cC2Ev(self);
        *(int**)self = data_0208e4b8;
        *(int**)self = _ZTV8dScene_c;
        {
            unsigned char* f = (unsigned char*)(self + 0x13);
            *f |= 1;
            *f |= 4;
        }
        *(int**)self = data_02091528;
    }
    return self;
}
