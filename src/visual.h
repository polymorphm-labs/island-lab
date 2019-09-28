#pragma push_macro ("UTL_S")
#pragma push_macro ("S")
#define UTL_S(name) island_lab_util_ ## name
#define S(name) island_lab_visual_ ## name

void
S (print_island) (FILE *stream, UTL_S (building_t) *perimeter,
        UTL_S (building_t) *buildings, int print_raw, int indent);

void
S (print_island_delim) (FILE *stream, int print_raw, int indent);

#pragma pop_macro ("S")
#pragma pop_macro ("UTL_S")
// vi:ts=4:sw=4:et
