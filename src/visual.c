// abort
#include <stdlib.h>

// fputc fprintf
#include <stdio.h>

#include "island-lab-config.h"
#include "visual.h"

#define CFG_M(name) ISLAND_LAB_CONFIG_ ## name
#define VIS_S(name) island_lab_
#define S(name) island_lab_visual_ ## name

#define PERIMETER_SIZE (CFG_M (SIZE) * 4)
#define BUILDINGS_SIZE (CFG_M (SIZE) * CFG_M (SIZE))

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
    int cell_size = calc_log (CFG_M (SIZE)) + 1;

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
        for (int i = 0; i < PERIMETER_SIZE; ++i)
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
        for (int i = 0; i < BUILDINGS_SIZE; ++i)
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

    for (int i = 0; i < CFG_M (SIZE); ++i)
    {
        print_num_cell (stream, perimeter[i]);
    }

    fputc ('\n', stream);

    for (int j = -1; j < CFG_M (SIZE) + 1; ++j)
    {
        print_indent (stream, indent);

        if (j == -1 || j == CFG_M (SIZE))
        {
            print_rem_space (stream, 0);

            fputc ('+', stream);
            print_rem_space_with_c (stream, 1, '-');

            for (int i = 0; i < CFG_M (SIZE); ++i)
            {
                print_rem_space_with_c (stream, 0, '-');
            }

            fputc ('+', stream);
            print_rem_space (stream, 1);
        }
        else
        {
            print_num_cell (stream, perimeter[CFG_M (SIZE) * 4 - 1 - j]);

            fputc ('|', stream);
            print_rem_space (stream, 1);

            for (int i = 0; i < CFG_M (SIZE); ++i)
            {
                print_num_cell (stream, buildings[CFG_M (SIZE) * j + i]);
            }

            fputc ('|', stream);
            print_rem_space (stream, 1);

            print_num_cell (stream, perimeter[CFG_M (SIZE) + j]);
        }

        fputc ('\n', stream);
    }

    print_indent (stream, indent);
    print_rem_space (stream, 0);
    print_rem_space (stream, 0);

    for (int i = 0; i < CFG_M (SIZE); ++i)
    {
        print_num_cell (stream, perimeter[CFG_M (SIZE) * 3 - 1 - i]);
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
