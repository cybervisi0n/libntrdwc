if(build_target == 'arm')
    nitrodwc_prebuilt += fs.copyfile('prebuilt' / 'libdwcauth.a')
else
    subdir('src')
    libdwcauth_includes = include_directories('include')
    libdwcauth = library('dwcauth',
        dependencies: [nitrosdk_dep, nitrosystem_dep, nitrowifi_dep],
        include_directories: [nitrodwc_includes, libdwcauth_includes],
        pic: true,
        sources: libdwcauth_srcs,
        native: (build_target == 'linux' or build_target == 'win64')
    )

    nitrodwc_libs += libdwcauth
endif
