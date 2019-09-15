#define _GNU_SOURCE

// abort
#include <stdlib.h>

// memcpy
#include <string.h>

#include "island-lab-config.h"
#include "util.h"
#include "resolver.h"

#define UTL_S(name) island_lab_util_ ## name
#define S(name) island_lab_resolver_ ## name

int
S (resolve) (int *perimeter, int *buildings, int ***resolved_buildingss_ptr,
        struct S (resolve_options) *options)
{
    // BEGIN STUB/TEST/DEBUG IMPLEMENTATION
    (void) perimeter;
    int branches = 0;
    for (int iter = 0; iter < 20; ++iter)
    {
        branches = branches * 2 + 123;
        if (branches > options->max_branches)
        {
            if (options->max_branches_exceeded)
            {
                options->max_branches_exceeded (
                        options->max_branches_exceeded_data);

            }
            abort ();
        }
        if (options->show_iterations && options->show_iter)
        {
            options->show_iter (iter + 1, 20 - iter - 1, branches,
                    options->show_iter_data);
        }
    }

    int resolved = 3;
    int **resolved_buildingss = calloc (resolved,
            sizeof (*resolved_buildingss));
    if (!resolved_buildingss) { abort (); }
    for (int i = 0; i < resolved; ++i)
    {
        resolved_buildingss[i] = calloc (UTL_S (buildings_size) (),
                sizeof (*resolved_buildingss[i]));
        if (!resolved_buildingss[i]) { abort (); }
        memcpy (resolved_buildingss[i], buildings,
                UTL_S (buildings_size) () *
                sizeof (*resolved_buildingss[i]));
        resolved_buildingss[i][0] = i + 1;
        resolved_buildingss[i][2] = i + 1;
        resolved_buildingss[i][4] = i + 1;
        resolved_buildingss[i][6] = i + 1;
        resolved_buildingss[i][7] = i + 1;
        resolved_buildingss[i][9] = i + 1;
        resolved_buildingss[i][11] = i + 1;
    }
    *resolved_buildingss_ptr = resolved_buildingss;
    return resolved;
    // ENDOF STUB/TEST/DEBUG IMPLEMENTATION
}

// vi:ts=4:sw=4:et
