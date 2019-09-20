#pragma push_macro ("S")
#define S(name) island_lab_custom_reallocarray_ ## name

void *
S (reallocarray) (void *ptr, size_t nmemb, size_t size);

#pragma pop_macro ("S")
// vi:ts=4:sw=4:et
