# lib/spl/meson.build

spl_sources = files(
  'ast.c',
  'json.c',
  'parser.c',
  'rule.c',
  'utils.c'
)

# Build as a static library, following makefile convention
spl_lib = static_library('spl',
  spl_sources,
  include_directories : include_directories('.')
)

# Provide a dependency for other parts of the project
spl_dep = declare_dependency(
  link_with : spl_lib,
  include_directories : include_directories('..')
)

# Optional: build the example program found in makefile
executable('spl-example',
  files('examples/main.c'),
  dependencies : spl_dep
)
