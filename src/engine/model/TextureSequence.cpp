//cpp
/* TextureSequence -- Animation child driving BTP-file playback. Cartridge
   class: _ZTV15TextureSequence at 0x0208e7d4 (two slots, the destructor
   pair); the cartridge RTTI names it dExtAnmTexPat_c. TU claims
   0x0201597c..0x02015a7c, the whole class run in delinks order. Written
   back-to-front: mwccarm emits .text in reverse source order under default
   deferred codegen.

   leftovers:
   - SetFile and its SetAnimation call keep mangled spellings (Fix12<int>
     by value, wall 6az); the member declarations in the headers are real.
   - func_02046d50 and func_02046bbc stay extern "C" helpers with no
     declaring header; Prepare's ROM body is a 0xc long-call veneer, so it
     calls func_02046d50 with that function's own two-argument signature.
   - file->unk_00 is the frame count handed to SetAnimation (the sibling
     BTA names it numFrames); BTP_File names only the words UpdateFileOffsets
     touches.
   - RTTI stays off: the cartridge names the class dExtAnmTexPat_c, so the
     emitted _ZTS/_ZTI records would have no configured home. */
#include "TextureSequence.h"

/* The cartridge spells this class dExtAnmTexPat_c, so the compiler-emitted
   _ZTS15TextureSequence / _ZTI15TextureSequence / _ZTS9Animation /
   _ZTI9Animation records would be homeless under either spelling. Compiling
   with RTTI off emits no records at all; the vtable preamble's typeinfo word
   deadstrips with the rest of the data sections. */
#pragma RTTI off

extern "C" void func_02046d50(BMD_File *model, BTP_File *file);
extern "C" void func_02046bbc(ModelComponents *model, BTP_File *file, int frame);
/* SetFile below and its SetAnimation call keep their mangled spellings:
   wall 6az (notes/mwccarm-codegen.md) homes class-typed by-value
   parameters that a body reads, and the real signature carries
   Fix12<int> -- passing one to the member declaration homes it to the
   caller's stack. The declarations in TextureSequence.h and dExtFrameCtrl_c.h
   are the real ones. */
extern "C" void _ZN15dExtFrameCtrl_c12SetAnimationEti5Fix12IiEt(dExtFrameCtrl_c *self,
                                                                u16 numFrames, s32 flags, s32 speed,
                                                                u16 startFrame);

// @symbol _ZN15TextureSequenceC1Ev
TextureSequence::TextureSequence() {
    file = 0;
}

// @symbol _ZN15TextureSequenceD1Ev
TextureSequence::~TextureSequence() {}

// @symbol _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj
extern "C" void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(TextureSequence *self,
                                                                   BTP_File *file, s32 flags,
                                                                   s32 speed, u16 startFrame) {
    if (file == self->file) {
        self->SetFlags(flags);
        self->speed = speed;
    } else {
        self->file = file;
        _ZN15dExtFrameCtrl_c12SetAnimationEti5Fix12IiEt(self, file->unk_00, flags, speed,
                                                        startFrame);
    }
}

// @symbol _ZN15TextureSequence6UpdateER15ModelComponents
void TextureSequence::Update(ModelComponents &model) {
    func_02046bbc(&model, file, (u16)((u32)currFrame >> 12));
}

/* The ROM body is a 0xc long-call veneer (ldr ip, [pc]; bx ip); the
   registers pass through untouched, so the matched func_02046d50.c's
   own two-argument signature is the call surface. Prepare is static:
   no this. */
// @symbol _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File
void TextureSequence::Prepare(BMD_File &model, BTP_File &animFile) {
    func_02046d50(&model, &animFile);
}
