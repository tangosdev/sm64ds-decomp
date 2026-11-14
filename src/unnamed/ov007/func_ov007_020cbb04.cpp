//cpp
#include "Ov007Mesh.h"

/* func_ov007_020c8098 is a long-branch veneer to func_ov007_020c3d1c, the
 * free hook. */
extern "C" void func_ov007_020c8098(void *p);
extern "C" void func_ov007_020c7368(Ov007Grid *grid);
extern "C" void func_ov007_020c3d1c(void *self);

/* Frees the mesh func_ov007_020cbbb0 builds: its buffers, every cell link,
 * the link table, the grid (func_ov007_020c7368) and the mesh itself. */
void func_ov007_020cbb04(Ov007Mesh *mesh) {
    func_ov007_020c3d1c(mesh->verts);
    func_ov007_020c3d1c(mesh->faceNormals);
    func_ov007_020c3d1c(mesh->vertexNormals);
    func_ov007_020c3d1c(mesh->texOffsetS);
    func_ov007_020c3d1c(mesh->texOffsetT);

    int i = 0;
    Ov007Grid *grid = mesh->grid;
    if ((grid->rows - 1) * (grid->columns - 1) > 0) {
        do {
            func_ov007_020c8098(mesh->cellLinks[i]);
            i++;
            grid = mesh->grid;
        } while (i < (grid->rows - 1) * (grid->columns - 1));
    }
    func_ov007_020c3d1c(mesh->cellLinks);
    func_ov007_020c7368(mesh->grid);
    func_ov007_020c3d1c(mesh);
}
