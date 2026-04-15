project(
    'c_variant_generator',
    'c',
    license: 'MIT',
    meson_version: '>=1.10.0',
    default_options: {
        'buildtype': 'release',
        'optimization': '3',
        'warning_level': '3',
        'werror': 'true',
        'b_ndebug': 'if-release',
    },
    version: '1.1.4',
)


pnpm = find_program(
    'pnpm',
    required: true,
)

helper_script_depend_files = files(
    'src/variants/base.ts',
    'src/variants/data.ts',
    'src/variants/index.ts',
    'src/all_cases.ts',
    'src/compression.ts',
    'src/generator.ts',
    'src/hpack.ts',
    'src/log.ts',
    'src/subcommands.ts',
    'src/utils.ts',
    'src/ws_tests.ts',
    'index.ts',
)

helper_script_depends = []

meson_wrapper_sh = find_program(meson.current_source_dir() / 'meson_wrapper.sh')

install_dependencies = custom_target(
    'install dependencies (helper script)',
    command: [
        meson_wrapper_sh,
        '--wrapper-generate',
        '@OUTPUT0@',
        '--link-file',
        '@CURRENT_SOURCE_DIR@/pnpm-lock.yaml',
        '@OUTPUT1@',
        pnpm,
        '--dir',
        meson.current_source_dir(),
        'exec',
        pnpm,
        'install',
        '--loglevel',
        'error',
    ],
    output: ['install_dependencies_date.txt', 'pnpm-lock-link.yaml'],
    build_subdir: 'generated' / 'pseudo',
    depend_files: ['package.json'],
)
helper_script_depends += install_dependencies

type_check_helper_script = custom_target(
    'typecheck code (helper script)',
    command: [
        meson_wrapper_sh,
        '--wrapper-generate',
        '@OUTPUT@',
        pnpm,
        '--dir',
        meson.current_source_dir(),
        'exec',
        'tsc',
        '-p',
        'tsconfig.json',
    ],
    output: ['typecheck_code_date.txt'],
    build_subdir: 'generated' / 'pseudo',
    depend_files: ['tsconfig.json'],
    depends: [install_dependencies],
)
helper_script_depends += type_check_helper_script

lint_helper_script = custom_target(
    'lint code (helper script)',
    command: [
        meson_wrapper_sh,
        '--wrapper-generate',
        '@OUTPUT@',
        pnpm,
        '--dir',
        meson.current_source_dir(),
        'run',
        'lint',
    ],
    output: ['lint_code_date.txt'],
    build_subdir: 'generated' / 'pseudo',
    depend_files: helper_script_depend_files,
    depends: [install_dependencies],
)
helper_script_depends += lint_helper_script

build_helper_script = custom_target(
    'build typescript code (helper script)',
    command: [
        meson_wrapper_sh,
        '--wrapper-generate',
        '@OUTPUT@',
        pnpm,
        '--dir',
        meson.current_source_dir(),
        'run',
        'build',
    ],
    output: ['build_code_date.txt'],
    build_subdir: 'generated' / 'pseudo',
    depend_files: helper_script_depend_files,
    depends: [install_dependencies],
)

helper_script_depends += build_helper_script

node = find_program(
    'node',
    required: false,
    version: '>=24.0.0',  # for .ts execution
)

deno = find_program(
    'deno',
    required: false,
)

bun = find_program(
    'bun',
    required: false,
)

node_like_runtime = disabler()

if node.found()
    node_like_runtime = node
elif deno.found()
    node_like_runtime = deno
elif bun.found()
    node_like_runtime = bun
else
    error('No JS runtime to generate sources')
endif


all_variants_generated_tgt = custom_target(
    'all_variants',
    command: [
        node_like_runtime,
        '@CURRENT_SOURCE_DIR@',
        'generator',
        '--type',
        'c_variants',
        '-o',
        '@OUTPUT@',
        '-i',
        '@INPUT@',
    ],
    input: [meson.project_source_root() / 'src' / 'all.variants.def.json'],
    output: ['all_variants.h'],
    build_subdir: 'generated' / 'c' / 'variants',
    depend_files: helper_script_depend_files,
    depends: helper_script_depends,
)

all_variants_dep = declare_dependency(
    version: meson.project_version(),
    sources: all_variants_generated_tgt,
)


json_variants_generated_tgt = custom_target(
    'json_variants',
    command: [
        node_like_runtime,
        '@CURRENT_SOURCE_DIR@',
        'generator',
        '--type',
        'c_variants',
        '-o',
        '@OUTPUT@',
        '-i',
        '@INPUT@',
    ],
    input: [meson.project_source_root() / 'src' / 'json' / 'json.variants.def.json'],
    output: ['json_variants.h'],
    build_subdir: 'generated' / 'c' / 'variants',
    depend_files: helper_script_depend_files,
    depends: helper_script_depends,
)


json_variants_dep = declare_dependency(
    version: meson.project_version(),
    sources: json_variants_generated_tgt,
)


genrated_variants_generated_tgt = custom_target(
    'generated_variants',
    command: [
        node_like_runtime,
        '@CURRENT_SOURCE_DIR@',
        'generator',
        '--type',
        'c_variants',
        '-o',
        '@OUTPUT@',
        '-i',
        '@INPUT@',
    ],
    input: [meson.current_source_dir() / 'src' / 'generated.variants.def.json'],
    output: ['generated_variants.h'],
    build_subdir: 'generated' / 'c' / 'variants',
    depend_files: helper_script_depend_files,
    depends: helper_script_depends,
)




meson.override_find_program('c_variant_generator')
