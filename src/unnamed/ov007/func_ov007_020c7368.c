#include "Ov007Spring.h"
#include "Ov007MenuMem.h"

extern void func_ov007_020c78dc(Ov007PointList *list);

/* Frees a func_ov007_020c73f0 grid: every row list, then every column list
 * (func_ov007_020c78dc), both list arrays and the grid itself. */
void func_ov007_020c7368(Ov007Grid *grid) {
    int i;
    for (i = 0; i < grid->rows; i++)
        func_ov007_020c78dc(grid->rowLists[i]);
    for (i = 0; i < grid->columns; i++)
        func_ov007_020c78dc(grid->columnLists[i]);
    func_ov007_020c3d1c(grid->rowLists);
    func_ov007_020c3d1c(grid->columnLists);
    func_ov007_020c3d1c(grid);
}
