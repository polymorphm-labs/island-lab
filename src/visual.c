#define _GNU_SOURCE

#include "island-lab-config.h"

// abort
#include <stdlib.h>

// fputc fprintf
#include <stdio.h>

#include "util.h"
#include "visual.h"

#define UTL_S(name) island_lab_util_ ## name
#define S(name) island_lab_visual_ ## name

static void
print_indent (FILE *stream, int indent)
{
    for (int i = 0; i < indent; ++i)
    {
        fputc (' ', stream);
    }
}

static int
calc_log (int num)
{
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
print_rem_space_with_c (FILE *stream, int filled, char c)
{
    int cell_size = calc_log (UTL_S (general_size) ()) + 1;

    for (int i = 0; i < cell_size - filled; ++i)
    {
        fputc (c, stream);
    }
}

static void
print_rem_space (FILE *stream, int filled)
{
    print_rem_space_with_c (stream, filled, ' ');
}

static void
print_num_cell (FILE *stream, int num)
{
    if (!num)
    {
        print_rem_space (stream, 0);
        return;
    }

    int filled = calc_log (num);

    fprintf (stream, "%d", num);
    print_rem_space (stream, filled);
}

void
S (print_island) (FILE *stream, int *perimeter, int *buildings, int print_raw,
        int indent)
{
    if (print_raw) {
        print_indent (stream, indent);
        fprintf (stream, "perimeter raw seq: ");
        for (int i = 0; i < UTL_S (perimeter_size) (); ++i)
        {
            if (i)
            {
                fputc (',', stream);
            }
            fprintf (stream, "%d", perimeter[i]);
        }
        fputc ('\n', stream);

        print_indent (stream, indent);
        fprintf (stream, "buildings raw seq: ");
        for (int i = 0; i < UTL_S (buildings_size) (); ++i)
        {
            if (i)
            {
                fputc (',', stream);
            }
            fprintf (stream, "%d", buildings[i]);
        }
        fputc ('\n', stream);
    }

    print_indent (stream, indent);
    print_rem_space (stream, 0);
    print_rem_space (stream, 0);
    for (int i = 0; i < UTL_S (general_size) (); ++i)
    {
        print_num_cell (stream, perimeter[UTL_S (north_obs_idx) (i)]);
    }
    fputc ('\n', stream);

    print_indent (stream, indent);
    print_rem_space (stream, 0);
    fputc ('+', stream);
    print_rem_space_with_c (stream, 1, '-');
    for (int i = 0; i < UTL_S (general_size) (); ++i)
    {
        print_rem_space_with_c (stream, 0, '-');
    }
    fputc ('+', stream);
    print_rem_space (stream, 1);
    fputc ('\n', stream);

    for (int j = 0; j < UTL_S (general_size) (); ++j)
    {
        print_indent (stream, indent);
        print_num_cell (stream, perimeter[UTL_S (west_obs_idx) (j)]);
        fputc ('|', stream);
        print_rem_space (stream, 1);
        for (int i = 0; i < UTL_S (general_size) (); ++i)
        {
            print_num_cell (stream, buildings[UTL_S (building_idx) (j, i)]);
        }
        fputc ('|', stream);
        print_rem_space (stream, 1);
        print_num_cell (stream, perimeter[UTL_S (east_obs_idx) (j)]);
        fputc ('\n', stream);
    }

    print_indent (stream, indent);
    print_rem_space (stream, 0);
    fputc ('+', stream);
    print_rem_space_with_c (stream, 1, '-');
    for (int i = 0; i < UTL_S (general_size) (); ++i)
    {
        print_rem_space_with_c (stream, 0, '-');
    }
    fputc ('+', stream);
    print_rem_space (stream, 1);
    fputc ('\n', stream);

    print_indent (stream, indent);
    print_rem_space (stream, 0);
    print_rem_space (stream, 0);
    for (int i = 0; i < UTL_S (general_size) (); ++i)
    {
        print_num_cell (stream, perimeter[UTL_S (south_obs_idx) (i)]);
    }
    fputc ('\n', stream);
}

void
S (print_island_delim) (FILE *stream, int print_raw __attribute__ ((unused)),
        int indent)
{
    print_indent (stream, indent);
    fputc ('\n', stream);
}

// vi:ts=4:sw=4:et
