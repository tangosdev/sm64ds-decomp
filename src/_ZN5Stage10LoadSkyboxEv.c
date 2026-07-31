#include "types.h"
typedef struct {
    char data[0x50];
} Model;

typedef struct {
    char pad[0x9bc];
    Model* skyboxModel;
} Stage;

extern u32 _ZN5Stage11GetSkyboxIDEv(Stage* self);
extern void* _Znwj(u32 size);
extern void* _ZN5ModelC1Ev(void* this);
extern void _ZN5Model14LoadAndSetFileEtii(void* this, u16 fileID, s32 a, s32 b);

extern u16 gSkyboxFileTable[];

void _ZN5Stage10LoadSkyboxEv(Stage* self) {
    u32 skyboxID = _ZN5Stage11GetSkyboxIDEv(self);
    if (!skyboxID) return;
    Model* model = (Model*)_Znwj(0x50);
    if (model != 0) {
        model = (Model*)_ZN5ModelC1Ev(model);
    }
    self->skyboxModel = model;
    u16 fileID = gSkyboxFileTable[skyboxID - 1];
    _ZN5Model14LoadAndSetFileEtii(self->skyboxModel, fileID, 0, 2);
}