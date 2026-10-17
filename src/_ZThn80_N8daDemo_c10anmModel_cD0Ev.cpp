//cpp
// @symbol _ZThn80_N8daDemo_c10anmModel_cD0Ev
/* Compiler-owned deleting-destructor adjustment thunk. The source describes
   the real destructor; mwccarm emits the -0x50 thunk named by this file as a
   byproduct of the dExtFrameCtrl_c base at +0x50 inside dExtAnmModel_c
   (daDemo_c.h). */
#include "daDemo_c.h"
#include "SharedFilePtr.h"
#include "TextureSequence.h"

daDemo_c::anmModel_c::~anmModel_c()
{
    SharedFilePtr *file;
    int i;

    file = mModelFile;
    if (file != 0)
        file->Release();
    for (i = 0; i < mNumAnimFiles; i++) {
        file = mAnimFiles[i];
        if (file != 0)
            file->Release();
    }
    if (mTexSeq != 0) {
        for (i = 0; i < mNumTexSeqFiles; i++) {
            file = mTexSeqFiles[i];
            if (file != 0)
                file->Release();
        }
        delete mTexSeq;
    }
}
