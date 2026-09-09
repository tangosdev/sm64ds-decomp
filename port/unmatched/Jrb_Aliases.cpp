/* GATE 188 link aliases for Jolly Roger Bay's movers.
 *
 * Three bridges the recovered ov016 src needs:
 *
 * 1. func_ov018_02111bf0 -- RETIRED, run link100 lane SMALLS. It was a
 *    cross-overlay ALIAS TYPO in the recovered src: the three UNAGI state
 *    bodies func_ov016_02111534/02111758/021118b4 call "func_ov018_02111bf0"
 *    where their own module's 0x02111bf0 is meant (ov016 and ov018 share load
 *    base 0x021111a0), and this file bridged the ov018 spelling onto
 *    func_ov016_02111bf0 with
 *        /alternatename:_func_ov018_02111bf0=_func_ov016_02111bf0
 *
 *    ov018's OWN 0x02111bf0 is a real, different function -- MOTHER_PENGUIN's
 *    floor/wall collision pass, src/func_ov018_02111bf0.c, called by
 *    func_ov018_02112234 and func_ov018_02111e28 with overlay(18) relocations
 *    at both sites. While the alias stood, DEFINING that body defeated it and
 *    silently rerouted all three Unagi bodies into the penguin's collision
 *    pass; alternatename_guard.py failed lane MPG's first link on exactly that
 *    (LHS 0001:00188350 against RHS 0001:00148d00), and MPG shipped nine of
 *    the penguin's ten ov018 TUs with the tenth host-copied under a port name
 *    rather than edit this file.
 *
 *    THE FIX IS THE GUARD'S OWN R1/R2 RECIPE (alternatename_guard.py's header,
 *    wave 5): delete the dead alternatename and recompile the TUs that
 *    referenced the LHS with a per-source -DLHS=RHS. The three ov016 renames
 *    are in port/CMakeLists.txt's gate-smalls block beside the slice, so the
 *    three Unagi bodies still reach func_ov016_02111bf0 and the ov018 name is
 *    free for the ov018 body that owns it. hal/unmatched/MotherPenguin_ClsnPass
 *    .cpp, the host copy that stood in for it, is retired with the same commit.
 *
 * 2. func_020b5e58 -- func_ov016_02112fa8 (id 60's daObjKi_Ita_c InitResources)
 *    calls it by the bare "func_020b5e58" C name, but the matched body is
 *    func_ov002_020b5e58 (ov002, in slice_gate188.txt). Bridge the bare name.
 *
 * 3. MeshColliderBase::UpdatePosWithTransform (the void* DATA form) -- ShipUp's
 *    InitResources declares it `extern void* _ZN16...S8_;` and takes its address
 *    as the BeforeClsn callback, which MSVC mangles as the data symbol
 *    ?_ZN16...S8_@@3PAXA. The real static method is
 *    ?UpdatePosWithTransform@MeshColliderBase@@SAX...@Z (hosted, gate 59). Alias
 *    the void* data spelling onto it. (FloatOnWater/id 313 declares it as a
 *    `void(void)` function whose &-address already resolves through the existing
 *    cxx_aliases.cpp bridge, so only ShipUp's void* form needs this.)
 */
#include <cstddef>

extern "C" {
int func_ov002_020b5e58(char *self, char *fp);
}

/* #1 is gone: the three ov016 callers name func_ov016_02111bf0 directly now,
   through the per-source renames in the gate-smalls CMake block. */
/* #2: the bare func_020b5e58 -> the ov002 matched body. Both C linkage. */
#pragma comment(linker, "/alternatename:_func_020b5e58=_func_ov002_020b5e58")
/* #3: ShipUp's void* data reference to UpdatePosWithTransform -> the real
   static method (the gate-59 host body). */
#pragma comment(linker, "/alternatename:?_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_@@3PAXA=?UpdatePosWithTransform@MeshColliderBase@@SAXAAU1@PAUActor@@AAUClsnResult@@AAUVector3@@PAUVector3_16@@4@Z")
/* #4: func_ov002_020b5e58 (id 60's daObjKi_Ita_c init helper) takes the address
   of UpdatePosAndAngs declared `extern char _ZN16...S8_;` -> MSVC mangles that
   data spelling as ?_ZN16...S8_@@3DA (D = char); bridge it to the real static
   method the same way. */
#pragma comment(linker, "/alternatename:?_ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_@@3DA=?UpdatePosAndAngs@MeshColliderBase@@SAXAAU1@PAUActor@@AAUClsnResult@@AAUVector3@@PAUVector3_16@@4@Z")
