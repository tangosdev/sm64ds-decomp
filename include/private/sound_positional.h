#ifndef PRIVATE_SOUND_POSITIONAL_H
#define PRIVATE_SOUND_POSITIONAL_H
#ifndef __cplusplus
#define func_02048528 _Z13func_02048528P12NNSSndHandlePK7Vector3
#define func_020485f0 _Z13func_020485f0P12NNSSndHandlePK7Vector3
#define func_02048720 _Z13func_02048720PK7Vector3ii
#define func_02048908 _Z13func_02048908P9SoundSlotPK7Vector3
#define func_02048a18 _Z13func_02048a18ii
#define func_02048a1c _Z13func_02048a1cPK7Vector3ii
#define func_02048af4 _Z13func_02048af4P12NNSSndHandlePK7Vector3i
#define func_02048c30 _Z13func_02048c30P12NNSSndHandlePK7Vector3i
#define func_02048d80 _Z13func_02048d80P12NNSSndHandlePK7Vector3
#define func_02048e54 _Z13func_02048e54v
#define func_02048e74 _Z13func_02048e74v
#define func_02048e94 _Z13func_02048e94v
#define func_02048ed4 _Z13func_02048ed4v
#define func_02048ef4 _Z13func_02048ef4v
#define func_02048f04 _Z13func_02048f04v
#define func_02048f14 _Z13func_02048f14v
#define func_02048f24 _Z13func_02048f24v
#define func_02048f34 _Z13func_02048f34Pv
#endif

/* The positional sound helpers of src/engine/sound/Sound2.cpp
 * (0x02048528..0x020490b0) and the voice slots they hand out. They work
 * out a positioned sound's volume and pan from its distance to the
 * listener. src/engine/sound/Sound.cpp is their only caller. A position
 * is the sound's camera-space Vector3 (dActor_c::mCamSpacePos for an
 * actor), and none of these writes through it.
 *
 * Parameter types follow each definition. Parameters stay unnamed: a name
 * here takes an @NNN in every C++ includer. */

#include "types.h"
#include "nnsys/snd/player.h"

/* One voice slot: the sound handle and its last listener distance. */
typedef struct SoundSlot {
    NNSSndHandle handle;
    int dist;
} SoundSlot;

#ifdef __cplusplus
#ifdef _MSC_VER
extern "C" {
#endif
#endif

/* Store the owner, clear the voice slots and select range mode 0. */
void func_02048f34(void *);

/* Select range modes 1, 2, 3, 4 and 6. */
void func_02048f24(void);
void func_02048f14(void);
void func_02048f04(void);
void func_02048ef4(void);
void func_02048ed4(void);

/* Set the slot-9 voice count back to four. */
void func_02048e94(void);
void func_02048e74(void);
void func_02048e54(void);

/* Set a voice's volume from its distance and its pan from the position's x. */
void func_02048d80(NNSSndHandle *, const Vector3 *);
void func_02048c30(NNSSndHandle *, const Vector3 *, int);
void func_02048af4(NNSSndHandle *, const Vector3 *, int);
void func_02048908(SoundSlot *, const Vector3 *);
void func_020485f0(NNSSndHandle *, const Vector3 *);
void func_02048528(NNSSndHandle *, const Vector3 *);

/* Is the sound (kind, id) inside its audible range at this position? */
int func_02048a1c(const Vector3 *, int, int);

/* Returns its first argument. */
int func_02048a18(int, int);

/* Pick a free (or the weakest) voice slot for the sound (kind, id), or 0. */
SoundSlot *func_02048720(const Vector3 *, int, int);

#ifdef __cplusplus
#ifdef _MSC_VER
}
#endif
#endif

#endif
