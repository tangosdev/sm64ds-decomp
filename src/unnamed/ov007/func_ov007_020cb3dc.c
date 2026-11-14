#include "Ov007Mesh.h"

extern void func_ov007_020cb7c0(Vector3_16 *out, int w, int h, int *pts, s16 a5, s16 a6);
extern void func_ov007_020cb4b0(s16 *p, s16 *nrm, int w1, int h1, int x, int y);
extern void func_ov007_020c368c(Vector3_16 *v, int count, int clamp);

/* Recomputes the mesh's normals: the face normals of every cell
 * (func_ov007_020cb7c0), then each point's normal from the faces around it
 * (func_ov007_020cb4b0), and flushes the point normals with
 * func_ov007_020c368c. */
void func_ov007_020cb3dc(Ov007Mesh *mesh) {
    int x, y;
    Ov007Grid *grid = mesh->grid;
    int w1 = grid->columns - 1;
    int h1 = grid->rows - 1;
    Vector3_16 *p;

    func_ov007_020cb7c0(mesh->faceNormals, w1, h1, (int *)mesh->verts, mesh->cellWidth,
                        mesh->cellHeight);

    p = mesh->vertexNormals;
    for (y = 0; y < mesh->grid->rows; y++) {
        for (x = 0; x < mesh->grid->columns; x++) {
            func_ov007_020cb4b0((s16 *)p, (s16 *)mesh->faceNormals, w1, h1, x, y);
            p++;
        }
    }

    func_ov007_020c368c(mesh->vertexNormals, mesh->grid->columns * mesh->grid->rows, 1);
}
