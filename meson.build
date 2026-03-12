spl_sources = files(
  'ast.c',
  'json.c',
  'parser.c',
  'rule.c',
  'utils.c'
)

# This adds 'lib/' to the include path, so that #include <spl/ast.h> works
spl_inc = include_directories('..')

# This library is only used within this project
spl_lib = static_library('spl',
  spl_sources,
  include_directories : spl_inc,
  install : false
)

# Create a dependency object that includes the library and the include paths
spl_dep = declare_dependency(
  link_with : spl_lib,
  include_directories : spl_inc
)

# Example executable
spl_example_exe = executable('spl_example',
  'examples/main.c',
  dependencies : spl_dep,
  include_directories : include_directories('.')
)

# Test for the example
test('spl_example_test', spl_example_exe,
  timeout : 5
)
