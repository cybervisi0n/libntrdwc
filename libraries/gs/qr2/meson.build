libdwcgsqr2 = library('dwcgsqr2',
    dependencies: [nitrosdk_dep, nitrosystem_dep, nitrowifi_dep],
    include_directories: nitrodwc_includes,
    pic: build_target == 'nx',
    sources: files(
        'qr2.c',
        'qr2regkeys.c'
    ),
    native: (build_target == 'linux' or build_target == 'win64')
)

nitrodwc_libs += libdwcgsqr2
