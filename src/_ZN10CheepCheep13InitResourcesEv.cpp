//cpp
struct SharedFilePtr;
struct BMD_File;
struct BCA_File;
struct Actor;
struct Vector3 { int x, y, z; };
struct Vector3_16;

extern struct BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(struct SharedFilePtr &);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thisp, struct BMD_File *, int, int);
extern void _ZN9Animation8LoadFileER13SharedFilePtr(struct SharedFilePtr &);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
    void *thisp, struct Actor *, struct Vector3 const &, int, int, unsigned int, unsigned int);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(
    void *thisp, struct Actor *, int, int, struct Vector3_16 *, int);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    void *thisp, struct BCA_File *, int, int, unsigned int);
extern "C" void func_ov090_021332e8(void *c, void *p);

extern struct SharedFilePtr data_ov090_02134564;
struct AnimFilePtr { int a; struct BCA_File *file; };
extern struct AnimFilePtr data_ov090_0213455c;
extern struct Vector3 data_ov090_021342d8;
extern int data_ov090_02134594;

extern "C" int _ZN10CheepCheep13InitResourcesEv(char *self) {
    struct BMD_File *bmd;
    struct Vector3 v;

    bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov090_02134564);
    _ZN9ModelBase7SetFileEP8BMD_Fileii((void *)(self + 0x30c), bmd, 1, -1);

    _ZN9Animation8LoadFileER13SharedFilePtr(*(struct SharedFilePtr *)&data_ov090_0213455c);

    v = data_ov090_021342d8;
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
        (void *)(self + 0x110), (struct Actor *)self, v, 0x32000, 0x3c000, 0x200004, 0x8000);

    *(int *)(self + 0x374) = *(int *)(self + 0x5c);
    *(int *)(self + 0x378) = *(int *)(self + 0x60);
    *(int *)(self + 0x37c) = *(int *)(self + 0x64);
    *(short *)(self + 0x8e) = *(short *)(self + 0x94);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(
        (void *)(self + 0x150), (struct Actor *)self, 0x1e000, 0x1e000, 0, 0);

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        (void *)(self + 0x30c), data_ov090_0213455c.file, 0, 0x1000, 0);

    func_ov090_021332e8(self, &data_ov090_02134594);
    return 1;
}
