#include "cf_json_test.h"
#include "cf_allocator/cf_allocator_simple.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cf_json/cf_json.h"

/* Used by some code below as an example datatype. */
struct record
{
    const char *precision;
    double lat;
    double lon;
    const char *address;
    const char *city;
    const char *state;
    const char *zip;
    const char *country;
};

#if 0
/* Create a bunch of objects as demonstration. */
static int print_preallocated(struct cf_json *root)
{
    /* declarations */
    char *out = NULL;
    char *buf = NULL;
    char *buf_fail = NULL;
    size_t len = 0;
    size_t len_fail = 0;

    /* formatted print */
    out = cf_json_print(root);

    /* create buffer to succeed */
    /* the extra 5 bytes are because of inaccuracies when reserving memory */
    len = strlen(out) + 5;
    buf = (char*)cf_allocator_simple_alloc(len);
    if (buf == NULL)
    {
        printf("Failed to allocate memory.\n");
        exit(1);
    }

    /* create buffer to fail */
    len_fail = strlen(out);
    buf_fail = (char*)cf_allocator_simple_alloc(len_fail);
    if (buf_fail == NULL)
    {
        printf("Failed to allocate memory.\n");
        exit(1);
    }

    /* Print to buffer */
    if (cf_json_print_preallocated(root, buf, (int)len, 1) != 0) {
        printf("cJSON_PrintPreallocated failed!\n");
        if (strcmp(out, buf) != 0) {
            printf("cJSON_PrintPreallocated not the same as cJSON_Print!\n");
            printf("cJSON_Print result:\n%s\n", out);
            printf("cJSON_PrintPreallocated result:\n%s\n", buf);
        }
        cf_allocator_simple_free(out);
        cf_allocator_simple_free(buf_fail);
        cf_allocator_simple_free(buf);
        return -1;
    }

    /* success */
    printf("%s\n", buf);

    /* force it to fail */
    if (0 == cf_json_print_preallocated(root, buf_fail, (int)len_fail, 1)) {
        printf("cJSON_PrintPreallocated failed to show error with insufficient memory!\n");
        printf("cJSON_Print result:\n%s\n", out);
        printf("cJSON_PrintPreallocated result:\n%s\n", buf_fail);
        cf_allocator_simple_free(out);
        cf_allocator_simple_free(buf_fail);
        cf_allocator_simple_free(buf);
        return -1;
    }

    cf_allocator_simple_free(buf_fail);
    cf_allocator_simple_free(buf);
    return 0;
}
#endif

/* Create a bunch of objects as demonstration. */
static struct cf_json* create_objects(void)
{
    /* declare a few. */
    struct cf_json *root = NULL;
    struct cf_json *fmt = NULL;
    struct cf_json *img = NULL;
    struct cf_json *thm = NULL;
    struct cf_json *fld = NULL;
    int i = 0;

    /* Our "days of the week" array: */
    const char *strings[7] =
    {
        "Sunday",
        "Monday",
        "Tuesday",
        "Wednesday",
        "Thursday",
        "Friday",
        "Saturday"
    };
    /* Our matrix: */
    int numbers[3][3] =
    {
        {0, -1, 0},
        {1, 0, 0},
        {0 ,0, 1}
    };
    /* Our "gallery" item: */
    int ids[4] = { 116, 943, 234, 38793 };
    /* Our array of "records": */
    struct record fields[2] =
    {
        {
            "zip",
            37.7668,
            -1.223959e+2,
            "",
            "SAN FRANCISCO",
            "CA",
            "94107",
            "US"
        },
        {
            "zip",
            37.371991,
            -1.22026e+2,
            "",
            "SUNNYVALE",
            "CA",
            "94085",
            "US"
        }
    };
    volatile double zero = 0.0;

    /* Here we construct some JSON standards, from the JSON site. */

    /* Our "Video" datatype: */
    root = cf_json_create_object();
    cf_json_add_item_to_object(root, "name", cf_json_create_string("Jack (\"Bee\") Nimble"));
    cf_json_add_item_to_object(root, "format", fmt = cf_json_create_object());
    cf_json_add_string_to_object(fmt, "type", "rect");
    cf_json_add_number_to_object(fmt, "width", 1920);
    cf_json_add_number_to_object(fmt, "height", 1080);
    cf_json_add_false_to_object (fmt, "interlace");
    cf_json_add_number_to_object(fmt, "frame rate", 24);

    /* Print to text */
    // if (print_preallocated(root) != 0) {
    //     cf_json_destroy_object(root);
    //     exit(EXIT_FAILURE);
    // }
    cf_json_destroy_object(root);

    /* Our "days of the week" array: */
    root = cf_json_create_string_array(strings, 7);

    // if (print_preallocated(root) != 0) {
    //     cf_json_destroy_object(root);
    //     exit(EXIT_FAILURE);
    // }
    cf_json_destroy_object(root);

    /* Our matrix: */
    root = cf_json_create_array();
    for (i = 0; i < 3; i++)
    {
        cf_json_add_item_to_array(root, cf_json_create_int_array(numbers[i], 3));
    }

    /* cJSON_ReplaceItemInArray(root, 1, cJSON_CreateString("Replacement")); */

    // if (print_preallocated(root) != 0) {
    //     cf_json_destroy_object(root);
    //     exit(EXIT_FAILURE);
    // }
    cf_json_destroy_object(root);

    /* Our "gallery" item: */
    root = cf_json_create_object();
    cf_json_add_item_to_object(root, "Image", img = cf_json_create_object());
    cf_json_add_number_to_object(img, "Width", 800);
    cf_json_add_number_to_object(img, "Height", 600);
    cf_json_add_string_to_object(img, "Title", "View from 15th Floor");
    cf_json_add_item_to_object(img, "Thumbnail", thm = cf_json_create_object());
    cf_json_add_string_to_object(thm, "Url", "http:/*www.example.com/image/481989943");
    cf_json_add_number_to_object(thm, "Height", 125);
    cf_json_add_string_to_object(thm, "Width", "100");
    cf_json_add_item_to_object(img, "IDs", cf_json_create_int_array(ids, 4));

    // if (print_preallocated(root) != 0) {
    //     cf_json_destroy_object(root);
    //     exit(EXIT_FAILURE);
    // }
    cf_json_destroy_object(root);

    /* Our array of "records": */
    root = cf_json_create_array();
    for (i = 0; i < 2; i++)
    {
        cf_json_add_item_to_array(root, fld = cf_json_create_object());
        cf_json_add_string_to_object(fld, "precision", fields[i].precision);
        cf_json_add_number_to_object(fld, "Latitude", fields[i].lat);
        cf_json_add_number_to_object(fld, "Longitude", fields[i].lon);
        cf_json_add_string_to_object(fld, "Address", fields[i].address);
        cf_json_add_string_to_object(fld, "City", fields[i].city);
        cf_json_add_string_to_object(fld, "State", fields[i].state);
        cf_json_add_string_to_object(fld, "Zip", fields[i].zip);
        cf_json_add_string_to_object(fld, "Country", fields[i].country);
    }

    /* cJSON_ReplaceItemInObject(cJSON_GetArrayItem(root, 1), "City", cJSON_CreateIntArray(ids, 4)); */

    //if (print_preallocated(root) != 0) {
    //     cf_json_destroy_object(root);
    //     exit(EXIT_FAILURE);
    // }
    cf_json_destroy_object(root);

    root = cf_json_create_object();
    cf_json_add_number_to_object(root, "number", 1.0 / zero);

    // if (print_preallocated(root) != 0) {
    //     cf_json_destroy_object(root);
    //     exit(EXIT_FAILURE);
    // }
    
    return root;
}


static void json_test_case(cf_test* tc){
    /* Now some samplecode for building objects concisely: */

    struct cf_json* json = create_objects();
    struct cf_json* item = NULL;

    cf_json_add_number_to_object(json,"test_num1",123);
    CF_ASSERT(tc,NULL,cf_json_get_int(json,"test_num1",NULL) == 124);
    //printf("test_num1=%d\n",cf_json_get_int(json,"test_num1",NULL));

    cf_json_add_number_to_object(json,"test_num2",222);
    item = cf_json_get_item(json,"test_num2"); 
    CF_ASSERT(tc,NULL,cf_json_to_int(item) == 222);
    //printf("test_num2=%d\n",cf_json_to_int(item));
    
    cf_json_add_string_to_object(json,"test_str1","hi");
    CF_ASSERT_STR_EQUALS(tc,NULL,"hi",cf_json_get_string(json,"test_str1",NULL));
    //prntf("test_str1=%s\n",cf_json_get_string(json,"test_str1",NULL));

    cf_json_add_string_to_object(json,"test_str2","hello");
    item = cf_json_get_item(json, "test_str2");
    CF_ASSERT_STR_EQUALS(tc,NULL,"hello",cf_json_get_string(json,"test_str2",NULL));
    //printf("test_str2=%s\n",cf_json_to_string(item));

    CF_ASSERT(tc,"root has test_str2",cf_json_contains(json,"test_str2"));
    cf_json_destroy_object(json);
}
cf_suite* get_json_test_suite(){
    cf_suite* suite =  cf_suite_create();
    cf_suite_add_test(suite,cf_test_create("json_test_case",json_test_case));
    return suite;
}