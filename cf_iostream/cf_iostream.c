#include <stdio.h>
#include "cf_iostream.h"



int cf_iostream_writeln( cf_iostream* stream,const char* str){
    return stream->m_vt->writeln(stream,str);
}
void cf_iostream_close(cf_iostream* stream){
    if(stream->m_vt->close){
        stream->m_vt->close(stream);
    }
}
void cf_iostream_destroy(cf_iostream* stream){
    if(stream->m_vt->destroy){
        stream->m_vt->destroy(stream);
    }
}
/*****************标准输出****************************************/
static int std_out_writeln( cf_iostream* stream,const char* str){
    printf("%s",str);
}

static const cf_iostream_vt std_out_iostream_vt = {
    .writeln = std_out_writeln,
    .close = NULL,
    .destroy = NULL
};
cf_iostream* cf_iostream_from_std_out(){
    static cf_iostream stream = { .m_vt = &std_out_iostream_vt};
    return &stream;
}
/*******************************************************************/

/*****************标准错误****************************************/
static int std_err_writeln( cf_iostream* stream,const char* str){
    fprintf( stderr,"%s", str );
}

static const cf_iostream_vt std_err_iostream_vt = {
    .writeln = std_err_writeln,
    .close = NULL,
    .destroy = NULL
};
cf_iostream* cf_iostream_from_std_err(){
    static cf_iostream stream = { .m_vt = &std_err_iostream_vt};
    return &stream;
}
/*******************************************************************/