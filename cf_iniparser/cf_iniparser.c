#include "cf_iniparser.h"
#include "iniparser/iniparser.h"
#include "cf_allocator/cf_allocator_simple.h"
#if 1

struct cf_inicontext{
    dictionary* dict;
};
struct cf_inicontext* cf_iniparser_load_from_file(const char* file_name)
{
    dictionary* dict = iniparser_load(file_name);
    if(dict == NULL)
        return NULL;
    struct cf_inicontext* context = cf_allocator_simple_alloc(sizeof(struct cf_inicontext));
    if(context == NULL)
        return NULL;

    context->dict = dict;
    return context;
}
void cf_inicontext_release(struct cf_inicontext* context){
    if(context == NULL)
        return;
    if(context->dict)
    {
        iniparser_freedict(context->dict);
        context->dict = NULL;
    }
    cf_allocator_simple_free(context);
}

int cf_iniparser_get_boolean(struct cf_inicontext* context,const char* key,int notfond)
{
    if(context == NULL || context->dict == NULL)
        return notfond;
        
    return iniparser_getboolean(context->dict,key,notfond);    
}

int cf_iniparser_get_int(struct cf_inicontext* context,const char* key,int notfond)
{
    if(context == NULL || context->dict == NULL)
        return notfond;
    
    return iniparser_getint(context->dict,key,notfond);    
}

const char *  cf_iniparser_get_string(struct cf_inicontext* context,const char* key,const char * notfond)
{
    if(context == NULL || context->dict == NULL)
        return notfond;
    
    return iniparser_getstring(context->dict,key,notfond);    
}

double cf_iniparser_get_double(struct cf_inicontext* context,const char* key,double notfond)
{
    if(context == NULL || context->dict == NULL)
        return notfond;
    
    return iniparser_getdouble(context->dict,key,notfond);    
}

/*********************************************
 * gcc -DCF_INIPARSER_TEST cf_iniparser.c  iniparser/dictionary.c iniparser/iniparser.c -o cf_iniparser_test
 * ********************************************/
#ifdef CF_INIPARSER_TEST
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
void create_example_ini_file(void);
int parse_ini_file(char * ini_name);

int main(int argc, char * argv[])
{
   int     status ;

    if (argc<2) {
        create_example_ini_file();
        status = parse_ini_file("example.ini");
    } else {
        status = parse_ini_file(argv[1]);
    }
    return status ;
}

void create_example_ini_file(void)
{
    FILE    *   ini ;

    if ((ini=fopen("example.ini", "w"))==NULL) {
        fprintf(stderr, "iniparser: cannot create example.ini\n");
        return ;
    }

    fprintf(ini,
    "#\n"
    "# This is an example of ini file\n"
    "#\n"
    "\n"
    "Test       = 1052 ;\n"
    "[Pizza]\n"
    "\n"
    "Ham       = yes ;\n"
    "Mushrooms = TRUE ;\n"
    "Capres    = 0 ;\n"
    "Cheese    = Non ;\n"
    "\n"
    "\n"
    "[Wine]\n"
    "\n"
    "Grape     = Cabernet Sauvignon ;\n"
    "Year      = 1989 ;\n"
    "Country   = Spain ;\n"
    "Alcohol   = 12.5  ;\n"
    "\n");
    fclose(ini);
}

int parse_ini_file(char * ini_name)
{
    struct cf_inicontext  *   context ;

    /* Some temporary variables to hold query results */
    int             b ;
    int             i ;
    double          d ;
    const char  *   s ;

    context = cf_iniparser_load_from_file(ini_name);
    if (context==NULL) {
        fprintf(stderr, "cannot parse file: %s\n", ini_name);
        return -1 ;
    }
    //iniparser_dump(ini, stderr);

    /* Get pizza attributes */
    

    i = cf_iniparser_get_int(context, ":Test", -1);
    printf("Test:      [%d]\n", i);
    
    printf("Pizza:\n");

    b = cf_iniparser_get_boolean(context, "pizza:ham", -1);
    printf("Ham:       [%d]\n", b);
    b = cf_iniparser_get_boolean(context, "pizza:mushrooms", -1);
    printf("Mushrooms: [%d]\n", b);
    b = cf_iniparser_get_boolean(context, "pizza:capres", -1);
    printf("Capres:    [%d]\n", b);
    b = cf_iniparser_get_boolean(context, "pizza:cheese", -1);
    printf("Cheese:    [%d]\n", b);

    /* Get wine attributes */
    printf("Wine:\n");
    s = cf_iniparser_get_string(context, "wine:grape", NULL);
    printf("Grape:     [%s]\n", s ? s : "UNDEF");

    i = cf_iniparser_get_int(context, "wine:year", -1);
    printf("Year:      [%d]\n", i);

    s = cf_iniparser_get_string(context, "wine:country", NULL);
    printf("Country:   [%s]\n", s ? s : "UNDEF");

    d = cf_iniparser_get_double(context, "wine:alcohol", -1.0);
    printf("Alcohol:   [%g]\n", d);

    cf_inicontext_release(context);
    return 0 ;
}
#endif//CF_INIPARSER_TEST
#endif