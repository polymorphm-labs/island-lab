project('island-lab', 'c', version : 'PRE-RELEASE')

cc = meson.get_compiler('c')

link_argp_opt = get_option('link-argp')
general_size_opt = get_option('general-size')
building_ctype_opt = get_option('building-ctype')
init_buildings_heap_opt = get_option('init-buildings-heap')
default_max_branches_opt = get_option('default-max-branches')
tests_opt = get_option('tests')

have_reallocarray = cc.has_function('reallocarray',
                                    prefix : '#define _GNU_SOURCE\n#include <stdlib.h>')
have_getrandom = cc.has_function('getrandom',
                                 prefix : '#define _GNU_SOURCE\n#include <sys/random.h>')

custom_reallocarray_opt = not have_reallocarray
no_random_opt = not have_getrandom

sys_dep = []

if link_argp_opt
  sys_dep += cc.find_library('argp')
endif

subdir('src')

if tests_opt
  subdir('tests')
endif

# vi:ts=2:sw=2:et
