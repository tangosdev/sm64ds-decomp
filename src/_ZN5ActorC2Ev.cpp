//cpp
#include "types.h"
// @symbol _ZN5ActorC2Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Actor.h"
extern "C" {
extern int data_0208e4b8;
extern int data_0208e3a4;
extern int data_0209b468;
extern s16* data_0209b460;
extern s16* data_0209b45c;
extern s16 data_0208e378;
extern int* data_020a4bb8;
extern unsigned char data_0209f2d8;
void _ZN9ActorBaseC1Ev(void* self);
int func_0203b244(void* l, void* n);
void _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(void* self, int a, int b, int c, int d);
}

extern "C" void* _ZN5ActorC2Ev(struct Actor *self) {
    int* entry;
    int b;
    int r3;
    _ZN9ActorBaseC1Ev(((char*)self));
    *(void**)((char*)self) = &data_0208e4b8;
    *(void**)((char*)self) = &data_0208e3a4;
    self->unk_050 = 0;
    self->unk_054 = 0;
    *(char**)((char*)&self->unk_058) = ((char*)self);
    func_0203b244((void*)&data_0209b468, ((char*)self) + 0x50);
    {
        s16* p = data_0209b460;
        if (p != 0) {
            self->mPosX = ((int*)p)[0];
            self->mPosY = ((int*)p)[1];
            self->mPosZ = ((int*)p)[2];
        }
    }
    {
        s16* q = data_0209b45c;
        if (q != 0) {
            self->mAngleX = q[0];
            self->mAngleY = q[1];
            self->mAngleZ = q[2];
            {
                s16* q2 = data_0209b45c;
                self->mPrevAngleX = q2[0];
                self->mPrevAngleY = q2[1];
                self->mPrevAngleZ = q2[2];
            }
        }
    }
    self->mAreaId = data_0209b44c;
    self->unk_0ce = data_0208e378;
    entry = (int*)(((int**)data_020a4bb8)[self->actorID]);
    self->mFlags = entry[2];
    b = (data_0209f2d8 == 2);
    if (b != 0)
        r3 = entry[5] + 0x7d0000;
    else
        r3 = entry[5];
    _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(((char*)self), entry[3], entry[4], r3, entry[6]);
    return ((char*)self);
}
