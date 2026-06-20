libdwcgscommon = library('dwcgscommon',
    dependencies: [nitrosdk_dep, nitrosystem_dep, nitrowifi_dep],
    include_directories: nitrodwc_includes,
    pic: build_target == 'nx',
    sources: files(
        '../available.c',
        '../darray.c',
        '../hashtable.c',
        '../md5c.c',
        '../nonport.c',
        '../stringutil.c'
    ),
    native: (build_target == 'linux' or build_target == 'win64')
)

nitrodwc_libs += libdwcgscommon
