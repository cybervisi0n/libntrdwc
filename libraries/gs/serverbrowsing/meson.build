libdwcgsserverbrowsing = library('dwcgsserverbrowsing',
    dependencies: [nitrosdk_dep, nitrosystem_dep, nitrowifi_dep],
    include_directories: nitrodwc_includes,
    pic: build_target == 'nx',
    sources: files(
        'sb_crypt.c',
        'sb_queryengine.c',
        'sb_server.c',
        'sb_serverbrowsing.c',
        'sb_serverlist.c'
    ),
    native: (build_target == 'linux' or build_target == 'win64')
)

nitrodwc_libs += libdwcgsserverbrowsing
