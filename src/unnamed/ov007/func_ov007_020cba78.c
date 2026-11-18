#include "Ov007Mesh.h"

extern void func_ov007_020c7240(Ov007Grid *grid, int stiffness, int damping);
extern void func_ov007_020c7a84(Ov007Link *link, int stiffness, int damping);
extern void func_ov007_020c71c0(Ov007Grid *grid, int damping, int shift);

/* One spring step: the grid's rows and columns, then every cell link along
 * z, then the points integrate (func_ov007_020c71c0). */
void func_ov007_020cba78(Ov007Mesh *mesh, int shift) {
    int i;
    int n;
    func_ov007_020c7240(mesh->grid, mesh->params.stiffness, mesh->params.damping);
    for (i = 0; (n = (mesh->grid->rows - 1) * (mesh->grid->columns - 1)), i < n; i++) {
        func_ov007_020c7a84(mesh->cellLinks[i], mesh->params.stiffness, mesh->params.damping);
    }
    func_ov007_020c71c0(mesh->grid, 0, shift);
}
