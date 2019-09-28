#pragma push_macro ("S")
#pragma push_macro ("CFG_M")
#define CFG_M(name) ISLAND_LAB_CONFIG_ ## name
#define S(name) island_lab_util_ ## name

static inline int
S (general_size) (void)
{
    return CFG_M (GENERAL_SIZE);
}

static inline int
S (perimeter_size) (void)
{
    return S (general_size) () * 4;
}

static inline int
S (buildings_size) (void)
{
    return S (general_size) () * S (general_size) ();
}

static inline int
S (north_obs_idx) (int i)
{
    return i;
}

static inline int
S (east_obs_idx) (int j)
{
    return S (general_size) () + j;
}

static inline int
S (south_obs_idx) (int i)
{
    return S (general_size) () * 3 - 1 - i;
}

static inline int
S (west_obs_idx) (int j)
{
    return S (general_size) () * 4 - 1 - j;
}

static inline int
S (building_idx) (int j, int i)
{
    return S (general_size) () * j + i;
}

static inline int
S (hori_building_idx) (int cns, int var)
{
    return S (building_idx) (cns, var);
}

static inline int
S (vert_building_idx) (int cns, int var)
{
    return S (building_idx) (var, cns);
}

static inline int
S (rev_hori_building_idx) (int cns, int var)
{
    return S (building_idx) (cns, S (general_size) () - var - 1);
}

static inline int
S (rev_vert_building_idx) (int cns, int var)
{
    return S (building_idx) (S (general_size) () - var - 1, cns);
}

#pragma pop_macro ("CFG_M")
#pragma pop_macro ("S")
// vi:ts=4:sw=4:et
