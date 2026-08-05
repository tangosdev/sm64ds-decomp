//cpp
// @symbol _ZN9Submarine13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Submarine.h"
struct SharedFilePtr;
struct BMD_File;
struct BTA_File;
struct BCA_File;

extern "C" {
extern struct BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(struct SharedFilePtr &);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thisp, struct BMD_File *, int, int);
extern void _ZN9Animation8LoadFileER13SharedFilePtr(struct SharedFilePtr &);
extern void _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(struct BMD_File &, struct BTA_File &);
extern void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void *thisp, struct BTA_File &, int, int, unsigned int);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thisp, struct BCA_File *, int, int, unsigned int);
}

struct FilePtr4 { int a; void *file; };
extern struct FilePtr4 data_ov026_02113f0c;
extern struct FilePtr4 data_ov026_02113f04;
extern struct BTA_File data_ov026_02112f40;

int Submarine::InitResources()
{
    struct BMD_File *bmd = _ZN5Model8LoadFileER13SharedFilePtr(*(struct SharedFilePtr *)&data_ov026_02113f0c);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0x114, bmd, 1, -1);

    _ZN9Animation8LoadFileER13SharedFilePtr(*(struct SharedFilePtr *)&data_ov026_02113f04);

    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(*(struct BMD_File *)data_ov026_02113f0c.file, data_ov026_02112f40);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(((char *)this) + 0x178, data_ov026_02112f40, 0, 0x1000, 0);

    unk_184 = 0x1000;
    unk_170 = 0x1000;

    unk_1a8 = mPosX;
    unk_1ac = mPosY;
    unk_1b0 = mPosZ;
    *(int *)(((int)((char *)this) + 0x1ac)) -= 0x64000;

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(((char *)this) + 0x114, (struct BCA_File *)data_ov026_02113f04.file, 0, 0x1000, 0);

    func_ov026_02111ee0(((char *)this), &data_ov026_02113f2c);
    return 1;
}
