//cpp
struct BMD_File;
struct BTP_File;
struct TextureSequence { static void Prepare(BMD_File &model, BTP_File &animFile); };
extern "C" {
struct SharedFilePtr { int a, file; };
void* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr& f);
int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* f, int a, int b);
void* _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr& f);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* f, int a, int fx, unsigned int e);
void* _Znwj(unsigned int sz);
void* _ZN15TextureSequenceC1Ev(void* self);
void* _ZN15TextureSequence8LoadFileER13SharedFilePtr(SharedFilePtr& f);
void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void* self, void* btp, int a, int fx, unsigned int e);

int func_ov002_020f6618(char* self, SharedFilePtr* mdl, int nAnims, SharedFilePtr** anims,
                        int arg5, unsigned char texByte, SharedFilePtr** texs, int tsData)
{
    int i;
    void* ts;
    *(SharedFilePtr**)(self + 0x70) = mdl;
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(self,
            _ZN5Model8LoadFileER13SharedFilePtr(**(SharedFilePtr**)(self + 0x70)), 1, tsData) == 0)
        return 0;
    self[0x80] = (char)nAnims;
    *(SharedFilePtr***)(self + 0x74) = anims;
    for (i = 0; i < *(unsigned char*)(self + 0x80); i++) {
        _ZN9Animation8LoadFileER13SharedFilePtr(*(*(SharedFilePtr***)(self + 0x74))[i]);
    }
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self,
        (void*)(*(*(SharedFilePtr***)(self + 0x74))[0]).file, 0, 0x1000, 0);
    if (texByte) {
        ts = _Znwj(0x14);
        if (ts) ts = _ZN15TextureSequenceC1Ev(ts);
        *(void**)(self + 0x7c) = ts;
        if (*(void**)(self + 0x7c) == 0)
            return 0;
        self[0x81] = texByte;
        *(SharedFilePtr***)(self + 0x78) = texs;
        for (i = 0; i < *(unsigned char*)(self + 0x81); i++) {
            void* bmd = (void*)(*(SharedFilePtr**)(self + 0x70))->file;
            void* btp = _ZN15TextureSequence8LoadFileER13SharedFilePtr(*(*(SharedFilePtr***)(self + 0x78))[i]);
            TextureSequence::Prepare(*(BMD_File*)bmd, *(BTP_File*)btp);
        }
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(*(void**)(self + 0x7c),
            (void*)(*(*(SharedFilePtr***)(self + 0x78))[0]).file, 0, 0x1000, 0);
    }
    self[0x83] = (char)arg5;
    return 1;
}
}
