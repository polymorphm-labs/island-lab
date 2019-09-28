#define _GNU_SOURCE

#include "island-lab-config.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "util.h"
#include "resolver.h"

#define CFG_M(name) ISLAND_LAB_CONFIG_ ## name
#define UTL_S(name) island_lab_util_ ## name
#define RSL_S(name) island_lab_resolver_ ## name

static UTL_S (building_t) test_perimeter[] =
        {0,0,0,2,2,0,0,0,0,6,3,0,0,4,0,0,0,0,4,4,0,3,0,0};
static UTL_S (building_t) test_empty_buildings[] =
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static UTL_S (building_t) exp_buildings[] =
        {5,6,1,4,3,2,4,1,3,2,6,5,2,3,6,1,5,4,6,5,4,3,2,1,1,2,5,6,4,3,3,4,2,5,1,6};

static int max_branches = 1048576;

static int
cmp_buildings (UTL_S (building_t) *buildings_a,
        UTL_S (building_t) *buildings_b)
{
    for (int i = 0; i < UTL_S (buildings_size) (); ++i)
    {
        if (buildings_a[i] < buildings_b[i])
        {
            return -1;
        }

        if (buildings_a[i] > buildings_b[i])
        {
            return 1;
        }
    }

    return 0;
}

int
test_resolver_regular ()
{
    int test_result = 1;
    int resolved = 0;
    int error;
    UTL_S (building_t) **resolved_buildingss = 0;
    struct RSL_S (resolve_options) rsl_options = {
        .init_buildings_heap = CFG_M (INIT_BUILDINGS_HEAP),
        .max_branches = max_branches,
    };

    resolved = RSL_S (resolve) (test_perimeter, test_empty_buildings,
            &resolved_buildingss, &rsl_options, &error);

    if (resolved != 1)
    {
        test_result = 0;
        goto exit;
    }

    if (cmp_buildings (exp_buildings, resolved_buildingss[0]))
    {
        test_result = 0;
        goto exit;
    }

exit:
    for (int i = 0; i < resolved; ++i)
    {
        free (resolved_buildingss[i]);
    }
    free (resolved_buildingss);

    return test_result;
}

#ifdef TEST_FUNCTION_NAME
int
main ()
{
    if (UTL_S (general_size) () != 6)
    {
        fprintf (stderr, "general_size != 6: %d\n", UTL_S (general_size) ());
        return 77; // skip test
    }

    int res = TEST_FUNCTION_NAME ();

    if (!res)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
#endif

// vi:ts=4:sw=4:et
