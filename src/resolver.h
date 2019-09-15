#pragma push_macro ("S")
#define S(name) island_lab_resolver_ ## name

struct S (resolve_options)
{
    int max_branches;
    int show_iterations;
    void (*max_branches_exceeded) (void *max_branches_exceeded_data);
    void *max_branches_exceeded_data;
    void (*show_iter) (int iter_made, int branches, void *show_iter_data);
    void *show_iter_data;
};

int
S (resolve) (int *perimeter, int *buildings, int ***resolved_buildingss_ptr,
        struct S (resolve_options) *options);

#pragma pop_macro ("S")
// vi:ts=4:sw=4:et
