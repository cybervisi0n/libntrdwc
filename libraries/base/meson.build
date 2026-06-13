subdir('src')

libdwcbase = library('dwcbase',
    dependencies: [nitrosdk_dep, nitrosystem_dep, nitrowifi_dep],
    include_directories: nitrodwc_includes,
    pic: false,
    sources: libdwcbase_srcs,
    native: build_target != 'arm'
)

nitrodwc_libs += libdwcbase
