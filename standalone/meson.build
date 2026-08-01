libntrdwc_link_args = []

libntrdwc_exe = executable(
    'libntrdwc',
    sources: files('main.c'),
    dependencies: [ nitrosdk_dep, nitrodwc_dep, nitrowifi_dep, nitrosystem_dep ],
    link_args: [ libntrdwc_link_args ],
    native: build_target != 'arm',
)
