conf_data = configuration_data()
conf_data.set_quoted('ISLAND_LAB_CONFIG_NAME', meson.project_name())
conf_data.set_quoted('ISLAND_LAB_CONFIG_VERSION', meson.project_version())
conf_data.set('ISLAND_LAB_CONFIG_CUSTOM_REALLOCARRAY',
              custom_reallocarray_opt)
conf_data.set('ISLAND_LAB_CONFIG_NO_RANDOM', no_random_opt)
conf_data.set('ISLAND_LAB_CONFIG_GENERAL_SIZE', general_size_opt)
conf_data.set('ISLAND_LAB_CONFIG_BUILDING_CTYPE', building_ctype_opt)
conf_data.set('ISLAND_LAB_CONFIG_INIT_BUILDINGS_HEAP',
              init_buildings_heap_opt)
conf_data.set('ISLAND_LAB_CONFIG_DEFAULT_MAX_BRANCHES',
              default_max_branches_opt)
configure_file(output : 'island-lab-config.h',
               configuration : conf_data)

git_rev_c = vcs_tag(command : ['git', 'describe', '--always', '--long',
                               '--dirty'],
                    input : 'git-rev.c.in',
                    output : 'git-rev.c')

shared_sources = []

if custom_reallocarray_opt
  shared_sources += files('custom-reallocarray.c')
endif

resolver_sources = [
  files(
    'resolver-main.c',
    'resolver.c',
  ),
  git_rev_c,
] + shared_sources

visual_sources = [
  files(
    'visual-main.c',
    'visual.c',
  ),
  git_rev_c,
] + shared_sources

executable('island-lab-resolver', resolver_sources,
           dependencies : sys_dep,
           install : true)

executable('island-lab-visual', visual_sources,
           dependencies : sys_dep,
           install : true)

# vi:ts=2:sw=2:et
