#include <stdio.h>
#include <string.h>
#include "cf_iostream_file.h"
#include "cf_collection/cf_string.h"
#include "cf_allocator/cf_allocator_simple.h"
#include <stdint.h>

typedef struct cf_iostream_file{
    cf_iostream m_stream;
    uint32_t m_flags;
    cf_string* m_filePath;
    FILE* m_fp;
}cf_iostream_file;

#define IOSTREAM_TO_FILE_STREAM(stream)    (cf_iostream_file*)((uint8_t*)(stream) - (uint8_t*)&((cf_iostream_file*)0)->m_stream)

static const char* toFileMode(uint32_t flags){
    uint32_t _flags = (flags & CF_IOS_WRITE) | (flags & CF_IOS_READ);
    const char* mode = NULL;
    switch (_flags)
    {
    case CF_IOS_WRITE:
        mode = "w";
        break;
    case CF_IOS_READ:
        mode = "r";
        break;
    case CF_IOS_READ | CF_IOS_WRITE:
        mode = "rw";
        break;
    
    default:
        break;
    }
    return mode;

}

static int file_stream_writeln(cf_iostream* stream,const char* str){
    //cf_iostream* offset = &((cf_iostream_file*)0)->m_stream;
    //cf_iostream_file* addr = (cf_iostream_file*)((uint8_t*)(stream) - (uint8_t*)offset);
    cf_iostream_file* file_stream = IOSTREAM_TO_FILE_STREAM(stream);
    if(file_stream->m_fp == NULL){
        file_stream->m_fp = fopen(cf_string_c_str(file_stream->m_filePath),toFileMode(file_stream->m_flags));
        if(file_stream->m_fp == NULL){
            return -1;
        }
    }
    return fprintf(file_stream->m_fp,"%s",str);
}

static void file_stream_close(cf_iostream* stream){
    cf_iostream_file* file_stream = IOSTREAM_TO_FILE_STREAM(stream);
    if(file_stream->m_fp != NULL){
        fclose(file_stream->m_fp);
        file_stream->m_fp = NULL;
    }
}
static void file_stream_destroy(cf_iostream* stream){
    cf_iostream_file* file_stream = IOSTREAM_TO_FILE_STREAM(stream);
    file_stream_close(stream);
    cf_string_destroy(file_stream->m_filePath);
    file_stream->m_filePath = NULL;
    cf_allocator_simple_free((void*)file_stream);
}

static const cf_iostream_vt file_iostream_vt = {
    .writeln = file_stream_writeln,
    .close = file_stream_close,
    .destroy = file_stream_destroy
};


cf_iostream* cf_iostream_from_file(const char* file_name,uint32_t flags){
    if(toFileMode(flags) == NULL)
        return NULL;
    cf_iostream_file* stream = (cf_iostream_file*)cf_allocator_simple_alloc(sizeof(cf_iostream_file));
    if(stream){
        stream->m_flags = flags;
        stream->m_filePath = cf_string_create_frome_c_str(file_name);
        if(stream->m_filePath == NULL){
            cf_allocator_simple_free(stream);
            return NULL;
        }

        stream->m_stream.m_vt = &file_iostream_vt;
    }
    return &stream->m_stream;
}

