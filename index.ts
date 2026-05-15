import { fsAsyncExists, getThisPackageFile } from "./src/utils.js";
import path from "node:path"
import { outputVariantJsonSchema } from "./src/data.js"
import { generateVariantCodeC, type ScriptInfo } from "./src/variants.js";
import fsAsync from "node:fs/promises"


async function isCallingThisScript(value: string): Promise<boolean> {

    try {

        const actualPath: string = path.resolve(value)

        const packageJson = path.join(actualPath, "package.json")

        if (!(await fsAsyncExists(packageJson))) {
            return false;
        }

        const thisPackageJson = await getThisPackageFile()

        return thisPackageJson == packageJson;
    } catch (_err) {
        return false;
    }
}


async function getScriptVersion(): Promise<string> {

    const packageFile = await getThisPackageFile();

    const packageFileContent = (await fsAsync.readFile(packageFile)).toString();

    const packageJson: Record<string, unknown> = JSON.parse(packageFileContent) as Record<string, unknown>

    const version = packageJson.version

    if (typeof version !== "string" || !version) {
        throw new Error(`Version of package.json is invalid`)
    }

    return version;

}

interface GenerateOptions {
    output: string,
    input: string

}


async function main(): Promise<void> {

    const options: Partial<GenerateOptions> & { info: Partial<ScriptInfo> } = { info: {} }

    for (let i = 0; i < process.argv.length; ++i) {
        // eslint-disable-next-line @typescript-eslint/no-non-null-assertion
        const value = process.argv[i]!

        if (value.endsWith('deno') || value.endsWith('node') || value.endsWith('bun')) {
            continue
        }

        if (value.endsWith('.js')) {
            continue
        }

        if (value.endsWith('.ts')) {
            continue
        }

        if (await isCallingThisScript(value)) {
            continue
        }

        if (value == '--variant-json-schema') {
            outputVariantJsonSchema()
            return;
        }


        if (value == '-o' || value == '--output') {
            if (i + 1 >= process.argv.length) {
                throw new Error(
                    `Expected another argument for the output argument`
                )
            }


            // eslint-disable-next-line @typescript-eslint/no-non-null-assertion
            const output = path.resolve(process.argv[i + 1]!)

            if (options.output) {
                throw new Error(
                    `Duplicate output argument`
                )
            }

            options.output = output
            ++i
            continue
        }

        if (value == '-i' || value == '--input') {
            if (i + 1 >= process.argv.length) {
                throw new Error(
                    `Expected another argument for the input argument`
                )
            }


            // eslint-disable-next-line @typescript-eslint/no-non-null-assertion
            const input = path.resolve(process.argv[i + 1]!)

            if (options.input) {
                throw new Error(
                    `Duplicate output argument`
                )
            }

            options.input = input
            ++i
            continue
        }

        if (!options.info.version) {
            options.info.version = value
            continue
        }

        if (!options.info.commit) {
            options.info.commit = value
            continue
        }


        throw new Error(`Unrecognized argument: ${value}`)

    }

    if (!options.output) {
        throw new Error(`No output given`)
    }

    if (!options.input) {
        throw new Error(`No input given`)
    }

    if (!options.info.version) {
        throw new Error(`No version given`)
    }

    if (!options.info.commit) {
        throw new Error(`No commit given`)
    }

    const scriptVersion = await getScriptVersion();


    if (options.info.version != scriptVersion) {
        throw new Error(`Meson and package.json versions differ: ${options.info.version} != ${scriptVersion}`)
    }

    await generateVariantCodeC(options.output, options.input, options.info as ScriptInfo)

}


void main()
