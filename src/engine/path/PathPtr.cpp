//cpp
/* PathPtr -- a read cursor over one entry of the level's path table.
 *
 * FromID binds the handle to the id'th PathDef; the accessors read through
 * def, and GetNode walks the shared flat table of 6-byte (three s16) node
 * records that def->firstNode indexes into. The table helpers are the
 * class's own plumbing: LoadPathObjects hands it the definition table and
 * LoadPathNodeObjects the node table (src/game/stages/LevelObjects.cpp).
 *
 * mwccarm emits .text in reverse source order, so the definitions below run
 * ROM-descending; the roster above reads ROM-ascending.
 *
 * leftovers:
 * - GetUnk004/unk_004 keep the decompiler name: the sole consumer
 *   (UpdateParallelTrackingCamera) compares it to 0xff and shifts it << 12 as a cap, but
 *   what the PathDef byte means is unrecovered; renaming the method would
 *   rename the mangled symbol tree-wide.
 * - func_0203ad84 is unrelated to paths: it reads the ROM file
 *   data_020994d4 names ("/BUILDTIME", here "2005-01-13 08:50:39
 *   Administrator") into gBuildTimeString, at most 0x1f chars plus a forced
 *   NUL, via func_02018434. Both globals keep their linker names. */
#include "PathPtr.h"
#include "common.h"

extern PathDef *PATH_DEFS;  /* the level's PathDef table */
extern Vector3_16 *PATH_NODES; /* flat node positions, three s16 each */
extern int NUM_PATHS;       /* entries in PATH_DEFS */

extern "C" {
void func_02018434(const char *path, int dst, int size);
extern char data_020994d4[];
extern char gBuildTimeString[];
}

void func_0203ad84(void) {
    func_02018434(data_020994d4, (int)gBuildTimeString, 31);
    gBuildTimeString[31] = 0;
}

PathPtr::PathPtr() : def(0), unk_004(0) {}

void PathPtr::SetDef(PathDef *d)
{
    def = d;
}

unsigned int PathPtr::NumNodes() const
{
    return def->numNodes;
}

unsigned int PathPtr::GetUnk004() const
{
    return def->unk_004;
}

unsigned int PathPtr::GetPullFactor() const
{
    return def->flags & 0x7f;
}

unsigned int PathPtr::Loops() const
{
    return def->flags & 0x80;
}

void PathPtr::GetNode(Vector3 &node, unsigned int idx) const
{
    Vector3_16 *path = GetNodeTable() + def->firstNode;
    node.x = path[idx].x << 12;
    node.y = path[idx].y << 12;
    node.z = path[idx].z << 12;
}

void PathPtr::SetNodeTable(void *nodes)
{
    PATH_NODES = (Vector3_16 *)nodes;
}

Vector3_16 *PathPtr::GetNodeTable() {
    return PATH_NODES;
}

void PathPtr::SetPathTable(PathDef *defs, int numPaths)
{
    PATH_DEFS = defs;
    NUM_PATHS = numPaths;
}

void PathPtr::FromID(unsigned int id)
{
    SetDef(PATH_DEFS + id);
}
