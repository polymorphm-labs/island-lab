#define _GNU_SOURCE

#include "island-lab-config.h"

// abort
#include <stdlib.h>

// asprintf fprintf
#include <stdio.h>

#include "util.h"
#include "visual.h"

#define UTL_S(name) island_lab_util_ ## name
#define S(name) island_lab_visual_ ## name

static int
calc_log (int num)
{
    // mathematical logarithm base 10 calculating

    if (num < 1)
    {
        abort ();
    }

    if (num < 10)
    {
        return 1;
    }

    if (num < 100)
    {
        return 2;
    }

    if (num < 1000)
    {
        return 3;
    }

    abort ();
}

static void
append_c (char **strp, char c)
{
    char *swp;
    int res;

    swp = *strp;
    res = asprintf (strp, "%s%c", *strp, c);

    if (res == -1)
    {
        abort ();
    }

    free (swp);
}

static void
append_s (char **strp, const char *s)
{
    char *swp;
    int res;

    swp = *strp;
    res = asprintf (strp, "%s%s", *strp, s);

    if (res == -1)
    {
        abort ();
    }

    free (swp);
}

static void
append_indent (char **strp, int indent)
{
    for (int i = 0; i < indent; ++i)
    {
        append_c (strp, ' ');
    }
}

static void
append_rem_space_with_c (char **strp, int filled, char c)
{
    int cell_size = calc_log (UTL_S (general_size) ()) + 1;

    for (int i = 0; i < cell_size - filled; ++i)
    {
        append_c (strp, c);
    }
}

static void
append_rem_space (char **strp, int filled)
{
    append_rem_space_with_c (strp, filled, ' ');
}

static void
append_num_cell (char **strp, int num)
{
    if (!num)
    {
        append_rem_space (strp, 0);
        return;
    }

    int filled = calc_log (num);
    char *swp;
    int res;

    swp = *strp;
    res = asprintf (strp, "%s%d", *strp, num);

    if (res == -1)
    {
        abort ();
    }

    append_rem_space (strp, filled);

    free (swp);
}

static void
append_perimeter (char **strp, UTL_S (building_t) *perimeter)
{
    char *swp;
    int res;

    for (int i = 0; i < UTL_S (perimeter_size) (); ++i)
    {
        swp = *strp;
        if (i)
        {
            res = asprintf (strp, "%s,%d", *strp, perimeter[i]);
        }
        else
        {
            res = asprintf (strp, "%s%d", *strp, perimeter[i]);
        }

        if (res == -1)
        {
            abort ();
        }

        free (swp);
    }
}

static void
append_buildings (char **strp, UTL_S (building_t) *buildings)
{
    char *swp;
    int res;

    for (int i = 0; i < UTL_S (buildings_size) (); ++i)
    {
        swp = *strp;

        if (i)
        {
            res = asprintf (strp, "%s,%d", *strp, buildings[i]);
        }
        else
        {
            res = asprintf (strp, "%s%d", *strp, buildings[i]);
        }

        if (res == -1)
        {
            abort ();
        }

        free (swp);
    }
}

void
S (print_island) (FILE *stream, UTL_S (building_t) *perimeter,
        UTL_S (building_t) *buildings, int print_raw, int indent)
{
    char *str = calloc (1, sizeof (*str));

    if (!str)
    {
        abort ();
    }

    if (print_raw)
    {
        append_indent (&str, indent);
        append_s (&str, "perimeter raw seq: ");
        append_perimeter (&str, perimeter);
        append_c (&str, '\n');

        append_indent (&str, indent);
        append_s (&str, "buildings raw seq: ");
        append_buildings (&str, buildings);
        append_c (&str, '\n');
    }

    append_indent (&str, indent);
    append_rem_space (&str, 0);
    append_rem_space (&str, 0);
    for (int i = 0; i < UTL_S (general_size) (); ++i)
    {
        append_num_cell (&str, perimeter[UTL_S (north_obs_idx) (i)]);
    }
    append_c (&str, '\n');

    append_indent (&str, indent);
    append_rem_space (&str, 0);
    append_c (&str, '+');
    append_rem_space_with_c (&str, 1, '-');
    for (int i = 0; i < UTL_S (general_size) (); ++i)
    {
        append_rem_space_with_c (&str, 0, '-');
    }
    append_c (&str, '+');
    append_rem_space (&str, 1);
    append_c (&str, '\n');

    for (int j = 0; j < UTL_S (general_size) (); ++j)
    {
        append_indent (&str, indent);
        append_num_cell (&str, perimeter[UTL_S (west_obs_idx) (j)]);
        append_c (&str, '|');
        append_rem_space (&str, 1);
        for (int i = 0; i < UTL_S (general_size) (); ++i)
        {
            append_num_cell (&str, buildings[UTL_S (building_idx) (j, i)]);
        }
        append_c (&str, '|');
        append_rem_space (&str, 1);
        append_num_cell (&str, perimeter[UTL_S (east_obs_idx) (j)]);
        append_c (&str, '\n');
    }

    append_indent (&str, indent);
    append_rem_space (&str, 0);
    append_c (&str, '+');
    append_rem_space_with_c (&str, 1, '-');
    for (int i = 0; i < UTL_S (general_size) (); ++i)
    {
        append_rem_space_with_c (&str, 0, '-');
    }
    append_c (&str, '+');
    append_rem_space (&str, 1);
    append_c (&str, '\n');

    append_indent (&str, indent);
    append_rem_space (&str, 0);
    append_rem_space (&str, 0);
    for (int i = 0; i < UTL_S (general_size) (); ++i)
    {
        append_num_cell (&str, perimeter[UTL_S (south_obs_idx) (i)]);
    }

    fprintf (stream, "%s\n", str);

    free (str);
}

void
S (print_island_delim) (FILE *stream, int print_raw __attribute__ ((unused)),
        int indent)
{
    char *str = calloc (1, sizeof (*str));

    if (!str)
    {
        abort ();
    }

    append_indent (&str, indent);

    fprintf (stream, "%s\n", str);

    free (str);
}

// vi:ts=4:sw=4:et
