#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "bspatch.h"

int bspatch_stream_read(bspatch_read_stream *stream, int64_t pos, void *buffer, size_t length)
{
    size_t rdsz = 0;
    FILE *fd = (FILE *)stream->ctx;
    if (pos >=0) fseek(fd, pos, SEEK_SET);
    rdsz = fread(buffer, 1, length, fd);
    return rdsz;
}

int bspatch_stream_write(bspatch_read_stream *stream, int64_t pos, void *buffer, size_t length)
{
    size_t wrsz = 0;
    FILE *fd = (FILE *)stream->ctx;
    if (pos >=0) fseek(fd, pos, SEEK_SET);
    wrsz = fwrite(buffer, 1, length, fd);
    return wrsz;
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
    size_t dif_sz = 0;
    size_t out_sz = 0;

    bspatch_read_stream src_stream;
    bspatch_read_stream dif_stream;
    bspatch_write_stream out_stream;

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

    src_fd = fopen(src_path, "rb");
    dif_fd = fopen(dif_path, "rb");
    out_fd = fopen(out_path, "wb");

    if (src_fd == NULL || dif_fd == NULL || out_fd == NULL) {
        printf("file open error\n");
        goto __exit;
    }

    fseek(src_fd, 0, SEEK_END);
    fseek(dif_fd, 0, SEEK_END);
    src_sz = ftell(src_fd);
    dif_sz = ftell(dif_fd);
    fseek(src_fd, 0, SEEK_SET);
    fseek(dif_fd, 0, SEEK_SET);

    printf("src size: %d\n", src_sz);
    printf("dif size: %d\n", dif_sz);

    src_stream.ctx = src_fd;
    dif_stream.ctx = dif_fd;
    out_stream.ctx = out_fd;

    src_stream.read  = bspatch_stream_read;
    dif_stream.read  = bspatch_stream_read;
    out_stream.write = bspatch_stream_write;

    if (bspatch_stream(&src_stream, src_sz, &out_stream, out_sz, &dif_stream, 256) == 0)
    {
        printf("patch done\n");
    } else {
        printf("patch fail\n");
    }

    __exit:
    if (src_fd) fclose(src_fd);
    if (dif_fd) fclose(dif_fd);
    if (out_fd) fclose(out_fd);
    return 0;
}