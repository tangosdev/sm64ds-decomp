extern void _ZN9Animation7AdvanceEv(char *self);

void _ZN14BlendModelAnim7AdvanceEv(char *self)
{
    _ZN9Animation7AdvanceEv(self + 0x50);
    if (*(int *)(self + 0x64) < 0x1000) {
        *(int *)(((long long)(int)(self + 0x64)) & 0xFFFFFFFFFFFFFFFFLL) += *(int *)(self + 0x68);
    }
}
