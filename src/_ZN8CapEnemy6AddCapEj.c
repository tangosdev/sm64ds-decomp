typedef unsigned char u8;
typedef unsigned int u32;

extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *sfp);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *this_, void *file, int a, int b);
extern int _ZN13SharedFilePtr7ReleaseEv(void *sfp);
extern void func_ov001_020ab228(char *c, char *a1, int idx, int a3, int a5);
extern int func_02005e28(unsigned char *self);

extern void *func_020ff028[];

struct CapEnemy {
    char pad0[0xb0];
    u32 fb0;
    char pad1[0x110 - 0xb4];
    u8 f110;
    u8 f111;
    u8 f112;
    u8 f113;
    char pad2[0x164 - 0x114];
    char base164[1];
};

int _ZN8CapEnemy6AddCapEj(struct CapEnemy *this_, unsigned int param)
{
    this_->f111 = 0;
    if (param >= 6) {
        this_->f113 = 6;
        return 6;
    }

    this_->f113 = (int)param % 3;

    if (param >= 3) {
        this_->f110 = 1;
        if (this_->f112 == 0) this_->f112 = 1;
    } else {
        this_->f110 = 0;
    }

    void *file = _ZN5Model8LoadFileER13SharedFilePtr(func_020ff028[this_->f113]);
    int ok = _ZN9ModelBase7SetFileEP8BMD_Fileii((char *)this_ + 0x114, file, 1, -1);
    if (ok == 0) {
        this_->f113 = 6;
        return _ZN13SharedFilePtr7ReleaseEv(func_020ff028[this_->f113]);
    }

    *(int *)(((long long)(int)((char *)this_ + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL) &= ~1;
    func_ov001_020ab228((char *)this_ + 0x164, (char *)this_, this_->f113, this_->f112, this_->f110 != 0);

    int result = func_02005e28((unsigned char *)this_);
    if (result != 0) {
        unsigned char *p = (unsigned char *)(((long long)(int)((char *)this_ + 0x113)) & 0xFFFFFFFFFFFFFFFFLL);
        result = *p | 0x80;
        *p = result;
    }
    return result;
}
