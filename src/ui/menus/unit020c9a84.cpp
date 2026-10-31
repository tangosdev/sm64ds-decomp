//cpp
/* unit020c9a84 -- ov007 unit 0x020c9a84..0x020ca1d4: 6 functions, .text only.
 *
 * The menu's item swings. ov007 is the title and file-select menu program
 * that dScDSMT_c hosts; outside dScDSMT_c it has no ROM RTTI or vtable, so
 * no class name is claimed and the file keeps the unit<addr> name.
 *
 * What the code does. Each menu item hangs from the grid mesh on a swing
 * (Ov007Swing, include/Ov007Swing.h): a chain of spring points along z with
 * its first point pinned, and two pairs of side points spread along x and
 * along y, each linked to the mesh point nearest it.
 * func_ov007_020ca010 builds a swing from its template and
 * func_ov007_020c9fa0 frees it; func_ov007_020c9f10 places it and
 * func_ov007_020c9d90 binds it to the mesh; func_ov007_020c9c24 steps it
 * once; func_ov007_020c9a84 turns it into the item sprite's offset and tilt.
 *
 * Edges: these six are the overlay's only functions that take an
 * Ov007Swing. func_ov007_020c9a2c before them parses a binary file
 * (Ov007BinFile) and func_ov007_020ca1d4 after them starts the mesh-wave
 * functions.
 *
 * Emission order: mwccarm 2004/b56 emits one .text section per function in
 * the reverse of source order, so the highest-address function is written
 * first here and the functions come out in ROM order.
 *
 * The functions keep their ROM names under C linkage.
 */
#include "types.h"
#include "nitro/fx/fx.h"
#include "cstd.h"
#include "math/CalcLib.h"
#include "Ov007Swing.h"

extern "C" {

extern void *func_ov007_020c3df4(const char *name, int size);
extern void func_ov007_020c3d1c(void *p);
extern int func_ov007_020c30ac(Vector3 *arr, int count, Vector3 *target, int arg4, int *out);
extern void func_ov007_020c7504(Ov007PointList *list, int damping, int shift);
extern void func_ov007_020c7560(Ov007PointList *list, int stiffness, int damping);
extern void func_ov007_020c7804(Ov007PointList *list);
extern void func_ov007_020c78dc(void *p);
extern Ov007PointList *func_ov007_020c798c(int springMode, int count, int pointMass, int flags);
extern void func_ov007_020c7a84(Ov007Link *link, int stiffness, int damping);
extern void func_ov007_020c8010(Ov007Link *link, Ov007Follower *from, Ov007Follower *to);
extern void func_ov007_020c8098(void *p);
extern Ov007Link *func_ov007_020c80a4(void);
extern void func_ov007_020c80c8(Ov007Follower *f, int damping, int shift);
extern void func_ov007_020c8440(void *p);
extern Ov007Follower *func_ov007_020c844c(Vector3 *p, int q);
extern int func_020538b8(int x, int y);

/* The ROM copies a Vector3 here as a block move (ldm/stm). The C++ front end
   copies Vector3, and any struct of three named ints, member by member; a
   struct whose only member is an array keeps the block move, so every copy
   goes through Vector3Bits. */
struct Vector3Bits {
    int w[3];
};

/* *dst = *src, as the block move. */
static inline void CopyVec3(Vector3 *dst, const Vector3 *src) {
    *(Vector3Bits *)dst = *(const Vector3Bits *)src;
}

/* A POD Vector3 for locals. A real Vector3 local emits the vague-linkage
   ~Vector3 into this text-only unit, and with Vector3's user-declared
   constructor the zero-initialised locals store differently from the ROM's. */
struct RawVector3 {
    int x, y, z;
};

/* Ov007Swing's side point positions (0x20 xPointPos, 0x38 yPointPos) as
   block-copy rows. Copying into swing->xPointPos[j] through a cast pointer
   shares its address with the func_ov007_020c844c argument; the ROM forms
   the copy's address from the swing plus a fixed offset, as a member
   access does. */
struct SwingSideRows {
    u8 pad[0x20];
    Vector3Bits x[2];
    Vector3Bits y[2];
};

/* Builds a swing from params: a chain of chainLength points laid along z
 * with x/y springs (mode 2) and its first point pinned, and two pairs of
 * side points with a link each, for func_ov007_020c9d90 to tie to the
 * mesh. */
// @symbol func_ov007_020ca010
Ov007Swing *func_ov007_020ca010(Ov007SwingParams *params) {
    Ov007Swing *swing;
    int n;
    int i;
    int j;
    Ov007PointList *chain;

    swing = (Ov007Swing *)func_ov007_020c3df4(0, sizeof(Ov007Swing));
    swing->mesh = 0;
    swing->params = params;
    n = params->chainLength;
    /* A temporary, not a named zero: the ROM gives it the frame's last slot. */
    (Vector3Bits &)swing->anchor = Vector3Bits();
    swing->chainPositions = (Vector3 *)func_ov007_020c3df4(0, n * sizeof(Vector3));
    swing->chain = func_ov007_020c798c(2, params->chainLength, params->chainPointMass, 1);
    chain = swing->chain;
    for (i = 0; i < params->chainLength; i++) {
        RawVector3 v = {0, 0, 0};
        v.z = i << 12;
        CopyVec3(&swing->chainPositions[i], (Vector3 *)&v);
        chain->items[i]->target = &swing->chainPositions[i];
    }
    func_ov007_020c7804(chain);
    chain->items[0]->flags |= 1;
    for (j = 0; j < 2; j++) {
        {
            RawVector3 a = {0, 0, 0};
            a.x = j << 12;
            ((SwingSideRows *)swing)->x[j] = (Vector3Bits &)a;
        }
        {
            RawVector3 b = {0, 0, 0};
            b.y = j << 12;
            ((SwingSideRows *)swing)->y[j] = (Vector3Bits &)b;
        }
        swing->xPoints[j] = func_ov007_020c844c(&swing->xPointPos[j], params->sidePointMass);
        swing->yPoints[j] = func_ov007_020c844c(&swing->yPointPos[j], params->sidePointMass);
        swing->xLinks[j] = func_ov007_020c80a4();
        swing->yLinks[j] = func_ov007_020c80a4();
    }
    return swing;
}

/* Frees the swing: its side points (func_ov007_020c8440) and links
 * (func_ov007_020c8098), the chain (func_ov007_020c78dc), the chain's
 * positions and the swing itself. */
// @symbol func_ov007_020c9fa0
void func_ov007_020c9fa0(Ov007Swing *self) {
    int i;
    for (i = 0; i < 2; i++) {
        func_ov007_020c8440(self->xPoints[i]);
        func_ov007_020c8440(self->yPoints[i]);
        func_ov007_020c8098(self->xLinks[i]);
        func_ov007_020c8098(self->yLinks[i]);
    }
    func_ov007_020c78dc(self->chain);
    func_ov007_020c3d1c(self->chainPositions);
    func_ov007_020c3d1c(self);
}

/* Places the swing at pos: the anchor there, the x side pair w apart along
 * x and the y side pair h apart along y, both centred on it. */
// @symbol func_ov007_020c9f10
void func_ov007_020c9f10(Ov007Swing *self, Vector3 *pos, int w, int h) {
    CopyVec3(&self->yPointPos[1], pos);
    CopyVec3(&self->yPointPos[0], &self->yPointPos[1]);
    CopyVec3(&self->xPointPos[1], &self->yPointPos[0]);
    CopyVec3(&self->xPointPos[0], &self->xPointPos[1]);
    CopyVec3(&self->anchor, &self->xPointPos[0]);
    self->xPointPos[0].x -= w / 2;
    self->xPointPos[1].x += w / 2;
    self->yPointPos[0].y -= h / 2;
    self->yPointPos[1].y += h / 2;
}

/* Binds the swing to mesh: meshPoint is the mesh point nearest the anchor,
 * and each side point is linked to the mesh point nearest it. Each link is
 * bound with that mesh point held at restZ, so its rest offset is taken
 * against the flat mesh. A mesh point's row is taken as its index / rows,
 * not index / columns, as the ROM has it. */
// @symbol func_ov007_020c9d90
void func_ov007_020c9d90(Ov007Swing *self, Ov007Mesh *mesh) {
    int count;
    int i;
    int nearestX;
    int nearestY;
    int column;
    int row;
    Ov007Grid *grid;
    Ov007Follower *point;
    int oldZ;

    count = mesh->grid->columns * mesh->grid->rows;
    self->mesh = mesh;
    self->meshPoint = func_ov007_020c30ac(mesh->verts, count, &self->anchor, -1, 0);

    for (i = 0; i < 2; i++) {
        nearestX = func_ov007_020c30ac(mesh->verts, count, &self->xPointPos[i], -1, 0);
        nearestY = func_ov007_020c30ac(mesh->verts, count, &self->yPointPos[i], -1, 0);

        grid = mesh->grid;
        column = cstd::mod(nearestX, grid->columns);
        row = cstd::div(nearestX, grid->rows);
        point = grid->rowLists[row]->items[column];
        oldZ = point->target->z;
        point->target->z = mesh->params.restZ;
        func_ov007_020c8010(self->xLinks[i], self->xPoints[i], point);
        point->target->z = oldZ;

        grid = mesh->grid;
        column = cstd::mod(nearestY, grid->columns);
        row = cstd::div(nearestY, grid->rows);
        point = grid->rowLists[row]->items[column];
        oldZ = point->target->z;
        point->target->z = mesh->params.restZ;
        func_ov007_020c8010(self->yLinks[i], self->yPoints[i], point);
        point->target->z = oldZ;
    }
}

/* One simulation step: the side points spring along z, the chain's pinned
 * first point follows the mesh normal under it, then the chain springs and
 * integrates. shift is the integration step's shift. */
// @symbol func_ov007_020c9c24
void func_ov007_020c9c24(Ov007Swing *self, int shift) {
    int i;
    Vector3_16 *normal;
    int nx, ny;
    int bound;

    for (i = 0; i < 2; i++) {
        func_ov007_020c7a84(self->xLinks[i], self->params->sideStiffness,
                            self->params->sideDamping);
        func_ov007_020c80c8(self->xPoints[i], 0, shift);
        func_ov007_020c7a84(self->yLinks[i], self->params->sideStiffness,
                            self->params->sideDamping);
        func_ov007_020c80c8(self->yPoints[i], 0, shift);
    }

    normal = &self->mesh->vertexNormals[self->meshPoint];
    bound = self->params->maxNormalX;
    nx = normal->x;
    ny = normal->y;
    if (nx > bound)
        nx = bound;
    else if (nx < -bound)
        nx = (s16)(-bound);
    bound = self->params->maxNormalY;
    if (ny > bound)
        ny = bound;
    else if (ny < -bound)
        ny = (s16)(-bound);

    self->chain->items[0]->target->x = FX_MUL(nx, self->params->normalScale);
    self->chain->items[0]->target->y = FX_MUL(ny, self->params->normalScale);

    func_ov007_020c7560(self->chain, self->params->chainStiffness, self->params->chainDamping);
    func_ov007_020c7504(self->chain, 0, shift);
}

/* a * b in 20.12 fixed point, truncated. */
static inline int MulFx(int a, int b) {
    return (int)(((s64)a * b) >> 12);
}

static inline int Sign(int v) {
    return (v >= 0) ? 1 : -1;
}

/* size / cap as a 20.12 fraction, held to [0, 1]. */
static inline int CapFraction(int size, u16 cap) {
    if (size <= 0)
        return 0;
    if (size >= cap)
        return 0x1000;
    return cstd::div(size << 12, cap);
}

/* Turns the swing into the item sprite's pose (func_ov007_020beeb0 passes
 * the sprite's posOffset, rotX and rotY). offset takes the x and y of the
 * chain's last point. Each side pair's tilt is the atan2 of the span between
 * its points, shifted by 0x3fff (x pair) or 0x7fff (y pair), a quarter or
 * half turn less one. Its size over maxRot, held to [0, 1], is eased out
 * as t (2 - t), and the rotation is that share of maxRot with the tilt's
 * sign. */
// @symbol func_ov007_020c9a84
void func_ov007_020c9a84(Ov007Swing *self, Vector3 *offset, u16 *rotX, u16 *rotY) {
    Vector3 span;
    int angleX, angleY;
    s16 tiltX, tiltY;
    int sizeX, sizeY;
    int fracX, fracY;
    int easeX, easeY;

    offset->x = self->chainPositions[self->chain->count - 1].x;
    offset->y = self->chainPositions[self->chain->count - 1].y;

    SubVec3(&self->xPointPos[0], &self->xPointPos[1], &span);
    angleX = func_020538b8(span.x, span.z);
    SubVec3(&self->yPointPos[0], &self->yPointPos[1], &span);
    angleY = func_020538b8(span.z, span.y);
    tiltX = (u16)(angleX + 0x3fff);
    tiltY = (u16)(angleY + 0x7fff);

    sizeX = (tiltX >= 0) ? tiltX : -tiltX;
    fracX = CapFraction(sizeX, self->params->maxRotX);
    sizeY = (tiltY >= 0) ? tiltY : -tiltY;
    fracY = CapFraction(sizeY, self->params->maxRotY);

    easeX = MulFx(fracX, 0x2000 - fracX);
    easeY = MulFx(fracY, 0x2000 - fracY);
    *rotX = (s16)((self->params->maxRotX * (easeX * Sign(tiltX))) >> 12);
    *rotY = (s16)((self->params->maxRotY * (easeY * Sign(tiltY))) >> 12);
}
}
