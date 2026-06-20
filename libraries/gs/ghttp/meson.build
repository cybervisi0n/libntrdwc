libdwcgsghttp = library('dwcgsghttp',
    dependencies: [nitrosdk_dep, nitrosystem_dep, nitrowifi_dep],
    include_directories: nitrodwc_includes,
    pic: build_target == 'nx',
    sources: files(
        'ghttpBuffer.c',
        'ghttpCallbacks.c',
        'ghttpCommon.c',
        'ghttpConnection.c',
        'ghttpEncryption.c',
        'ghttpMain.c',
        'ghttpPost.c',
        'ghttpProcess.c'
    ),
    native: (build_target == 'linux' or build_target == 'win64')
)

nitrodwc_libs += libdwcgsghttp
