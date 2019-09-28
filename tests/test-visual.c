#define _GNU_SOURCE

#include "island-lab-config.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "util.h"
#include "visual.h"

#define UTL_S(name) island_lab_util_ ## name
#define VIS_S(name) island_lab_visual_ ## name

static UTL_S (building_t) test_perimeter[] =
        {0,0,0,2,2,0,0,0,0,6,3,0,0,4,0,0,0,0,4,4,0,3,0,0};
static UTL_S (building_t) test_buildings_a[] =
        {5,6,1,4,3,2,4,1,3,2,6,5,2,3,6,1,5,4,6,5,4,3,2,1,1,2,5,6,4,3,3,4,2,5,1,6};
static UTL_S (building_t) test_buildings_b[] =
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static UTL_S (building_t) test_buildings_c[] =
        {0,6,0,4,3,0,0,0,3,2,0,5,2,3,6,0,5,4,6,5,0,0,2,1,0,2,5,6,4,3,3,4,2,5,0,6};
static int test_indent_a = 13;
static int test_indent_b = 17;

static char *exp_visual_str_a =
        "                       2 2   \n"
        "               +-------------+ \n"
        "               | 5 6 1 4 3 2 |   \n"
        "               | 4 1 3 2 6 5 |   \n"
        "             3 | 2 3 6 1 5 4 |   \n"
        "               | 6 5 4 3 2 1 | 6 \n"
        "             4 | 1 2 5 6 4 3 | 3 \n"
        "             4 | 3 4 2 5 1 6 |   \n"
        "               +-------------+ \n"
        "                         4   \n"
        "             \n"
        "                       2 2   \n"
        "               +-------------+ \n"
        "               |             |   \n"
        "               |             |   \n"
        "             3 |             |   \n"
        "               |             | 6 \n"
        "             4 |             | 3 \n"
        "             4 |             |   \n"
        "               +-------------+ \n"
        "                         4   \n"
        "             \n"
        "                       2 2   \n"
        "               +-------------+ \n"
        "               |   6   4 3   |   \n"
        "               |     3 2   5 |   \n"
        "             3 | 2 3 6   5 4 |   \n"
        "               | 6 5     2 1 | 6 \n"
        "             4 |   2 5 6 4 3 | 3 \n"
        "             4 | 3 4 2 5   6 |   \n"
        "               +-------------+ \n"
        "                         4   \n";

static char *exp_visual_str_b =
        "                 perimeter raw seq: 0,0,0,2,2,0,0,0,0,6,3,0,0,4,0,0,0,0,4,4,0,3,0,0\n"
        "                 buildings raw seq: 5,6,1,4,3,2,4,1,3,2,6,5,2,3,6,1,5,4,6,5,4,3,2,1,1,2,5,6,4,3,3,4,2,5,1,6\n"
        "                           2 2   \n"
        "                   +-------------+ \n"
        "                   | 5 6 1 4 3 2 |   \n"
        "                   | 4 1 3 2 6 5 |   \n"
        "                 3 | 2 3 6 1 5 4 |   \n"
        "                   | 6 5 4 3 2 1 | 6 \n"
        "                 4 | 1 2 5 6 4 3 | 3 \n"
        "                 4 | 3 4 2 5 1 6 |   \n"
        "                   +-------------+ \n"
        "                             4   \n"
        "                 \n"
        "                 perimeter raw seq: 0,0,0,2,2,0,0,0,0,6,3,0,0,4,0,0,0,0,4,4,0,3,0,0\n"
        "                 buildings raw seq: 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0\n"
        "                           2 2   \n"
        "                   +-------------+ \n"
        "                   |             |   \n"
        "                   |             |   \n"
        "                 3 |             |   \n"
        "                   |             | 6 \n"
        "                 4 |             | 3 \n"
        "                 4 |             |   \n"
        "                   +-------------+ \n"
        "                             4   \n"
        "                 \n"
        "                 perimeter raw seq: 0,0,0,2,2,0,0,0,0,6,3,0,0,4,0,0,0,0,4,4,0,3,0,0\n"
        "                 buildings raw seq: 0,6,0,4,3,0,0,0,3,2,0,5,2,3,6,0,5,4,6,5,0,0,2,1,0,2,5,6,4,3,3,4,2,5,0,6\n"
        "                           2 2   \n"
        "                   +-------------+ \n"
        "                   |   6   4 3   |   \n"
        "                   |     3 2   5 |   \n"
        "                 3 | 2 3 6   5 4 |   \n"
        "                   | 6 5     2 1 | 6 \n"
        "                 4 |   2 5 6 4 3 | 3 \n"
        "                 4 | 3 4 2 5   6 |   \n"
        "                   +-------------+ \n"
        "                             4   \n";

static int fread_buf_size = 131072;

static void
fread_to_str (FILE *stream, char **strp)
{
    char *buf = calloc (fread_buf_size, sizeof (*buf));

    if (!buf)
    {
        abort ();
    }

    fread (buf, fread_buf_size - 1, sizeof (*buf), stream);

    *strp = buf;
}

int
test_visual_regular ()
{
    int test_result = 1;
    int print_raw = 0;
    int indent = test_indent_a;
    char *str = 0;
    FILE *stream = tmpfile ();

    if (!stream)
    {
        abort ();
    }

    VIS_S (print_island) (stream, test_perimeter, test_buildings_a, print_raw,
            indent);
    VIS_S (print_island_delim) (stream, print_raw,
            indent);
    VIS_S (print_island) (stream, test_perimeter, test_buildings_b, print_raw,
            indent);
    VIS_S (print_island_delim) (stream, print_raw,
            indent);
    VIS_S (print_island) (stream, test_perimeter, test_buildings_c, print_raw,
            indent);

    rewind (stream);

    fread_to_str (stream, &str);

    if (strcmp (exp_visual_str_a, str))
    {
        test_result = 0;
    }

    fclose (stream);
    free (str);

    return test_result;
}

int
test_visual_with_print_raw ()
{
    int test_result = 1;
    int print_raw = 1;
    int indent = test_indent_b;
    char *str = 0;
    FILE *stream = tmpfile ();

    if (!stream)
    {
        abort ();
    }

    VIS_S (print_island) (stream, test_perimeter, test_buildings_a, print_raw,
            indent);
    VIS_S (print_island_delim) (stream, print_raw,
            indent);
    VIS_S (print_island) (stream, test_perimeter, test_buildings_b, print_raw,
            indent);
    VIS_S (print_island_delim) (stream, print_raw,
            indent);
    VIS_S (print_island) (stream, test_perimeter, test_buildings_c, print_raw,
            indent);

    rewind (stream);

    fread_to_str (stream, &str);

    if (strcmp (exp_visual_str_b, str))
    {
        test_result = 0;
    }

    fclose (stream);
    free (str);

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
