#define _GNU_SOURCE

#include "island-lab-config.h"

// abort free *alloc* EXIT_* atoi
#include <stdlib.h>

// strlen strdup strtok_r
#include <string.h>

// fprintf stdout stderr scanf
#include <stdio.h>

// argp_*
#include <argp.h>

#ifdef ISLAND_LAB_CONFIG_CUSTOM_REALLOCARRAY
#include "custom-reallocarray.h"
#include "custom-reallocarray-compat.h"
#endif

#include "git-rev.h"
#include "util.h"
#include "visual.h"

#define CFG_M(name) ISLAND_LAB_CONFIG_ ## name
#define GIT_S(name) island_lab_git_ ## name
#define UTL_S(name) island_lab_util_ ## name
#define VIS_S(name) island_lab_visual_ ## name

static void
argp_print_version (FILE *stream,
        struct argp_state *state __attribute__ ((unused)))
{
    fprintf (stream, "%s visual %s (%s)\n",
            CFG_M (NAME), CFG_M (VERSION), GIT_S (rev));
}

void (*argp_program_version_hook) (FILE *, struct argp_state *) =
        argp_print_version;

struct arguments
{
    int print_raw;
    int indent;
    int buildings_str_count;
    char *perimeter_str;
    char **buildings_strs;
};

static struct argp_option argp_options[] =
{
    {
        .name = "print-raw",
        .key = 'r',
        .doc = "Print input sequence strings in raw format besides "
                "visualisation",
    },
    {
        .name = "indent",
        .key = 'i',
        .arg = "INDENT-SIZE",
        .doc = "Indentation size",
    },
    {},
};

static error_t
argp_parser (int key, char *arg, struct argp_state *state)
{
    struct arguments *arguments = state->input;

    switch (key)
    {
        case 'r':
            arguments->print_raw = 1;
            break;

        case 'i':
            arguments->indent = atoi (arg);
            break;

        case ARGP_KEY_ARG:
            switch (state->arg_num)
            {
                case 0:
                    arguments->perimeter_str = strdup (arg);
                    break;

                default:
                    ++arguments->buildings_str_count;
                    arguments->buildings_strs = reallocarray (
                            arguments->buildings_strs,
                            arguments->buildings_str_count,
                            sizeof (*arguments->buildings_strs));

                    if (!arguments->buildings_strs)
                    {
                        abort ();
                    }

                    arguments->buildings_strs
                            [arguments->buildings_str_count - 1]
                            = strdup (arg);
            }

            break;

        case ARGP_KEY_END:
            if (state->arg_num < 1)
            {
                argp_error (state, "too few arguments");
                return EINVAL;
            }

            break;

        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

static struct argp argp =
{
    .options = argp_options,
    .parser = argp_parser,
    .args_doc =
            "PERIMETER-SEQUENCE [BUILDINGS-SEQUENCE [BUILDINGS-SEQUENCE [...]]]",
};

int
main (int argc, char *argv[])
{
    struct arguments arguments = {};

    argp_parse (&argp, argc, argv, 0, 0, &arguments);

    int exit_code = EXIT_SUCCESS;
    FILE *stream = stdout;
    UTL_S (building_t) *perimeter = calloc (UTL_S (perimeter_size) (),
            sizeof (*perimeter));
    UTL_S (building_t) *buildings = calloc (UTL_S (buildings_size) (),
            sizeof (*buildings));
    int token_i;
    char *token;
    char *tok_state;
    int token_int;

    if (!arguments.perimeter_str || !perimeter || !buildings)
    {
        abort ();
    }

    if (strcmp (arguments.perimeter_str, "-") == 0)
    {
        free (arguments.perimeter_str);
        scanf ("%ms", &arguments.perimeter_str);

        if (!arguments.perimeter_str)
        {
            fprintf (stderr,
                    "missing input for perimeter sequence\n");
            exit_code = EXIT_FAILURE;
            goto exit;
        }
    }

    for (token_i = 0,
            token = strtok_r (arguments.perimeter_str, ",", &tok_state);
            token; token = strtok_r (0, ",", &tok_state))
    {
        ++token_i;

        if (token_i > UTL_S (perimeter_size) ())
        {
            fprintf (stderr,
                    "too long perimeter sequence: %d\n", token_i);
            exit_code = EXIT_FAILURE;
            goto exit;
        }

        token_int = atoi (token);

        if (token_int < 0)
        {
            fprintf (stderr,
                    "too small perimeter sequence element: %d\n", token_int);
            exit_code = EXIT_FAILURE;
            goto exit;
        }

        if (token_int > UTL_S (general_size) ())
        {
            fprintf (stderr,
                    "too big perimeter sequence element: %d\n", token_int);
            exit_code = EXIT_FAILURE;
            goto exit;
        }

        perimeter[token_i - 1] = token_int;
    }

    if (token_i < UTL_S (perimeter_size) ())
    {
        fprintf (stderr,
                "too short perimeter sequence: %d\n", token_i);
        exit_code = EXIT_FAILURE;
        goto exit;
    }

    if (!arguments.buildings_str_count)
    {
        VIS_S (print_island) (stream, perimeter, buildings,
                arguments.print_raw, arguments.indent);
        goto exit;
    }

    for (int i = 0; i < arguments.buildings_str_count; ++i)
    {
        if (!arguments.buildings_strs[i])
        {
            abort ();
        }

        if (strcmp (arguments.buildings_strs[i], "-") == 0)
        {
            free (arguments.buildings_strs[i]);
            scanf ("%ms", &arguments.buildings_strs[i]);

            if (!arguments.buildings_strs[i])
            {
                fprintf (stderr,
                        "missing input for buildings sequence\n");
                exit_code = EXIT_FAILURE;
                goto exit;
            }
        }

        if (i)
        {
            VIS_S (print_island_delim) (stream, arguments.print_raw,
                    arguments.indent);
        }

        for (token_i = 0,
                token = strtok_r (arguments.buildings_strs[i], ",", &tok_state);
                token; token = strtok_r (0, ",", &tok_state))
        {
            ++token_i;

            if (token_i > UTL_S (buildings_size) ())
            {
                fprintf (stderr,
                        "too long buildings sequence: %d\n", token_i);
                exit_code = EXIT_FAILURE;
                goto exit;
            }

            token_int = atoi (token);

            if (token_int < 0)
            {
                fprintf (stderr,
                        "too small buildings sequence element: %d\n", token_int);
                exit_code = EXIT_FAILURE;
                goto exit;
            }

            if (token_int > UTL_S (general_size) ())
            {
                fprintf (stderr,
                        "too big buildings sequence element: %d\n", token_int);
                exit_code = EXIT_FAILURE;
                goto exit;
            }

            buildings[token_i - 1] = token_int;
        }

        if (token_i < UTL_S (buildings_size) ())
        {
            fprintf (stderr, "too short buildings sequence: %d\n", token_i);
            exit_code = EXIT_FAILURE;
            goto exit;
        }

        VIS_S (print_island) (stream, perimeter, buildings,
                arguments.print_raw, arguments.indent);
    }

exit:
    free (buildings);
    free (perimeter);
    for (int i = 0; i < arguments.buildings_str_count; ++i)
    {
        free (arguments.buildings_strs[i]);
    }
    free (arguments.buildings_strs);
    free (arguments.perimeter_str);

    return exit_code;
}

// vi:ts=4:sw=4:et
