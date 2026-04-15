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

c_variant_generator_script_depend_files = files(
    'src/base.ts',
    'src/data.ts',
    'src/utils.ts',
    'src/variants.ts',
    'index.ts',
)

c_variant_generator_script_depends = []

meson_wrapper_sh = find_program(meson.current_source_dir() / 'meson_wrapper.sh')

install_dependencies = custom_target(
    'install dependencies (c_variant_generator script)',
    command: [
        meson_wrapper_sh,
        '--wrapper-generate', '@OUTPUT0@',
        '--link-file', '@CURRENT_SOURCE_DIR@/pnpm-lock.yaml',
        '@OUTPUT1@',
        pnpm,
        '--dir', meson.current_source_dir(),
        'exec',
        pnpm,
        'install',
        '--loglevel', 'error',
    ],
    output: ['install_dependencies_date.txt', 'pnpm-lock-link.yaml'],
    build_subdir: 'generated' / 'pseudo',
    depend_files: ['package.json'],
)
c_variant_generator_script_depends += install_dependencies

type_check_helper_script = custom_target(
    'typecheck code (c_variant_generator script)',
    command: [
        meson_wrapper_sh,
        '--wrapper-generate', '@OUTPUT@',
        pnpm,
        '--dir', meson.current_source_dir(),
        'exec',
        'tsc',
        '-p', 'tsconfig.json',
    ],
    output: ['typecheck_code_date.txt'],
    build_subdir: 'generated' / 'pseudo',
    depend_files: ['tsconfig.json'],
    depends: [install_dependencies],
)
c_variant_generator_script_depends += type_check_helper_script

lint_helper_script = custom_target(
    'lint code (c_variant_generator script)',
    command: [
        meson_wrapper_sh,
        '--wrapper-generate', '@OUTPUT@',
        pnpm,
        '--dir', meson.current_source_dir(),
        'run',
        'lint',
    ],
    output: ['lint_code_date.txt'],
    build_subdir: 'generated' / 'pseudo',
    depend_files: c_variant_generator_script_depend_files,
    depends: [install_dependencies],
)
c_variant_generator_script_depends += lint_helper_script

build_helper_script = custom_target(
    'build typescript code (c_variant_generator script)',
    command: [
        meson_wrapper_sh,
        '--wrapper-generate', '@OUTPUT@',
        pnpm,
        '--dir', meson.current_source_dir(),
        'run',
        'build',
    ],
    output: ['build_code_date.txt'],
    build_subdir: 'generated' / 'pseudo',
    depend_files: c_variant_generator_script_depend_files,
    depends: [install_dependencies],
)

c_variant_generator_script_depends += build_helper_script

node = find_program(
    'node',
    required: false,
    version: '>=24.0.0', # for .ts execution
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

generate_program_sh = find_program(meson.current_source_dir() / 'generate_program.sh')

c_variant_generator_file = custom_target(
    command: [
        generate_program_sh,
        node_like_runtime,
        '@CURRENT_SOURCE_DIR@',
        '@OUTPUT@',
    ],
    output: ['c_variant_generator_wrapper.sh'],
    depend_files: c_variant_generator_script_depend_files,
    depends: c_variant_generator_script_depends,
)

c_variant_generator_file_path = c_variant_generator_file.full_path()

fs = import('fs')

fs.(c_variant_generator_file_path)

c_variant_generator_prog = find_program(c_variant_generator_file_path)

meson.override_find_program('c_variant_generator', c_variant_generator_prog)
