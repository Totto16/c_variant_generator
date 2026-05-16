src_files += files(
    'generate.c',
    'generate.h',
    'json.c',
    'json.h',
    'main.c',
    'utils.h',
)

inc_dirs += include_directories('.')

stdc_version = cc.get_define('__STDC_VERSION__ ')
stdc_version_int = stdc_version.replace('L', '').to_int()
if stdc_version_int >= 202000
    compile_flags += '-D_C_VARIANT_GENERATOR_COMPILE_WITH_NARROWED_ENUMS'
endif


git = find_program('git')

vcs_dep = vcs_tag(
    command: [git, 'describe', '--tags', '--always', '--abbrev=12'],
    input: files('git_version.h.in'),
    output: 'git_version.h',
    replace_string: '@GIT_COMMIT_HASH@',
)

src_files += vcs_dep
