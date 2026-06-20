libdwcgschat = library('dwcgschat',
    dependencies: [nitrosdk_dep, nitrosystem_dep, nitrowifi_dep],
    include_directories: nitrodwc_includes,
    pic: build_target == 'nx',
    sources: files(
        'chatCallbacks.c',
        'chatChannel.c',
        'chatCrypt.c',
        'chatHandlers.c',
        'chatMain.c',
        'chatSocket.c'
    ),
    native: (build_target == 'linux' or build_target == 'win64')
)

nitrodwc_libs += libdwcgschat
