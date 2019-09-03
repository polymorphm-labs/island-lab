#pragma push_macro ("S")
#pragma push_macro ("CFG_M")
#define CFG_M(name) ISLAND_LAB_CONFIG_ ## name
#define S(name) island_lab_util_ ## name

static inline int
S (general_size) ()
{
    return CFG_M (SIZE);
}

static inline int
S (perimeter_size) ()
{
    return CFG_M (SIZE) * 4;
}

static inline int
S (buildings_size) ()
{
    return CFG_M (SIZE) * CFG_M (SIZE);
}

static inline int
S (north_obs_idx) (int i)
{
    return i;
}

static inline int
S (east_obs_idx) (int j)
{
    return CFG_M (SIZE) + j;
}

static inline int
S (south_obs_idx) (int i)
{
    return CFG_M (SIZE) * 3 - 1 - i;
}

static inline int
S (west_obs_idx) (int j)
{
    return CFG_M (SIZE) * 4 - 1 - j;
}

static inline int
S (building_idx) (int j, int i)
{
    return CFG_M (SIZE) * j + i;
}

static inline int
S (has_north_building) (int j)
{
    return j > 0;
}

static inline int
S (has_east_building) (int i)
{
    return i < CFG_M (SIZE) - 1;
}

static inline int
S (has_south_building) (int j)
{
    return j < CFG_M (SIZE) - 1;
}

static inline int
S (has_west_building) (int i)
{
    return i > 0;
}

#pragma pop_macro ("CFG_M")
#pragma pop_macro ("S")
// vi:ts=4:sw=4:et
