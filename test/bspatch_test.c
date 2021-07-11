#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "bspatch.h"

int stream_file_read(struct bspatch_stream *stream, void *buffer, int length)
{
    size_t rdsz = 0;
    FILE *fd = (FILE *)stream->opaque;
    rdsz = fread(buffer, 1, length, fd);
    // printf("try read: %08u, real: %08u, fd: %p\n", length, rdsz, fd);
    return rdsz == length ? 0 : -1;
}

int main(int argc, char **argv)
{
    int ch;
    char *src_path = NULL;
    char *dif_path = NULL;
    char *out_path = NULL;

    FILE *src_fd = NULL;
    FILE *dif_fd = NULL;
    FILE *out_fd = NULL;

    size_t src_sz = 0;
    size_t dst_sz = 0;
    size_t out_sz = 0;

    uint8_t *src_buf = NULL;
    // uint8_t *dif_buf = NULL;
    uint8_t *out_buf = NULL;

    struct bspatch_stream stream = {
        .opaque = NULL,
        .read   = stream_file_read
    };

    opterr = 0;
    while( (ch=getopt(argc,argv, "s:d:o:n:")) != -1)
    {
        switch (ch)
        {
            case 's':
                src_path = optarg;
                printf("src file: %s\n", optarg);
                break;

            case 'd':
                dif_path = optarg;
                printf("dif file: %s\n", optarg);
                break;

            case 'o':
                out_path = optarg;
                printf("out file: %s\n", optarg);
                break;

            case 'n':
                sscanf(optarg, "%u", &out_sz);
                printf("out size: %u\n", out_sz);
                break;
            
            default:
                printf("unknown\n");
                break;
        }
    }

    if (src_path == NULL || dif_path == NULL || out_path == NULL || out_sz == 0) {
        printf(
            "Usage: bspatch_test -s [old_file] -d [dif_file] -o [out_file] -n [out_size]\n"
        );
        return 0;
    }

    printf("\n");

    src_fd = fopen(src_path, "rb");
    dif_fd = fopen(dif_path, "rb");
    out_fd = fopen(out_path, "wb");

    if (src_fd == NULL || dif_fd == NULL || out_fd == NULL) {
        printf("file open error\n");
        goto __exit;
    }

    stream.opaque = dif_fd;

    fseek(src_fd, 0, SEEK_END);
    fseek(dif_fd, 0, SEEK_END);
    src_sz = ftell(src_fd);
    dst_sz = ftell(dif_fd);
    fseek(src_fd, 0, SEEK_SET);
    fseek(dif_fd, 0, SEEK_SET);

    printf("src size: %d\n", src_sz);
    printf("dif size: %d\n", dst_sz);

    src_buf = (uint8_t *)malloc(src_sz);
    out_buf = (uint8_t *)malloc(out_sz);

    if (src_buf == NULL || out_buf == NULL) {
        printf("mem allocate err\n");
        goto __exit;
    }

    fread(src_buf, 1, src_sz, src_fd);

    if ( bspatch(src_buf, src_sz, out_buf, out_sz, &stream) == 0)
    {
        fwrite(out_buf, 1, out_sz, out_fd);
    } else {
        printf("patch fail\n");
    }

    __exit:
    if (src_fd) fclose(src_fd);
    if (dif_fd) fclose(dif_fd);
    if (out_fd) fclose(out_fd);
    if (src_buf) free(src_buf);
    if (out_buf) free(out_buf);
    return 0;
}