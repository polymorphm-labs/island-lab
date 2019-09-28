test_island_lab_resolver_items = [
  {
    'name' : 'resolver regular',
    'exe' : 'test-island-lab-resolver-regular',
    'c_args' : ['-DTEST_FUNCTION_NAME=test_resolver_regular'],
    'sources' : files(
      'test-resolver.c',
    ),
  },
]

test_island_lab_visual_items = [
  {
    'name' : 'visual regular',
    'exe' : 'test-island-lab-visual-regular',
    'c_args' : ['-DTEST_FUNCTION_NAME=test_visual_regular'],
    'sources' : files(
      'test-visual.c',
    ),
  },
  {
    'name' : 'visual with print-raw',
    'exe' : 'test-island-lab-visual-with-print-raw',
    'c_args' : ['-DTEST_FUNCTION_NAME=test_visual_with_print_raw'],
    'sources' : files(
      'test-visual.c',
    ),
  },
]

foreach test_item : test_island_lab_resolver_items
  test_exe = executable(test_item.get('exe'),
		        test_item.get('sources'),
			'../src/resolver.c',
			shared_sources,
			include_directories : '../src',
			c_args : test_item.get('c_args'),
			dependencies : sys_dep)

  test(test_item.get('name'), test_exe, suite : 'resolver')
endforeach

foreach test_item : test_island_lab_visual_items
  test_exe = executable(test_item.get('exe'),
		        test_item.get('sources'),
			'../src/visual.c',
			shared_sources,
			include_directories : '../src',
			c_args : test_item.get('c_args'),
			dependencies : sys_dep)

  test(test_item.get('name'), test_exe, suite : 'visual')
endforeach

# vi:ts=2:sw=2:et
