typedef unsigned int u32;
typedef int s32;
typedef short s16;
typedef signed char s8;
struct Vector3 { int x, y, z; };
struct Vector3_16 { short x, y, z; };
typedef struct ActorBase ActorBase;
extern ActorBase *func_02010e78(const struct Vector3 *pos, const struct Vector3_16 *rot, s8 areaID, s16 deathTableID);
extern ActorBase *_ZN12ActorDerived5SpawnEjP9ActorBaseii(u32 actorID, ActorBase *parent, u32 param1, s32 flags);
extern ActorBase *data_0209f5c0; /* 0x0209f5c0 */
ActorBase *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
    u32 actorID, u32 param1,
    const struct Vector3 *pos, const struct Vector3_16 *rot,
    s8 areaID, s16 deathTableID)
{
    func_02010e78(pos, rot, areaID, deathTableID);
    return _ZN12ActorDerived5SpawnEjP9ActorBaseii(actorID, data_0209f5c0, param1, 2);
}
