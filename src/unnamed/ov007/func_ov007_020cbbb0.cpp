//cpp
#include "types.h"
#include "nitro/fx/fx.h"
#include "Ov007Mesh.h"
#include "nitro/fx/fx_cp.h"

/* The ROM copies the template with one ldm/stm block. Under C++ a struct of
 * six named ints copies word by word, a struct with an array member as one
 * block, so the copy goes through this. */
struct MeshParamsWords {
    int w[6];
};

extern "C" void *func_ov007_020c3df4(const char *name, int size);
extern "C" Ov007Grid *func_ov007_020c73f0(int springMode, int columns, int rows, int pointMass);
extern "C" Ov007Link *func_ov007_020c80a4(void);
extern "C" void func_ov007_020c72d0(Ov007Grid *grid);
void func_ov007_020c8010(Ov007Link *link, Ov007Follower *from, Ov007Follower *to);
extern "C" void func_ov007_020cb3dc(Ov007Mesh *mesh);

/* Builds a columns x rows mesh `width` wide, centred on the origin, every
 * point at the template's rest z. Rows are cellHeight apart (width *
 * sqrt(2) / 2 over the columns - 1 cells), odd rows shifted half a cell
 * along x, and the border points are pinned. Each cell gets a link across
 * its diagonal, which alternates with the row. The link pass reuses
 * `columns` as the row's first cell index.
 *
 * The link table holds one Ov007Link * per cell but is allocated
 * sizeof(Ov007Link) bytes per cell, six times what it uses. */
Ov007Mesh *func_ov007_020cbbb0(int columns, int rows, int width, Ov007MeshParams *params) {
    int cellWidth, cellHeight, t, halfWidth, halfHeight, cellRows, cellColumns, j, i;
    Ov007Mesh *mesh;

    mesh = (Ov007Mesh *)func_ov007_020c3df4(0, sizeof(Ov007Mesh));
    cellColumns = columns - 1;
    cellWidth = FX_DivS32(width, cellColumns);
    t = FX_MUL(width, 0x16a1);
    cellHeight = FX_DivS32(t / 2, cellColumns);
    {
        int n = columns - 1;
        cellColumns = n;
    }
    halfWidth = (cellWidth * cellColumns) / 2;
    cellRows = rows - 1;
    halfHeight = (cellHeight * cellRows) / 2;
    mesh->verts = (Vector3 *)func_ov007_020c3df4(0, columns * 12 * rows);
    mesh->faceNormals = (Vector3_16 *)func_ov007_020c3df4(0, cellColumns * 6 * cellRows * 2);
    mesh->vertexNormals = (Vector3_16 *)func_ov007_020c3df4(0, columns * 6 * rows);
    mesh->texOffsetS = (int *)func_ov007_020c3df4(0, columns * 4);
    mesh->texOffsetT = (int *)func_ov007_020c3df4(0, columns * 4);
    mesh->cellWidth = cellWidth;
    mesh->cellHeight = cellHeight;
    mesh->cellLinks =
        (Ov007Link **)func_ov007_020c3df4(0, cellColumns * sizeof(Ov007Link) * cellRows);
    mesh->grid = func_ov007_020c73f0(3, columns, rows, params->pointMass);

    for (i = 0; i < rows; i++) {
        for (j = 0; j < columns; j++) {
            Vector3 *elem = &mesh->verts[i * columns + j];
            elem->x = j * cellWidth - halfWidth + ((i % 2 == 0) ? 0 : cellWidth / 2);
            elem->y = cellHeight * (rows - 1 - i) - halfHeight;
            elem->z = params->restZ;
            mesh->grid->rowLists[i]->items[j]->target = elem;
            if (j < cellColumns && i < cellRows) {
                mesh->cellLinks[i * cellColumns + j] = func_ov007_020c80a4();
            }
            if (j == 0 || j == columns - 1 || i == 0 || i == rows - 1) {
                mesh->grid->rowLists[i]->items[j]->flags |= 1;
            }
        }
    }

    func_ov007_020c72d0(mesh->grid);

    {
        i = 0;
        if (cellRows > 0) {
            columns = i;
            do {
                j = 0;
                if (cellColumns > 0) {
                    do {
                        int i2mod = i % 2;
                        int t = i2mod;
                        int i2p1 = i + 1;
                        int sum = j + columns;
                        if (i2mod == 0) {
                            Ov007Grid *g = mesh->grid;
                            Ov007Link **links = mesh->cellLinks;
                            Ov007Link *link = links[sum];
                            Ov007Follower *ea = g->rowLists[i]->items[j + 1];
                            Ov007Follower *eb = g->rowLists[i2p1]->items[j];
                            func_ov007_020c8010(link, ea, eb);
                        } else {
                            Ov007Grid *g = mesh->grid;
                            Ov007Link **links = mesh->cellLinks;
                            Ov007Link *link = links[sum];
                            Ov007Follower *ea = g->rowLists[i]->items[j];
                            Ov007Follower *eb = g->rowLists[i2p1]->items[j + 1];
                            func_ov007_020c8010(link, ea, eb);
                        }
                        j++;
                    } while (j < cellColumns);
                }
                i++;
                columns += cellColumns;
            } while (i < cellRows);
        }
    }

    *(MeshParamsWords *)&mesh->params = *(MeshParamsWords *)params;
    func_ov007_020cb3dc(mesh);
    return mesh;
}
