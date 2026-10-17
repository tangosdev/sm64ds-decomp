//cpp
// @symbol func_0203ad84
// @symbol _ZN7PathPtr6FromIDEj
// @symbol _ZN7PathPtr12SetPathTableEP7PathDefi
// @symbol _ZN7PathPtr12GetNodeTableEv
// @symbol _ZN7PathPtr12SetNodeTableEPv
// @symbol _ZNK7PathPtr7GetNodeER7Vector3j
// @symbol _ZNK7PathPtr5LoopsEv
// @symbol _ZNK7PathPtr13GetPullFactorEv
// @symbol _ZNK7PathPtr9GetUnk004Ev
// @symbol _ZNK7PathPtr8NumNodesEv
// @symbol _ZN7PathPtr6SetDefEP7PathDef
// @symbol _ZN7PathPtrC1Ev
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
 *   (func_0200b0fc) compares it to 0xff and shifts it << 12 as a cap, but
 *   what the PathDef byte means is unrecovered; renaming the method would
 *   rename the mangled symbol tree-wide.
 * - func_0203ad84 formats data_020994d4 into data_020a0d90 (0x1f chars plus
 *   forced NUL) via func_02018434; both globals are unrelated to paths and
 *   their contents are unrecovered, so they keep linker names. */
#include "PathPtr.h"

extern PathDef *PATH_DEFS;  /* the level's PathDef table */
extern char *PATH_NODES;    /* flat node records, six bytes each */
extern int NUM_PATHS;       /* entries in PATH_DEFS */

extern "C" {
void func_02018434(int a0, int a1, int a2);
extern char data_020994d4[];
extern char data_020a0d90[];
}

// @symbol func_0203ad84
extern "C" void func_0203ad84(void) {
    func_02018434((int)data_020994d4, (int)data_020a0d90, 0x1f);
    data_020a0d90[0x1f] = 0;
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
    char *base = GetNodeTable();
    u16 first = def->firstNode;
    char *row = base + first * 6;
    int off = idx * 6;
    node.x = ((int)*(s16 *)(row + off)) << 12;
    node.y = ((int)*(s16 *)(row + off + 2)) << 12;
    node.z = ((int)*(s16 *)(row + off + 4)) << 12;
}

void PathPtr::SetNodeTable(void *nodes)
{
    PATH_NODES = (char *)nodes;
}

char *PathPtr::GetNodeTable()
{
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
