//cpp
struct SharedFilePtr;
struct BMD_File;
struct BTA_File;
struct BCA_File;

extern struct BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(struct SharedFilePtr &);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thisp, struct BMD_File *, int, int);
extern void _ZN9Animation8LoadFileER13SharedFilePtr(struct SharedFilePtr &);
extern void _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(struct BMD_File &, struct BTA_File &);
extern void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void *thisp, struct BTA_File &, int, int, unsigned int);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thisp, struct BCA_File *, int, int, unsigned int);
extern "C" void func_ov026_02111ee0(void *c, void *p);

struct FilePtr4 { int a; void *file; };
extern struct FilePtr4 data_ov026_02113f0c;
extern struct FilePtr4 data_ov026_02113f04;
extern struct BTA_File data_ov026_02112f40;
extern int data_ov026_02113f2c;

extern "C" int func_ov026_021120ec(char *self) {
    struct BMD_File *bmd = _ZN5Model8LoadFileER13SharedFilePtr(*(struct SharedFilePtr *)&data_ov026_02113f0c);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(self + 0x114, bmd, 1, -1);

    _ZN9Animation8LoadFileER13SharedFilePtr(*(struct SharedFilePtr *)&data_ov026_02113f04);

    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(*(struct BMD_File *)data_ov026_02113f0c.file, data_ov026_02112f40);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(self + 0x178, data_ov026_02112f40, 0, 0x1000, 0);

    *(int *)(self + 0x184) = 0x1000;
    *(int *)(self + 0x170) = 0x1000;

    *(int *)(self + 0x1a8) = *(int *)(self + 0x5c);
    *(int *)(self + 0x1ac) = *(int *)(self + 0x60);
    *(int *)(self + 0x1b0) = *(int *)(self + 0x64);
    *(int *)(((int)self + 0x1ac) & 0xFFFFFFFFFFFFFFFF) -= 0x64000;

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x114, (struct BCA_File *)data_ov026_02113f04.file, 0, 0x1000, 0);

    func_ov026_02111ee0(self, &data_ov026_02113f2c);
    return 1;
}
