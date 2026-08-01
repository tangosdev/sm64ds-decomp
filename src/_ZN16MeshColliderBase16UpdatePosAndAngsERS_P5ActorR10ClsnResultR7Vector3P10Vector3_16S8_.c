#include "types.h"
struct MeshColliderBase;
struct Actor;
struct ClsnResult;
struct Vector3;
struct Vector3_16;

extern void _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_(
    struct MeshColliderBase *clsn,
    struct Actor *clsnActor,
    struct ClsnResult *wmClsnResult,
    struct Vector3 *posToUpdate,
    struct Vector3_16 *motionAngToUpdate,
    struct Vector3_16 *angToUpdate);

extern void _ZN16MeshColliderBase25UpdateAngsWithAngularVelYERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_(
    struct MeshColliderBase *clsn,
    struct Actor *clsnActor,
    struct ClsnResult *wmClsnResult,
    struct Vector3 *posToUpdate,
    struct Vector3_16 *motionAngToUpdate,
    struct Vector3_16 *angToUpdate);

void _ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_(
    struct MeshColliderBase *clsn,
    struct Actor *clsnActor,
    struct ClsnResult *wmClsnResult,
    struct Vector3 *posToUpdate,
    struct Vector3_16 *motionAngToUpdate,
    struct Vector3_16 *angToUpdate)
{
    _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_(
        clsn, clsnActor, wmClsnResult, posToUpdate, motionAngToUpdate, angToUpdate);
    _ZN16MeshColliderBase25UpdateAngsWithAngularVelYERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_(
        clsn, clsnActor, wmClsnResult, posToUpdate, motionAngToUpdate, angToUpdate);
}
