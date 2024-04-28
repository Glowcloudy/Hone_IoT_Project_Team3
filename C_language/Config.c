#ifndef __SERVER__HEADER__
#define __SERVER__HEADER__
#include "Server_Header.h"
#endif

// Read JSON file and set server config
void read_Config(void) {
    
    struct json_object_iterator it, itEnd;
    struct json_object* obj;
    struct json_object* cur;

    const char* type;
    const char* name;
    
    // Read JSON file, and make JSON object
    obj =  json_object_from_file("./config.json");

    // If read JSON file fail, output error message and exit program
    if(obj == NULL) {
        printf("Check json file\n");
        exit(1);
    }
    
    // Make JSON iterator
    it = json_object_iter_begin(obj);
    itEnd = json_object_iter_end(obj);
    
    // Loop JSON object
    while(!json_object_iter_equal(&it, &itEnd)) {
        
        // Read data from iterator
        cur = json_object_iter_peek_value(&it);
        type = json_type_to_name(json_object_get_type(cur));
        name = json_object_iter_peek_name(&it);
        
        // Input Data
        if(!strcmp(type,"string")) {

            // DB host
            if(!strcmp("DB_HOST", name))
                strcpy(cfg.HOST, json_object_get_string(cur));

            // DB user_id
            else if(!strcmp("DB_USER", name))
                strcpy(cfg.USER, json_object_get_string(cur));

            // DB user_password
            else if(!strcmp("DB_PASS", name))
                strcpy(cfg.PASS, json_object_get_string(cur));

            // ignore other type
            else
                continue;        
        }

        // DB port
        else if(!strcmp(type,"int"))
            cfg.PORT = json_object_get_int(cur);

        // other type
        else
            printf("Unkown Type.\n");

        // Next iterator
        json_object_iter_next(&it);
    }
}