//cpp
#include "types.h"
#include "nitro/fx/fx.h"
#include "Ov007Mesh.h"
#include "Ov007Texture.h"
#include "nitro/fx/fx_cp.h"

#include "nitro/gx/g3imm_cmd.h"

/* The NitroSDK immediate-mode commands evaluate their arguments right to
 * left, which is the z, y, x load order the ROM shows for the translate. */

extern "C" void func_ov007_020c1448(Ov007Texture *self, u32 texGen, u32 repeat, u32 flip, u32 color0);
extern "C" void Texture_BindPalette(Ov007Texture *tex);
extern void func_ov007_020caeac(Vector3 *vtx, Vector3_16 *nrm, s32 *tu, s32 *tv, s32 w, s32 h,
                                s32 u0, s32 v0, s32 du, s32 dv, s32 su, s32 sv, s32 shift);
extern void func_ov007_020ca86c(Vector3 *vtx, Vector3_16 *nrm, s32 w, s32 h, s32 shift);

/* Draws the mesh (data_ov007_02104bd4[0]) with one of its materials
 * ([1] or [2]): sets the material's light colours, texture and polygon
 * attributes, translates by pos and scales z by scaleZ, then emits the
 * grid. With smooth set it is drawn textured with the per-point normals
 * (func_ov007_020caeac), otherwise flat with the face normals clamped to
 * 0xfff (func_ov007_020ca86c). */
extern "C" void func_ov007_020ca5f0(Ov007Mesh *mesh, Ov007MeshMaterial *mat, int *pos, int scaleZ,
                         int smooth) {
    Ov007Texture *tex = mat->texture;
    Ov007Grid *grid = mesh->grid;
    int cellCols = grid->columns - 1;
    int cellRows = grid->rows - 1;
    int tw = 0;
    int th = 0;
    REG_DIF_AMB = (u32)mat->diffuse | ((u32)mat->ambient << 16) | 0x8000;
    REG_SPE_EMI = 0;
    if (tex != 0) {
        tw = 8 << tex->sizeS;
        th = 8 << tex->sizeT;
    }
    func_ov007_020c1448(tex, 1, mat->texRepeat, 0, mat->texColor0);
    Texture_BindPalette(tex);
    REG_POLYGON_ATTR =
        0x8c1 | (mat->polygonMode << 4) | (mat->polygonId << 24) | (mat->alpha << 16);
    G3_PushMtx();
    G3_Translate(pos[0], pos[1], pos[2]);
    G3_Scale(0x1000, 0x1000, scaleZ);
    G3_PushMtx();
    G3_Scale(0x40000, 0x40000, 0x40000);
    if (smooth != 0) {
        int du = FX_DivS32(tw * mat->tilesS, cellCols);
        int dv = FX_DivS32(th * mat->tilesT, cellRows);
        int shiftS = FX_MUL(mesh->cellWidth, mat->warpS);
        int shiftT = FX_MUL(mesh->cellHeight, mat->warpT);
        func_ov007_020caeac(mesh->verts, mesh->vertexNormals, mesh->texOffsetS, mesh->texOffsetT,
                            cellCols, cellRows, mat->texS, mat->texT, du, dv, shiftS, shiftT, 6);
    } else {
        int i;
        for (i = 0; i < cellRows * cellCols * 2; i++) {
            if (mesh->faceNormals[i].x > 0xfff) {
                mesh->faceNormals[i].x = 0xfff;
            }
            if (mesh->faceNormals[i].y > 0xfff) {
                mesh->faceNormals[i].y = 0xfff;
            }
            if (mesh->faceNormals[i].z > 0xfff) {
                mesh->faceNormals[i].z = 0xfff;
            }
        }
        func_ov007_020ca86c(mesh->verts, mesh->faceNormals, cellCols, cellRows, 6);
    }
    G3_PopMtx(2);
}
