#ifndef CF_INIPARSER_H
#define CF_INIPARSER_H
struct cf_inicontext;
struct cf_iniparser;
struct cf_inicontext* cf_iniparser_load_from_file(const char* file_name);
void cf_inicontext_release(struct cf_inicontext*);

int cf_iniparser_getboolean();

#endif//CF_INIPARSER_H
