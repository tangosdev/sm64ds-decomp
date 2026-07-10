typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef signed int s32;

struct SharedFilePtr { u16 fileID; u8 numRefs; char* filePtr; };

extern void _ZN13SharedFilePtr7ReleaseEv(struct SharedFilePtr *self);

extern struct SharedFilePtr data_ov077_02127b50;
extern struct SharedFilePtr data_ov077_02127b48;
extern struct SharedFilePtr data_ov077_02127b38;
extern struct SharedFilePtr* data_ov077_02127238[2];
extern struct SharedFilePtr* data_ov077_02127230[2];

int _ZN6Lakitu16CleanupResourcesEv(void)
{
    s32 i;
    _ZN13SharedFilePtr7ReleaseEv(&data_ov077_02127b50);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov077_02127b48);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov077_02127b38);
    for (i = 0; i < 2; i++)
        _ZN13SharedFilePtr7ReleaseEv(data_ov077_02127238[i]);
    for (i = 0; i < 2; i++)
        _ZN13SharedFilePtr7ReleaseEv(data_ov077_02127230[i]);
    return 1;
}
