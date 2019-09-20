#define _GNU_SOURCE

#include "island-lab-config.h"

// abort free *alloc*
#include <stdlib.h>

// memcpy
#include <string.h>

// t*
#include <search.h>

#ifdef ISLAND_LAB_CONFIG_CUSTOM_REALLOCARRAY
#include "custom-reallocarray.h"
#include "custom-reallocarray-compat.h"
#endif

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
    //
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
    //
    // this function counts three metrics:
    //
    //      * real_seen_cnt:
    //          real situation with REVEALED building only.
    //          how many real buildings we see RIGHT NOW.
    //      * pessim_seen_cnt:
    //          pessimistic prediction. how many buildings WOULD BE if some
    //          too tall and yet unrevealed building hid the other buildings.
    //      * optim_seen_cnt:
    //          optimistic prediction. how many buildings WOULD BE if
    //          unrevealed buildings ordered with accuracy by their height.
    //
    // for simplify implementation (and execute speed) we don't take into
    // account uniqueness buildings in the line.
    //
    // in the end of the function we can give a FALSE-POSITIVE result,
    // but we are forbidden to give a FALSE-NEGATIVE result.

    int obs = perimeter[side_obs_idx (cns)];

    if (!obs)
    {
        // anything is allowed! observer is absent

        return 1;
    }

    int real_seen_cnt = 0;
    int pessim_seen_cnt = 0;
    int optim_seen_cnt = 0;

    // real seen buildings metric

    for (int var = 0, prev_building = 0;
            var < UTL_S (general_size) () &&
            prev_building < UTL_S (general_size) ();
            ++var)
    {
        int building = buildings[line_building_idx (cns, var)];

        if (building <= prev_building)
        {
            continue;
        }

        ++real_seen_cnt;
        prev_building = building;
    }

    // pessimistic prediction metric

    for (int var = 0, prev_building = 0;
            var < UTL_S (general_size) () &&
            prev_building < UTL_S (general_size) ();
            ++var)
    {
        int building = buildings[line_building_idx (cns, var)];

        if (!building)
        {
            building = UTL_S (general_size) ();

            // this hypothetical tallest building will hide the other
        }

        if (building <= prev_building)
        {
            continue;
        }

        ++pessim_seen_cnt;
        prev_building = building;
    }

    // optimistic prediction metric

    for (int var = 0, prev_building = 0;
            var < UTL_S (general_size) () &&
            prev_building < UTL_S (general_size) ();
            ++var)
    {
        int building = buildings[line_building_idx (cns, var)];

        if (!building)
        {
            building = prev_building + 1;
        }

        if (building <= prev_building)
        {
            continue;
        }

        ++optim_seen_cnt;
        prev_building = building;
    }

    // in some use cases real_seen_cnt value could be less
    // than pessim_seen_cnt value.
    // so we have to choose the least value from them
    // to avoid false-negative result answering.
    //
    // but we assert optim_seen_cnt value is always the greatest.

    return (obs >= pessim_seen_cnt || obs >= real_seen_cnt) &&
            obs <= optim_seen_cnt;
}

static int
are_hori_buildings_allowed (int *perimeter, int *buildings, int j)
{
    // checking for the puzzle's rules selectively for a horizontal line (j).
    //
    // steps:
    //      1. checking for horizontal line uniqueness.
    //      2. checking for east observer possibility.
    //      3. checking for west observer possibility.

    return is_line_unique (buildings, UTL_S (hori_building_idx), j) &&
            is_observer_possible (perimeter, buildings,
                    UTL_S (east_obs_idx), UTL_S (rev_hori_building_idx), j) &&
            is_observer_possible (perimeter, buildings,
                    UTL_S (west_obs_idx), UTL_S (hori_building_idx), j);
}

static int
are_vert_buildings_allowed (int *perimeter, int *buildings, int i)
{
    // checking for the puzzle's rules selectively for a vertical line (i).
    //
    // steps:
    //      1. checking for vertical line uniqueness.
    //      2. checking for north observer possibility.
    //      3. checking for south observer possibility.

    return is_line_unique (buildings, UTL_S (vert_building_idx), i) &&
            is_observer_possible (perimeter, buildings,
                    UTL_S (north_obs_idx), UTL_S (vert_building_idx), i) &&
            is_observer_possible (perimeter, buildings,
                    UTL_S (south_obs_idx), UTL_S (rev_vert_building_idx), i);
}

static int
is_building_allowed (int *perimeter, int *buildings, int j, int i)
{
    // checking for the puzzle's rules for only building.
    // we should not check every building here.
    //
    // in this use case looking at one horizontal line (j)
    // and one vertical line (i) should be enough.

    return are_hori_buildings_allowed (perimeter, buildings, j) &&
            are_vert_buildings_allowed (perimeter, buildings, i);
}

static int
are_all_buildings_allowed (int *perimeter, int *buildings)
{
    // version of checking puzzle's rules for all buildings on the island

    // all horizontal rules

    for (int j = 0; j < UTL_S (general_size) (); ++j)
    {
        if (!are_hori_buildings_allowed (perimeter, buildings, j))
        {
            return 0;
        }
    }

    // all vertical rules

    for (int i = 0; i < UTL_S (general_size) (); ++i)
    {
        if (!are_vert_buildings_allowed (perimeter, buildings, i))
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
            [buildings_idx * UTL_S (buildings_size) ()];
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
                multiverse->allocated * UTL_S (buildings_size) (),
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
compare_multiverse_iter_items (const void *pa, const void *pb)
{
    const struct multiverse_iter_item *item_a = pa;
    const struct multiverse_iter_item *item_b = pb;

    if (item_a->j < item_b->j)
    {
        return -1;
    }

    if (item_a->j > item_b->j)
    {
        return 1;
    }

    if (item_a->i < item_b->i)
    {
        return -1;
    }

    if (item_a->i > item_b->i)
    {
        return 1;
    }

    return 0;
}

static int
prepare_iterations (int *buildings, struct multiverse_iter_item **iter_items_ptr)
{
    // this function gets sequence (iter_items) of the resolving buildings.
    // but besides we want to have a specific order of this sequence.
    //
    // let the order is from the island's border to the island's centre.
    // the Pythagorean theorem will help us!

    float centre = ((float) UTL_S (general_size) () - 1.0f) / 2.0f;

    int size = 0;
    struct multiverse_iter_item *iter_items = 0;
    struct multiverse_iter_item iter_item;
    struct multiverse_iter_item best_iter_item;
    float distance;
    float best_distance;

    for (;;)
    {
        int found = 0;

        for (int j = 0; j < UTL_S (general_size) (); ++j)
        {
            for (int i = 0; i < UTL_S (general_size) (); ++i)
            {
                if (!buildings[UTL_S (building_idx) (j, i)])
                {
                    distance = ((float) j - centre) * ((float) j - centre) +
                            ((float) i - centre) * ((float) i - centre);

                    if (!found || distance > best_distance)
                    {
                        iter_item = (struct multiverse_iter_item) {
                            .j = j,
                            .i = i,
                        };

                        int already_have = 0;

                        for (int iter = 0; iter < size; ++iter)
                        {
                            if (compare_multiverse_iter_items (
                                    &iter_items[iter], &iter_item) == 0)
                            {
                                already_have = 1;
                                break;
                            }
                        }

                        if (!already_have)
                        {
                            best_iter_item = iter_item;
                            best_distance = distance;
                            found = 1;
                        }
                    }
                }
            }
        }

        if (!found)
        {
            break;
        }

        ++size;
        iter_items = reallocarray (iter_items, size, sizeof (*iter_items));

        if (!iter_items)
        {
            abort ();
        }

        iter_items[size - 1] = best_iter_item;
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
        struct S (resolve_options) *options, int *error_ptr)
{
    int resolved = 0;
    *error_ptr = 0;

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
    add_buildings_copy (multiverse_a, buildings);

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
                            multiverse_b, prev_buildings);

                    next_buildings[UTL_S (building_idx) (j, i)] = building;

                    if (is_building_allowed (perimeter, next_buildings,
                            j, i))
                    {
                        add_allocated_buildings_node (multiverse_b);
                    }
                }

                if (multiverse_b->size > options->max_branches)
                {
                    *error_ptr = S (max_branches_exceeded_error);
                    goto exit;
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

    resolved = return_buildingss (multiverse_b, resolved_buildingss_ptr);

exit:
    free (iter_items);
    clear_multiverse (&multiverse2);
    clear_multiverse (&multiverse1);

    return resolved;
}

// vi:ts=4:sw=4:et
