//cpp
typedef int Fix12;

struct SharedFilePtr { int id; void* file; };

extern "C" {
int _ZN5Actor9TrackStarEjj(void* self, unsigned int a, unsigned int b);
void LoadSilverStarAndNumber(void);
void* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr* f);
void* _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr* f);
void _ZN8CapEnemy6AddCapEj(void* self, unsigned int x);
int _ZN8CapEnemy21DestroyIfCapNotNeededEv(void* self);
int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* f, int a, int b);
int _ZN11ShadowModel12InitCylinderEv(void* self);
void _ZN15MaterialChanger7PrepareER8BMD_FileR8BMA_File(void* bmd, void* bma);
void _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(void* self, void* bma, int a, Fix12 b, unsigned int cc);
void LoadBlueCoinModel(void* c);
void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* self, void* a, Fix12 r, Fix12 h, unsigned int e, unsigned int g);
void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void* self, void* a, Fix12 b, Fix12 cc, void* d, Fix12 e);
void _ZN12WithMeshClsn19StartDetectingWaterEv(void* self);
void func_ov084_021290d4(void* c);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* f, int a, Fix12 b, unsigned int cc);

extern SharedFilePtr data_ov084_02130cf8;
extern SharedFilePtr data_ov084_02130ce8;
extern char data_ov084_0213089c;
extern SharedFilePtr* data_ov084_02130278[7];
extern int data_ov084_02130258[];
extern int data_ov084_02130208[];
extern int data_ov084_02130228[];
extern int data_ov084_02130238[];
}

extern "C" int func_ov084_0212bc30(char* c)
{
    int i;

    *(unsigned char*)(c + 0x464) = (*(unsigned int*)(c + 8) >> 4) & 0xf;
    *(signed char*)(c + 0x465) = -1;
    *(unsigned char*)(c + 0x112) = (*(unsigned int*)(c + 8) >> 8) & 0xf;
    *(unsigned char*)(c + 0x466) = (*(unsigned int*)(c + 8) >> 0xc) & 0xf;

    if (*(unsigned char*)(c + 0x464) == 1)
    {
        *(unsigned char*)(c + 0x465) = _ZN5Actor9TrackStarEjj(c, *(unsigned char*)(c + 0x466), 1);
        LoadSilverStarAndNumber();
    }
    else if (*(unsigned char*)(c + 0x464) == 2)
    {
        LoadSilverStarAndNumber();
    }

    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov084_02130cf8);
    for (i = 0; i < 7; i++)
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov084_02130278[i]);

    _ZN8CapEnemy6AddCapEj(c, (unsigned char)(*(int*)(c + 8) & 0xf));

    if ((*(unsigned char*)(c + 0x113) & 0xf) < 6)
        *(int*)(c + 8) = *(int*)(c + 8) & 0xf0ff;

    if (_ZN8CapEnemy21DestroyIfCapNotNeededEv(c) == 0)
        return 0;

    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x370, data_ov084_02130cf8.file, 1, -1) == 0)
        return 0;

    if (_ZN11ShadowModel12InitCylinderEv(c + 0x3d4) == 0)
        return 0;

    _ZN15MaterialChanger7PrepareER8BMD_FileR8BMA_File(data_ov084_02130cf8.file, &data_ov084_0213089c);
    _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(c + 0x3fc, &data_ov084_0213089c, 0x40000000, 0x1000, 0);

    *(unsigned char*)(c + 0x108) = 1;

    {
        int id = *(unsigned short*)(c + 0xc);
        int cond = (id == 0xc9);
        if (cond != false)
        {
            *(int*)(c + 0x460) = 0;
        }
        else
        {
            cond = (id == 0xc8);
            if (cond != false)
            {
                if (*(int*)(c + 8) == 0xeeee || *(int*)(c + 8) == 0xeeef)
                {
                    *(int*)(c + 0x460) = 3;
                    *(int*)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFF) &= ~2;
                    if (*(int*)(c + 8) == 0xeeee)
                        *(unsigned char*)(c + 0x108) = 0;
                }
                else
                {
                    *(int*)(c + 0x460) = 1;
                }
            }
            else
            {
                *(int*)(c + 0x460) = 2;
                LoadBlueCoinModel(c);
            }
        }
    }

    {
        int scale = data_ov084_02130258[*(int*)(c + 0x460)];
        *(int*)(c + 0x80) = scale;
        *(int*)(c + 0x84) = scale;
        *(int*)(c + 0x88) = scale;
    }
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x180, c, *(int*)(c + 0x80) * 0x3c, data_ov084_02130208[*(int*)(c + 0x460)], 0x200000, 0xa6efe0);

    if (*(int*)(c + 0x460) == 2)
        *(int*)(((int)c + 0x19c) & 0xFFFFFFFFFFFFFFFF) &= ~0x8000;

    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x1b4, c, *(int*)(c + 0x80) * 0x3c, *(int*)(c + 0x80) * 0x3c, 0, 0);
    _ZN12WithMeshClsn19StartDetectingWaterEv(c + 0x1b4);

    *(unsigned char*)(c + 0x468) = 0;
    *(int*)(c + 0x434) = 0;
    *(int*)(c + 0x10c) = 0;
    *(int*)(c + 0x438) = 0;
    *(int*)(c + 0x440) = 0x7fffffff;
    *(short*)(c + 0x45a) = *(short*)(c + 0x94);
    *(int*)(c + 0x444) = data_ov084_02130228[*(int*)(c + 0x460)];
    *(short*)(c + 0x450) = 0;
    *(int*)(c + 0x43c) = 0;
    *(short*)(c + 0x454) = 0;
    *(short*)(c + 0x456) = 0;
    *(int*)(c + 0x428) = *(int*)(c + 0x5c);
    *(int*)(c + 0x42c) = *(int*)(c + 0x60);
    *(int*)(c + 0x430) = *(int*)(c + 0x64);
    *(short*)(c + 0x458) = 0;
    func_ov084_021290d4(c);

    *(int*)(c + 0x41c) = *(int*)(c + 0x5c);
    *(int*)(c + 0x420) = *(int*)(c + 0x60);
    *(int*)(c + 0x424) = *(int*)(c + 0x64);
    *(int*)(c + 0x9c) = data_ov084_02130238[*(int*)(c + 0x460)];
    *(int*)(c + 0xa0) = -0x32000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x370, data_ov084_02130ce8.file, 0, 0x1000, 0);

    *(unsigned char*)(c + 0x467) = 0;
    *(int*)(c + 0x44c) = *(int*)(c + 8);
    return 1;
}
