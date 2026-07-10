extern void _ZN13SharedFilePtr7ReleaseEv(void *self);
extern char data_ov100_02148668;

int _ZN15RollingIronBall16CleanupResourcesEv(char *c)
{
    char *file = *(char **)(c + 0x3a8);

    if (file != 0) {
        (*(unsigned char *)(((int)file + 0x3d2) & 0xFFFFFFFFFFFFFFFF))--;
    }

    _ZN13SharedFilePtr7ReleaseEv(&data_ov100_02148668);
    return 1;
}
