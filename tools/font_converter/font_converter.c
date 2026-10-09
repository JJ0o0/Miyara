#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char** argv) {
    if (argc != 4) {
        fprintf(
            stderr,
            "Usage: %s <input.psf> <output.c> <font_name>\n",
            argv[0]
        );
        
        return 1;
    }

    const char* input_path = argv[1];
    const char* output_path = argv[2];
    const char* font_name = argv[3];

    FILE* input = fopen(input_path, "rb");
    if (!input) {
        perror("fopen input");
        return 1;
    }

    uint8_t header[4];
    if (fread(header, 1, sizeof(header), input) != sizeof(header)) {
        fprintf(stderr, "Failed to read PSF header\n");
        fclose(input);
        return 1;
    }

    if (header[0] != 0x36 || header[1] != 0x04) {
        fprintf(stderr, "Not a PSF1 font\n");
        fclose(input);
        return 1;
    }

    uint8_t mode = header[2];
    uint8_t charsize = header[3];

    size_t glyph_count = (mode & 0x01) ? 512 : 256;
    size_t output_glyph_count = 128;
    if (output_glyph_count > glyph_count) {
        output_glyph_count = glyph_count;
    }

    FILE* output = fopen(output_path, "w");
    if (!output) {
        perror("fopen output");
        fclose(input);
        return 1;
    }

    fprintf(output, "#include <graphics/font.h>\n\n");
    fprintf(output, "static const u8 %s_data[] = {\n", font_name);

    for (size_t glyph = 0; glyph < output_glyph_count; glyph++) {
        fprintf(output, "    ");

        for (size_t row = 0; row < charsize; row++) {
            uint8_t byte;

            if (fread(&byte, 1, 1, input) != 1) {
                fprintf(stderr, "Unexpected end of PSF glyph data\n");
                fclose(input);
                fclose(output);
                return 1;
            }

            fprintf(output, "0x%02X", byte);

            if (!(glyph == output_glyph_count - 1 && row == charsize - 1)) {
                fprintf(output, ", ");
            }
        }

        fprintf(output, "\n");
    }

    fprintf(output, "};\n\n");

    fprintf(
        output,
        "const BitmapFont %s_font = {\n"
        "    .data = %s_data,\n"
        "    .glyph_size = {8, %u},\n"
        "    .glyph_count = %zu,\n"
        "    .bit_order = FONT_BIT_ORDER_MSB_FIRST\n"
        "};\n",
        font_name,
        font_name,
        charsize,
        output_glyph_count
    );

    fclose(input);
    fclose(output);

    return 0;
}