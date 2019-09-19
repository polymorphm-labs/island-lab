project('island-lab', 'c',
        version : 'PRE-1.0')

link_argp_opt = get_option('link-argp')
no_random_opt = get_option('no-random')

sys_dep = []

if link_argp_opt
  sys_dep += meson.get_compiler('c').find_library('argp')
endif

subdir('src')

# vi:ts=2:sw=2:et
