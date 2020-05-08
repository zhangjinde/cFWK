#include <stdio.h>
#include <string.h>
#include "cf_iostream_file.h"
#include "cf_collection/cf_string.h"
#include "cf_allocator/cf_allocator_simple.h"

#define IOSTREAM_TO_FILE_STREAM(stream)    ((stream) - &((cf_iostream_file*)0)->m_stream)

typedef struct cf_iostream_file{
    cf_iostream m_stream;
    const char* m_filePath;
    FILE* m_fp;
}cf_iostream_file;




static int file_writeln(cf_iostream* stream,const char* str){
    cf_iostream_file* file_stream = IOSTREAM_TO_FILE_STREAM(stream);
    if(file_stream->m_fp == NULL){
        file_stream->m_fp = fopen(file_stream->m_filePath,"w");
        if(file_stream->m_fp == NULL){
            return -1;
        }
    }
    return 0;
}
static const cf_iostream_vt file_iostream_vt = {
    .writeln = file_writeln,
    .close = NULL,
    .destroy = NULL
};


cf_iostream* cf_iostream_from_file(const char* file_name){
    cf_iostream_file* stream = cf_allocator_simple_alloc(sizeof(cf_iostream));
    if(stream){
        stream->m_filePath = strdup(file_name);
        stream->m_stream.m_vt = &file_iostream_vt;
    }
    return stream;
}

