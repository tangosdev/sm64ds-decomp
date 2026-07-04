extern void* _ZN9ActorBasenwEj(unsigned int size);
extern void _ZN9ActorBaseC1Ev(void* self);
extern int data_0208e4b8[];
extern int data_02092680[];
extern int data_02091528[];

void* func_02023624(void)
{
    char* self = (char*)_ZN9ActorBasenwEj(0x58);
    if (self != 0) {
        _ZN9ActorBaseC1Ev(self);
        *(int**)self = data_0208e4b8;
        *(int**)self = data_02092680;
        {
            unsigned char* f = (unsigned char*)(((long long)(int)(self + 0x13)) & 0xFFFFFFFFFFFFFFFFLL);
            *f |= 1;
            *f |= 4;
        }
        *(int**)self = data_02091528;
    }
    return self;
}
