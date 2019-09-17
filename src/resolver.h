#pragma push_macro ("S")
#define S(name) island_lab_resolver_ ## name

enum
{
    S (no_error),
    S (max_branches_exceeded_error),
};

struct S (resolve_options)
{
    int init_buildings_heap;
    int max_branches;
    int show_iterations;
    void (*show_iter) (int iter_made, int branches, void *show_iter_data);
    void *show_iter_data;
};

int
S (resolve) (int *perimeter, int *buildings, int ***resolved_buildingss_ptr,
        struct S (resolve_options) *options, int *error_ptr);

#pragma pop_macro ("S")
// vi:ts=4:sw=4:et
