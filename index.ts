import { fsAsyncExists, getThisPackageFile } from "./src/utils.js";
import path from "node:path"
import { outputVariantJsonSchema } from "./src/data.js"
import { generateVariantCodeC } from "./src/variants.js";



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


interface GenerateOptions {
    output: string,
    input: string
}


async function main(): Promise<void> {

    const options: Partial<GenerateOptions> = {}

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

            options.input = input
            ++i
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

    await generateVariantCodeC(options.output, options.input)


}


void main()
