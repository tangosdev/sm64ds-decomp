struct ModelComponents {
    void *modelFile;
    void *materials;
    void *bones;
    void *transforms;
    void *unk10;
};

struct Material { char pad[0x24]; unsigned int flags; char pad2[0x30 - 0x24 - 4]; };
struct ModelFile { char pad[0x24]; int materialCount; };

extern struct ModelComponents *func_02016e70(void *file);
extern void _ZN15ModelComponents21UpdateVertsUsingBonesEv(struct ModelComponents *data);

int _ZN11ShadowModel9DoSetFileEPcii(char *self, void *file, int flag)
{
    *(struct ModelComponents **)(self + 8) = func_02016e70(file);

    if (*(struct ModelComponents **)(self + 8) == 0)
        return 0;

    _ZN15ModelComponents21UpdateVertsUsingBonesEv(*(struct ModelComponents **)(self + 8));

    if (flag != 0) {
        struct ModelComponents *data = *(struct ModelComponents **)(self + 8);
        struct ModelFile *mf = (struct ModelFile *)data->modelFile;
        unsigned int n = mf->materialCount;
        struct Material *mat = (struct Material *)data->materials;

        for (unsigned int i = 0; i < n; i++) {
            *(unsigned int *)(((long long)(int)&mat->flags) & 0xFFFFFFFFFFFFFFFFLL) |= 0x8000;
            mat = (struct Material *)((char *)mat + 0x30);
        }
    }

    return 1;
}
