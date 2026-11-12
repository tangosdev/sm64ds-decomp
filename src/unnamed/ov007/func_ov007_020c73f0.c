#include "Ov007Spring.h"
#include "Ov007MenuMem.h"

extern void func_ov007_020c7348(Ov007Grid *grid);
extern Ov007PointList *func_ov007_020c798c(int springMode, int count, int pointMass, int flags);

/* Builds a columns x rows grid of points of mass pointMass (Q12). The row
 * lists make the points; each column list shares them, point i of every
 * row. */
Ov007Grid *func_ov007_020c73f0(int springMode, int columns, int rows, int pointMass) {
    int i;
    int j;
    Ov007Grid *grid;
    grid = func_ov007_020c3df4(0, sizeof(Ov007Grid));
    func_ov007_020c7348(grid);
    grid->rowLists = func_ov007_020c3df4(0, rows * 4);
    grid->columnLists = func_ov007_020c3df4(0, columns * 4);
    grid->columns = columns;
    grid->rows = rows;
    for (j = 0; j < rows; j++) {
        grid->rowLists[j] = func_ov007_020c798c(springMode, columns, pointMass, 1);
    }
    for (i = 0; i < columns; i++) {
        grid->columnLists[i] = func_ov007_020c798c(springMode, rows, pointMass, 0);
        for (j = 0; j < rows; j++) {
            grid->columnLists[i]->items[j] = grid->rowLists[j]->items[i];
        }
    }
    return grid;
}
