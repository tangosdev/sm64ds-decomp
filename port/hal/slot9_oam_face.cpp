// ONE FACE, IN ITS OWN FILE, AND THE FILE IS THE POINT.
// (run link100, lane RENDER9 -- _ZTV5Stage slot 9.)
//
// THE SEVEN-ARGUMENT OAM::Render HAS TWO MSVC NAMES IN THIS LINK, and they
// differ only in the return type, which MSVC encodes and the ARM does not:
//
//   ?Render@OAM@@SAX_NPAUOamAttr@@HHHHPAUMatrix2x2@@@Z   void
//       spelled by src/_ZN5Stage12RenderNumberEhiibi.cpp, which declares
//       `static void Render(bool, OamAttr *, int, int, int, int, Matrix2x2 *)`
//   ?Render@OAM@@SAH_NPAUOamAttr@@HHHHPAUMatrix2x2@@@Z   int
//       spelled by src/_ZN5Stage6RenderEv.cpp, which declares the same
//       parameter list returning int
//
// Both recoveries are faithful readings of the same ARM function: on the
// cartridge 0x020214a4 leaves something in r0 and its callers differ on whether
// they model that as a value. src/_ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2.c is
// the matched body and it is flat C returning void, so NEITHER decorated name
// is defined by anything in the tree.
//
// A TRANSLATION UNIT CANNOT DECLARE BOTH. They are the same member of the same
// class with the same parameters, so one file can carry one of them and no
// more -- C++ has no overload on return type. hal/stage_frame.cpp carries the
// int spelling, because that is the one Stage::Render itself uses and that file
// is slot 9's; this file carries the void spelling for Stage::RenderNumber. A
// second file rather than an /alternatename because the decorated names would
// then be hand-typed into a pragma, and a mis-typed alias is silent: it
// resolves nothing, the LHS stays unreferenced, and alternatename_guard.py
// reports "OK (unused)" for a row that was supposed to be load-bearing. A face
// that does not compile does not link either, and says so.
//
// THE FORWARD IS A CALL AND NOTHING ELSE. A `static` member is __cdecl in MSVC
// and so is the flat C body; the seven arguments are the same seven in the same
// order, with `bool` occupying the same four-byte stack slot the ARM's r0 word
// occupied. Nothing is dropped, nothing is invented, and no return value is
// manufactured -- this spelling has none.

struct OamAttr;
struct Matrix2x2;

class OAM {
public:
    static void Render(bool sub, OamAttr *data, int x, int y, int a, int b,
                       Matrix2x2 *mtx);
};

extern "C" void _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(int draw, void *obj,
                                                         int px, int py,
                                                         int pal, int prio,
                                                         void *mtx);

void OAM::Render(bool sub, OamAttr *data, int x, int y, int a, int b,
                 Matrix2x2 *mtx)
{
    _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(sub ? 1 : 0, (void *)data, x, y,
                                             a, b, (void *)mtx);
}
