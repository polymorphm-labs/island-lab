#define _GNU_SOURCE

// abort
#include <stdlib.h>

// memcpy
#include <string.h>

// t*
#include <search.h>

#include "island-lab-config.h"
#include "util.h"
#include "resolver.h"

#define UTL_S(name) island_lab_util_ ## name
#define S(name) island_lab_resolver_ ## name


static int
is_line_unique (int *buildings, int (*line_building_idx) (int cns, int var),
        int cns)
{
    // checking for the puzzle's rule about line's uniqueness.
    //
    // use cases:
    //      for horizontal line:
    //          line_building_idx=hori_building_idx
    //      for vertical line:
    //          line_building_idx=vert_building_idx

    for (int a = 0; a < UTL_S (general_size) (); ++a)
    {
        int building_a = buildings[line_building_idx (cns, a)];

        if (!building_a)
        {
            continue;
        }

        for (int b = 0; b < UTL_S (general_size) (); ++b)
        {
            int building_b = buildings[line_building_idx (cns, b)];

            if (b != a && building_b && building_b == building_a)
            {
                return 0;
            }
        }
    }

    return 1;
}

static int
is_observer_possible (int *perimeter, int *buildings,
        int (*side_obs_idx) (int cns),
        int (*line_building_idx) (int cns, int var), int cns)
{
    // checking for the puzzle's rule about side observer possibility.
    //
    // use cases for various sides of the world:
    //      north side:
    //          side_obs_idx=north_obs_idx
    //          line_building_idx=vert_building_idx
    //      east side:
    //          side_obs_idx=east_obs_idx
    //          line_building_idx=rev_hori_building_idx
    //      south side:
    //          side_obs_idx=south_obs_idx
    //          line_building_idx=rev_vert_building_idx
    //      west side:
    //          side_obs_idx=west_obs_idx
    //          line_building_idx=hori_building_idx

    int obs = perimeter[side_obs_idx (cns)];

    if (!obs)
    {
        return 1;
    }

    int seen_min = 0;
    int seen_max = 0;
    int seen_in_dark = 0;
    int prev_building = 1;

    for (int var = 0; var < UTL_S (general_size) (); ++var)
    {
        int building = buildings[line_building_idx (cns, var)];

        if (!building)
        {
            ++seen_in_dark;

            if (var < UTL_S (general_size) () - 1)
            {
                continue;
            }
        }

        if (building < prev_building)
        {
            building = prev_building;
        }

        int max_seen_in_dark = building - prev_building - 1;

        if (max_seen_in_dark < 0)
        {
            max_seen_in_dark = 0;
        }

        if (seen_in_dark > max_seen_in_dark)
        {
            seen_in_dark = max_seen_in_dark;
        }

        seen_min += building - prev_building;
        seen_max += building - prev_building + seen_in_dark;

        seen_in_dark = 0;
        prev_building = building;
    }

    return obs >= seen_min && obs <= seen_max;
}

static int
are_buildings_allowed (int *perimeter, int *buildings, int j, int i)
{
    // checking for the puzzle's rules!
    // whe should not check every building.
    // looking at line(j) and row(i) should be enough.

    // checking for horizontal line uniqueness

    if (j != -1 && !is_line_unique (buildings, UTL_S (hori_building_idx), j))
    {
        return 0;
    }

    // checking for vertical line uniqueness

    if (i != -1 && !is_line_unique (buildings, UTL_S (vert_building_idx), i))
    {
        return 0;
    }

    // checking for north observer possibility

    if (i != -1 && !is_observer_possible (perimeter, buildings,
            UTL_S (north_obs_idx), UTL_S (vert_building_idx), i))
    {
        return 0;
    }

    // checking for east observer possibility

    if (j != -1 && !is_observer_possible (perimeter, buildings,
            UTL_S (east_obs_idx), UTL_S (rev_hori_building_idx), j))
    {
        return 0;
    }

    // checking for south observer possibility

    if (i != -1 && !is_observer_possible (perimeter, buildings,
            UTL_S (south_obs_idx), UTL_S (rev_vert_building_idx), i))
    {
        return 0;
    }

    // checking for west observer possibility

    if (j != -1 && !is_observer_possible (perimeter, buildings,
            UTL_S (west_obs_idx), UTL_S (hori_building_idx), j))
    {
        return 0;
    }

    return 1;
}

static int
are_all_buildings_allowed (int *perimeter, int *buildings)
{
    // version of checking puzzle's rules for all buildings on the island

    // all horizontal rules

    for (int j = 0; j < UTL_S (general_size) (); ++j)
    {
        if (!are_buildings_allowed (perimeter, buildings, j, -1))
        {
            return 0;
        }
    }

    // all vertical rules

    for (int i = 0; i < UTL_S (general_size) (); ++i)
    {
        if (!are_buildings_allowed (perimeter, buildings, -1, i))
        {
            return 0;
        }
    }

    return 1;
}

static unsigned
hash_buildings (int *buildings)
{
    unsigned h = 5381;

    for (int i = 0; i < UTL_S (buildings_size) (); ++i)
    {
        h = h * 33 + buildings[i];
    }

    return h & 0xffffffff;
}

struct multiverse
{
    int init_buildings_heap;
    int allocated;
    int size;
    int *buildings_heap;
    void *tree;
};

struct buildings_node
{
    struct multiverse *multiverse;
    int buildings_idx;
    unsigned hash;
};

static void
clear_multiverse (struct multiverse *multiverse)
{
    tdestroy (multiverse->tree, free);
    free (multiverse->buildings_heap);

    multiverse->tree = 0;
    multiverse->buildings_heap = 0;
    multiverse->size = 0;
    multiverse->allocated = 0;
}

static int *
get_buildings_by_idx (struct multiverse *multiverse, int buildings_idx)
{
    return &multiverse->buildings_heap
            [buildings_idx * UTL_S (general_size) ()];
}

static int
compare_buildings_nodes (const void *pa, const void *pb)
{
    const struct buildings_node *node_a = pa;
    const struct buildings_node *node_b = pb;

    if (node_a->hash < node_b->hash)
    {
        return -1;
    }

    if (node_a->hash > node_b->hash)
    {
        return 1;
    }

    int *buildings_a = get_buildings_by_idx (node_a->multiverse,
            node_a->buildings_idx);
    int *buildings_b = get_buildings_by_idx (node_b->multiverse,
            node_b->buildings_idx);

    for (int i = 0; i < UTL_S (general_size) (); ++i)
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

static int
allocate_buildings_idx (struct multiverse *multiverse)
{
    // WARNING! it's VARY dangerous function:
    //      memory could be reallocated to another addresses after it is executed.
    //      therefore don't store any multiverse's inside-addresses for a long time

    if (multiverse->allocated == multiverse->size)
    {
        if (multiverse->allocated)
        {
            multiverse->allocated *= 2;
        }
        else
        {
            multiverse->allocated = multiverse->init_buildings_heap;
        }

        multiverse->buildings_heap = reallocarray (multiverse->buildings_heap,
                multiverse->allocated * UTL_S (general_size) (),
                sizeof (*multiverse->buildings_heap));

        if (!multiverse->buildings_heap)
        {
            abort ();
        }
    }

    return multiverse->size;
}

static void
add_allocated_buildings_node (struct multiverse *multiverse)
{
    int buildings_idx = multiverse->size;
    int *buildings = get_buildings_by_idx (multiverse, buildings_idx);
    unsigned hash = hash_buildings (buildings);

    struct buildings_node *node = malloc (sizeof (*node));

    if (!node)
    {
        abort ();
    }

    *node = (struct buildings_node) {
        .multiverse = multiverse,
        .buildings_idx = buildings_idx,
        .hash = hash,
    };

    struct buildings_node **found_node = tsearch (node, &multiverse->tree,
            compare_buildings_nodes);

    if (!found_node)
    {
        abort ();
    }

    if (*found_node != node)
    {
        // a duplicate node was found. canceling adding a new node

        free (node);
        return;
    }

    ++multiverse->size;
}

static void
copy_buildings (int *src_buildings, int *dst_buildings)
{
    memcpy (dst_buildings, src_buildings,
            UTL_S (buildings_size) () * sizeof (*src_buildings));
}

static int *
allocate_buildings_copy (struct multiverse *multiverse, int *src_buildings)
{
    int buildings_idx = allocate_buildings_idx (multiverse);
    int *dst_buildings = get_buildings_by_idx (multiverse, buildings_idx);
    copy_buildings (src_buildings, dst_buildings);

    return dst_buildings;
}

static void
add_buildings_copy (struct multiverse *multiverse, int *buildings)
{
    // adding buildings copy to a multiverse

    allocate_buildings_copy (multiverse, buildings);
    add_allocated_buildings_node (multiverse);
}

struct multiverse_iter_item
{
    int j;
    int i;
};

static int
prepare_iterations (int *buildings, struct multiverse_iter_item **iter_items_ptr)
{
    int size = 0;
    struct multiverse_iter_item *iter_items = 0;

    for (int j = 0; j < UTL_S (general_size) (); ++j)
    {
        for (int i = 0; i < UTL_S (general_size) (); ++i)
        {
            if (!buildings[UTL_S (building_idx) (j, i)])
            {
                ++size;
                iter_items = reallocarray (iter_items, size,
                        sizeof (*iter_items));

                if (!iter_items)
                {
                    abort ();
                }

                iter_items[size - 1] = (struct multiverse_iter_item) {
                    .j = j,
                    .i = i,
                };
            }
        }
    }

    *iter_items_ptr = iter_items;
    return size;
}

static int
return_buildingss (struct multiverse *multiverse,
        int ***returned_buildingss_ptr)
{
    // return buildings into new memory blocks

    int size = multiverse->size;
    int **buildingss = calloc (size, sizeof (*buildingss));

    if (!buildingss)
    {
        abort ();
    }

    for (int i = 0; i < size; ++i)
    {
        int *buildings = get_buildings_by_idx (multiverse, i);
        buildingss[i] = calloc (UTL_S (buildings_size) (),
                sizeof (*buildingss[i]));

        if (!buildingss[i])
        {
            abort ();
        }

        copy_buildings (buildings, buildingss[i]);
    }

    *returned_buildingss_ptr = buildingss;
    return size;
}

int
S (resolve) (int *perimeter, int *buildings, int ***resolved_buildingss_ptr,
        struct S (resolve_options) *options)
{
    struct multiverse multiverse1 = {
        .init_buildings_heap = options->init_buildings_heap,
    };
    struct multiverse multiverse2 = {
        .init_buildings_heap = options->init_buildings_heap,
    };
    struct multiverse *multiverse_a = &multiverse1;
    struct multiverse *multiverse_b = &multiverse2;
    struct multiverse *multiverse_swp;
    struct multiverse_iter_item *iter_items = 0;
    int iter_items_size;

    // first, getting list all planned iterations

    iter_items_size = prepare_iterations (buildings, &iter_items);

    // if planned iterations are got,
    // the next cycle will step-by-step try to apply these iterations

    if (iter_items_size)
    {
        // a regular case:
        //      we should try to apply planned iterations

        for (int iter = 0; iter < iter_items_size; ++iter)
        {
            int j = iter_items[iter].j;
            int i = iter_items[iter].i;

            for (int buildings_idx = 0;
                    buildings_idx < multiverse_a->size;
                    ++buildings_idx)
            {
                int *prev_buildings = get_buildings_by_idx (multiverse_a,
                        buildings_idx);

                for (int building = 1;
                        building <= UTL_S (general_size) ();
                        ++building)
                {
                    int *next_buildings = allocate_buildings_copy (
                            multiverse_a, prev_buildings);

                    next_buildings[UTL_S (building_idx) (j, i)] = building;

                    if (are_buildings_allowed (perimeter, next_buildings,
                            j, i))
                    {
                        add_allocated_buildings_node (multiverse_b);
                    }
                }

                if (multiverse_b->size > options->max_branches)
                {
                    if (options->max_branches_exceeded)
                    {
                        options->max_branches_exceeded (
                                options->max_branches_exceeded_data);
                    }
                    abort ();
                }
            }

            if (options->show_iterations && options->show_iter)
            {
                options->show_iter (iter + 1, multiverse_b->size,
                        options->show_iter_data);
            }

            multiverse_swp = multiverse_a;
            multiverse_a = multiverse_b;
            multiverse_b = multiverse_swp;
            clear_multiverse (multiverse_b);

            if (!multiverse_a->size)
            {
                break;
            }
        }
    }
    else
    {
        // a special case:
        //      the resolved answer is already here,
        //      but it still can be wrong

        add_buildings_copy (multiverse_a, buildings);
    }

    // final checking all buildings in the multiverse and returning good ones

    for (int buildings_idx = 0;
            buildings_idx < multiverse_a->size;
            ++buildings_idx)
    {
        int *prev_buildings = get_buildings_by_idx (multiverse_a,
                buildings_idx);

        if (are_all_buildings_allowed (perimeter, prev_buildings))
        {
            add_buildings_copy (multiverse_b, prev_buildings);
        }
    }

    // returning result into a new memory blocks
    // and freeing the other memory blocks

    int resolved = return_buildingss (multiverse_b, resolved_buildingss_ptr);

    free (iter_items);
    clear_multiverse (&multiverse2);
    clear_multiverse (&multiverse1);

    return resolved;
}

// vi:ts=4:sw=4:et
