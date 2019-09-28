#define _GNU_SOURCE

#include "island-lab-config.h"

// abort free *alloc* EXIT_* atoi
#include <stdlib.h>

// strlen strdup strtok_r
#include <string.h>

// fprintf stdout stderr scanf asprintf
#include <stdio.h>

// argp_*
#include <argp.h>

#ifndef ISLAND_LAB_CONFIG_NO_RANDOM
// getrandom
#include <sys/random.h>
#endif

#ifdef ISLAND_LAB_CONFIG_CUSTOM_REALLOCARRAY
#include "custom-reallocarray.h"
#include "custom-reallocarray-compat.h"
#endif

#include "git-rev.h"
#include "util.h"
#include "resolver.h"

#define CFG_M(name) ISLAND_LAB_CONFIG_ ## name
#define GIT_S(name) island_lab_git_ ## name
#define UTL_S(name) island_lab_util_ ## name
#define RSL_S(name) island_lab_resolver_ ## name
#define ENV_NAME(name) ("ISLAND_LAB_RESOLVER_" name)

static void
argp_print_version (FILE *stream,
        struct argp_state *state __attribute__ ((unused)))
{
    fprintf (stream, "%s resolver %s (%s)\n",
            CFG_M (NAME), CFG_M (VERSION), GIT_S (rev));
}

void (*argp_program_version_hook) (FILE *, struct argp_state *) =
        argp_print_version;

struct arguments
{
    int do_continue;
    int single;
};

static struct argp_option argp_options[] =
{
    {
        .name = "continue",
        .key = 'c',
        .doc = "continue resolving using explicit passed buildings-sequence",
    },
#ifndef ISLAND_LAB_CONFIG_NO_RANDOM
    {
        .name = "single",
        .key = '1',
        .doc = "return no more than single solution, randomly selected "
                "from all resolved",
    },
#endif
    {},
};

static error_t
argp_parser (int key, char *arg __attribute__ ((unused)),
        struct argp_state *state)
{
    struct arguments *arguments = state->input;

    switch (key)
    {
        case 'c':
            arguments->do_continue = 1;
            break;

        case '1':
            arguments->single = 1;
            break;

        case ARGP_KEY_ARG:
            argp_error (state, "too many arguments");
            return EINVAL;

        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

static struct argp argp =
{
    .options = argp_options,
    .parser = argp_parser,
};

struct resolve_hook_options
{
    FILE *stream;
};

static void
show_iter (int iter_made, int branches, void *show_iter_data)
{
    struct resolve_hook_options *options = show_iter_data;
    FILE *stream = options->stream;

    fprintf (stream, "iteration %d: %d branches\n", iter_made, branches);
}

#ifndef ISLAND_LAB_CONFIG_NO_RANDOM
static int
select_single (int resolved)
{
    unsigned rnd;

    for (;;)
    {
        ssize_t res = getrandom (&rnd, sizeof (rnd), 0);

        if (res == -1)
        {
            abort ();
        }

        if (res != sizeof (rnd))
        {
            continue;
        }

        break;
    }

    return rnd % resolved;
}
#endif

static void
print_buildings (FILE *stream, UTL_S (building_t) *buildings)
{
    char *str = 0;
    char *swp;
    int res;

    for (int i = 0; i < UTL_S (buildings_size) (); ++i)
    {
        if (i)
        {
            swp = str;
            res = asprintf (&str, "%s,%d", str, buildings[i]);
            free (swp);
        }
        else
        {
            res = asprintf (&str, "%d", buildings[i]);
        }

        if (res == -1)
        {
            abort ();
        }
    }

    fprintf (stream, "%s\n", str);
    free (str);
}

int
main (int argc, char *argv[])
{
    struct arguments arguments = {};

    argp_parse (&argp, argc, argv, 0, 0, &arguments);

    int exit_code = EXIT_SUCCESS;
    char *perimeter_str = 0;
    char *buildings_str = 0;
    UTL_S (building_t) *perimeter = calloc (UTL_S (perimeter_size) (),
            sizeof (*perimeter));
    UTL_S (building_t) *buildings = calloc (UTL_S (buildings_size) (),
            sizeof (*buildings));
    int token_i;
    char *token;
    char *tok_state;
    int token_int;
    int resolved = 0;
    int error;
    UTL_S (building_t) **resolved_buildingss = 0;
    struct resolve_hook_options rsl_hook_options = {
        .stream = stderr,
    };
    struct RSL_S (resolve_options) rsl_options = {
        .init_buildings_heap = CFG_M (INIT_BUILDINGS_HEAP),
        .max_branches = CFG_M (DEFAULT_MAX_BRANCHES),
        .show_iter = show_iter,
        .show_iter_data = &rsl_hook_options,
    };

    if (!perimeter || !buildings)
    {
        abort ();
    }

    scanf ("%ms", &perimeter_str);

    if (!perimeter_str)
    {
        fprintf (stderr, "missing input for perimeter sequence\n");
        exit_code = EXIT_FAILURE;
        goto exit;
    }

    for (token_i = 0,
            token = strtok_r (perimeter_str, ",", &tok_state);
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
                    "too small perimeter sequence element: %d\n",
                    token_int);
            exit_code = EXIT_FAILURE;
            goto exit;
        }

        if (token_int > UTL_S (general_size) ())
        {
            fprintf (stderr,
                    "too big perimeter sequence element: %d\n",
                    token_int);
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

    if (arguments.do_continue)
    {
        scanf ("%ms", &buildings_str);

        if (!buildings_str)
        {
            fprintf (stderr, "missing input for buildings sequence\n");
            exit_code = EXIT_FAILURE;
            goto exit;
        }

        for (token_i = 0,
                token = strtok_r (buildings_str, ",", &tok_state);
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
                        "too small buildings sequence element: %d\n",
                        token_int);
                exit_code = EXIT_FAILURE;
                goto exit;
            }

            if (token_int > UTL_S (general_size) ())
            {
                fprintf (stderr,
                        "too big buildings sequence element: %d\n",
                        token_int);
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
    }

    char *max_branches_str = getenv (ENV_NAME ("MAX_BRANCHES"));
    char *show_iterations_str = getenv (ENV_NAME ("SHOW_ITERATIONS"));

    if (max_branches_str)
    {
        rsl_options.max_branches = atoi (max_branches_str);
    }

    if (show_iterations_str && atoi (show_iterations_str))
    {
        rsl_options.show_iterations = 1;
    }

    resolved = RSL_S (resolve) (perimeter, buildings, &resolved_buildingss,
            &rsl_options, &error);

    if (!resolved)
    {
        if (error)
        {
            switch (error)
            {
                case RSL_S (max_branches_exceeded_error):
                    fprintf (stderr, "max branches exceeded! "
                            "use environment variable %s to increase this limit\n",
                            ENV_NAME ("MAX_BRANCHES"));
                    break;
            }

            exit_code = EXIT_FAILURE;
        }
        goto exit;
    }

#ifndef ISLAND_LAB_CONFIG_NO_RANDOM
    if (arguments.single)
    {
        int i = select_single (resolved);
        print_buildings (stdout, resolved_buildingss[i]);
        goto exit;
    }
#endif

    for (int i = 0; i < resolved; ++i)
    {
        print_buildings (stdout, resolved_buildingss[i]);
    }

exit:
    for (int i = 0; i < resolved; ++i)
    {
        free (resolved_buildingss[i]);
    }
    free (resolved_buildingss);
    free (buildings);
    free (perimeter);
    free (buildings_str);
    free (perimeter_str);

    return exit_code;
}

// vi:ts=4:sw=4:et
