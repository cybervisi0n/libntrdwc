subdir('src')

libdwcaccount = library('dwcaccount',
    dependencies: [nitrosdk_dep, nitrosystem_dep, nitrowifi_dep],
    include_directories: nitrodwc_includes,
    pic: false,
    sources: libdwcaccount_srcs,
    native: build_target != 'arm'
)

nitrodwc_libs += libdwcaccount
