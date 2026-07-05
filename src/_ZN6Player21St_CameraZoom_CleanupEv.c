int _ZN6Player21St_CameraZoom_CleanupEv(char *self)
{
    *(unsigned short *)(((long long)(int)(self + 0x6ce)) & 0xFFFFFFFFFFFFFFFFLL) &= ~4;
    *(unsigned char *)(self + 0x715) = 0;
    return 1;
}
