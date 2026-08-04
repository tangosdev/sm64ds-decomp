//cpp
struct BCA_File;
struct Animation { int Finished(); int WillHitFrame(int) const; };
struct ModelAnim { void SetAnim(BCA_File *f, int a, int b, unsigned int c); };
extern "C" void *data_ov006_02141e8c;
extern "C" void *data_ov006_02141e84;
extern "C" void _ZN5Sound12PlayBank2_2DEj(unsigned int);

extern "C" void func_ov006_020e7cc0(char *thiz)
{
    if (((Animation*)(thiz + 0x5c))->Finished() != 0 &&
        *(void**)(thiz + 0x6c) == ((void**)&data_ov006_02141e8c)[1]) {
        ((ModelAnim*)(thiz + 0xc))->SetAnim(
            (BCA_File*)((void**)&data_ov006_02141e84)[1], 0, 0x800, 0);
        return;
    }
    if (((Animation*)(thiz + 0x5c))->WillHitFrame(0x10) == 0 &&
        ((Animation*)(thiz + 0x5c))->WillHitFrame(0x50) == 0)
        return;
    if (*(int*)(thiz + 0x20c) != 0)
        return;
    _ZN5Sound12PlayBank2_2DEj(0x18f);
}
