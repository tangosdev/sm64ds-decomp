struct SharedFilePtr { unsigned int data[4]; };
extern void _ZN13SharedFilePtr7ReleaseEv(struct SharedFilePtr *self);
extern void* _ZN5Actor10FindWithIDEj(unsigned int id);
extern void func_ov100_02146280(void);

extern struct SharedFilePtr data_ov100_021489cc;
extern struct SharedFilePtr *data_ov100_021473a4[];
extern struct SharedFilePtr *data_ov100_021473b0[];

int func_ov100_02146a98(char* c)
{
    _ZN13SharedFilePtr7ReleaseEv(&data_ov100_021489cc);
    _ZN13SharedFilePtr7ReleaseEv(data_ov100_021473a4[*(unsigned char*)(c + 0x15b)]);
    _ZN13SharedFilePtr7ReleaseEv(data_ov100_021473b0[*(unsigned char*)(c + 0x15b)]);
    if (*(unsigned char*)(c + 0x159) == 0) {
        if (_ZN5Actor10FindWithIDEj(*(unsigned int*)(c + 0x13c)))
            func_ov100_02146280();
    }
    return 1;
}
