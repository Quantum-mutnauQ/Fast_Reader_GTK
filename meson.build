project('FastReader', 'c',
  version: '2',
  default_options: ['warning_level=3'])

# Abhängigkeiten
gtk4_dep = dependency('gtk4')
libconfig_dep = dependency('libconfig')

# Quellen
executable('FastReader', 'FastReader.c',
  dependencies: [gtk4_dep, libconfig_dep])

