project('island-lab', 'c',
        version : 'PRE-RELEASE')

link_argp_opt = get_option('link-argp')
custom_reallocarray_opt = get_option('custom-reallocarray')
no_random_opt = get_option('no-random')
general_size_opt = get_option('general-size')
building_ctype_opt = get_option('building-ctype')
init_buildings_heap_opt = get_option('init-buildings-heap')
default_max_branches_opt = get_option('default-max-branches')

sys_dep = []

if link_argp_opt
  sys_dep += meson.get_compiler('c').find_library('argp')
endif

subdir('src')

# vi:ts=2:sw=2:et
