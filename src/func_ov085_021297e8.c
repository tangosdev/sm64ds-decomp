extern void _ZN13SharedFilePtr7ReleaseEv(void *self);
extern int MARIO_CAP_MODEL_PTR[];
extern int LUIGI_CAP_MODEL_PTR[];
extern int WARIO_CAP_MODEL_PTR[];
extern int data_ov085_02130480[];
extern int data_ov085_02130488[];
extern int data_ov085_02130490[];

int func_ov085_021297e8(void *c)
{
    if (*(unsigned char *)((char *)c + 0x20b) == 1) {
        _ZN13SharedFilePtr7ReleaseEv(MARIO_CAP_MODEL_PTR);
        _ZN13SharedFilePtr7ReleaseEv(LUIGI_CAP_MODEL_PTR);
        _ZN13SharedFilePtr7ReleaseEv(WARIO_CAP_MODEL_PTR);
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov085_02130480);
    _ZN13SharedFilePtr7ReleaseEv(data_ov085_02130488);
    _ZN13SharedFilePtr7ReleaseEv(data_ov085_02130490);
    return 1;
}
