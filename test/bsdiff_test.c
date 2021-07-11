#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "bsdiff.h"

int stream_file_write(struct bsdiff_stream *stream, const void *buffer, int size)
{
    size_t wrsz = 0;
    FILE *fd = (FILE *)stream->opaque;
    wrsz = fwrite(buffer, 1, size, fd);
    return wrsz == size ? 0 : -1;
}

int main(int argc, char **argv)
{
    int ch;
    char *src_path = NULL;
    char *dst_path = NULL;
    char *out_path = NULL;

    FILE *src_fd = NULL;
    FILE *dst_fd = NULL;
    FILE *out_fd = NULL;

    size_t src_sz = 0;
    size_t dst_sz = 0;

    uint8_t *src_buf = NULL;
    uint8_t *dst_buf = NULL;

    struct bsdiff_stream stream = {
        .opaque = NULL,
        .malloc = malloc,
        .free   = free,
        .write  = stream_file_write
    };

    opterr = 0;
    while( (ch=getopt(argc,argv, "s:d:o:")) != -1)
    {
        switch (ch)
        {
            case 's':
                src_path = optarg;
                printf("src file: %s\n", optarg);
                break;

            case 'd':
                dst_path = optarg;
                printf("dst file: %s\n", optarg);
                break;

            case 'o':
                out_path = optarg;
                printf("out file: %s\n", optarg);
                break;
            
            default:
                printf("unknown\n");
                break;
        }
    }

    if (src_path == NULL || dst_path == NULL || out_path == NULL) {
        printf(
            "Usage: bsdiff_test -s [old_file] -d [dst_file] -o [patch_file]\n"
        );
        return 0;
    }

    src_fd = fopen(src_path, "rb");
    dst_fd = fopen(dst_path, "rb");
    out_fd = fopen(out_path, "wb");

    if (src_fd == NULL || dst_fd == NULL || out_fd == NULL) {
        printf("file open error\n");
        goto __exit;
    }

    stream.opaque = out_fd;

    fseek(src_fd, 0, SEEK_END);
    fseek(dst_fd, 0, SEEK_END);
    src_sz = ftell(src_fd);
    dst_sz = ftell(dst_fd);
    fseek(src_fd, 0, SEEK_SET);
    fseek(dst_fd, 0, SEEK_SET);

    printf("src size: %d\n", src_sz);
    printf("dst size: %d\n", dst_sz);

    src_buf = (uint8_t *)malloc(src_sz);
    dst_buf = (uint8_t *)malloc(dst_sz);

    if (!src_buf || !dst_buf) {
        printf("mem allocate err\n");
        goto __exit;
    }

    fread(src_buf, 1, src_sz, src_fd);
    fread(dst_buf, 1, dst_sz, dst_fd);

    bsdiff(src_buf, src_sz, dst_buf, dst_sz, &stream);

    __exit:
    if (src_fd) fclose(src_fd);
    if (dst_fd) fclose(dst_fd);
    if (out_fd) fclose(out_fd);
    if (src_buf) free(src_buf);
    if (dst_buf) free(dst_buf);
    return 0;
}