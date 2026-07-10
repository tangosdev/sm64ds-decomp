extern void CleanCommonModelDataArr(void);
extern int _ZN13SharedFilePtr7ReleaseEv(void*);
extern int func_ov075_0211b3b8(void* c);
extern char data_ov075_0211d404[];
extern char data_ov075_0211d3c4[];
extern char data_ov075_0211d414[];
extern char data_ov075_0211d394[];
extern char data_ov075_0211d3cc[];
extern char data_ov075_0211d39c[];
extern char data_ov075_0211d3d4[];
extern char data_ov075_0211d3a4[];
extern char data_ov075_0211d3ec[];
extern char data_ov075_0211d384[];
extern char data_ov075_0211d424[];
extern char data_ov075_0211d42c[];
extern char data_ov075_0211d41c[];
extern char data_ov075_0211d3ac[];
extern char data_ov075_0211d3b4[];
extern char data_ov075_0211d3f4[];
extern char data_ov075_0211d38c[];
extern char data_ov075_0211d3dc[];
extern char data_ov075_0211d40c[];
extern char data_ov075_0211d3fc[];
extern char data_ov075_0211d3bc[];
extern char data_ov075_0211d3e4[];

int _ZN14UnknownVsEntry16CleanupResourcesEv(void* c)
{
    CleanCommonModelDataArr();
    _ZN13SharedFilePtr7ReleaseEv(data_ov075_0211d404);
    _ZN13SharedFilePtr7ReleaseEv(data_ov075_0211d3c4);
    _ZN13SharedFilePtr7ReleaseEv(data_ov075_0211d414);
    if (*(int*)((char*)c+8) != 1) {
        _ZN13SharedFilePtr7ReleaseEv(data_ov075_0211d394);
        _ZN13SharedFilePtr7ReleaseEv(data_ov075_0211d3cc);
        _ZN13SharedFilePtr7ReleaseEv(data_ov075_0211d39c);
        _ZN13SharedFilePtr7ReleaseEv(data_ov075_0211d3d4);
        _ZN13SharedFilePtr7ReleaseEv(data_ov075_0211d3a4);
        _ZN13SharedFilePtr7ReleaseEv(data_ov075_0211d3ec);
        _ZN13SharedFilePtr7ReleaseEv(data_ov075_0211d384);
        _ZN13SharedFilePtr7ReleaseEv(data_ov075_0211d424);
        _ZN13SharedFilePtr7ReleaseEv(data_ov075_0211d42c);
        _ZN13SharedFilePtr7ReleaseEv(data_ov075_0211d41c);
    } else {
        _ZN13SharedFilePtr7ReleaseEv(data_ov075_0211d3ac);
        _ZN13SharedFilePtr7ReleaseEv(data_ov075_0211d3b4);
        _ZN13SharedFilePtr7ReleaseEv(data_ov075_0211d3f4);
        _ZN13SharedFilePtr7ReleaseEv(data_ov075_0211d38c);
        _ZN13SharedFilePtr7ReleaseEv(data_ov075_0211d3dc);
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov075_0211d40c);
    _ZN13SharedFilePtr7ReleaseEv(data_ov075_0211d3fc);
    if (*(int*)((char*)c+8) != 1) {
        _ZN13SharedFilePtr7ReleaseEv(data_ov075_0211d3bc);
        _ZN13SharedFilePtr7ReleaseEv(data_ov075_0211d3e4);
    }
    func_ov075_0211b3b8((char*)c+0xe80);
    return 1;
}
