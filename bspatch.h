/*-
 * Copyright 2003-2005 Colin Percival
 * Copyright 2012 Matthew Endsley
 * All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted providing that the following conditions 
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef BSPATCH_H
# define BSPATCH_H

# include <stdint.h>

struct bspatch_stream
{
	void* opaque;
	int (*read)(const struct bspatch_stream* stream, void* buffer, int length);
};

typedef struct bspatch_read_stream
{
	void *ctx;
	/**
	 * @brief read stream
	 * 
	 * @param stream stream pointer
	 * @param pos position, -1 for cur pos
	 * @param buffer read buffer
	 * @param length want read length
	 * 
	 * @return read length
	 */
	size_t (*read)(struct bspatch_read_stream *stream, int64_t pos, void *buffer, size_t length);
}bspatch_read_stream;

typedef struct bspatch_write_stream
{
	void *ctx;
	/**
	 * @brief write stream
	 * 
	 * @param stream stream pointer
	 * @param pos position, -1 for cur pos, -2 for head, other positive for position
	 * @param buffer write buffer
	 * @param length write buffer length
	 * 
	 * @return length writed
	 */
	size_t (*write)(struct bspatch_write_stream *stream, int64_t pos, void *buffer, size_t length);
}bspatch_write_stream;

/**
 * @brief bspatch stream version, with block read write
 * 
 * @param src_stream src read stream
 * @param src_sz src size
 * @param dst_stream dst write stream
 * @param dst_sz dst size
 * @param patch_stream patch read stream
 * @param block_sz Read write block, too small maybe too slow, but memory usage better.
 * @return int 
 */
int bspatch_stream(bspatch_read_stream *src_stream, size_t src_sz,
					bspatch_write_stream *dst_stream, size_t dst_sz, 
					bspatch_read_stream *patch_stream, size_t block_sz);

/**
 * @brief bspatch origin
 * 
 * @param old old data buffer
 * @param oldsize old size
 * @param new new data buffer
 * @param newsize new size
 * @param stream patch read stream
 * @return int 
 */
int bspatch(const uint8_t* old, int64_t oldsize, uint8_t* new, int64_t newsize, struct bspatch_stream* stream);

#endif

