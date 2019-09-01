#pragma push_macro ("S")
#define S(name) island_lab_visual_ ## name

void
S (print_island) (FILE *stream, int *perimeter, int *buildings, int print_raw,
    int indent);

void
S (print_island_delim) (FILE *stream, int print_raw, int indent);

#pragma pop_macro ("S")
// vi:ts=4:sw=4:et
