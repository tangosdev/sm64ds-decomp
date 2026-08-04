//cpp
extern "C" {
struct BMD_File; struct BTA_File;
void func_ov006_020cecb4(int *p);
}
struct ModelBase { void SetFile(BMD_File*, int, int); };
struct Model : ModelBase { void SetPolygonID(int); };
struct TextureTransformer { static void Prepare(BMD_File&, BTA_File&); void SetFile(BTA_File&, int, int, unsigned int); };

struct data_t { int a; BMD_File* f; };
extern data_t data_ov006_02140850;
extern data_t data_ov006_02140858;
extern data_t data_ov006_02140848;
extern BTA_File data_ov006_0213b3a4;

extern "C" void func_ov006_020cecc0(char* c)
{
    ((Model*)(c+0xa0))->SetFile(data_ov006_02140850.f, 1, -1);
    ((Model*)(c+0xf0))->SetFile(data_ov006_02140858.f, 1, -1);
    ((Model*)(c+0x140))->SetFile(data_ov006_02140848.f, 1, -1);
    ((Model*)(c+0xa0))->SetPolygonID(1);
    ((Model*)(c+0xf0))->SetPolygonID(2);
    ((Model*)(c+0x140))->SetPolygonID(3);
    TextureTransformer::Prepare(*data_ov006_02140850.f, data_ov006_0213b3a4);
    ((TextureTransformer*)(c+0x194))->SetFile(data_ov006_0213b3a4, 0, 0x1000, 0);
    func_ov006_020cecb4((int*)c);
}
