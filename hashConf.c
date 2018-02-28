#include <stdlib.h>
#include <string.h>
#include <zconf.h>
#include "hashConf.h"

hashtable_t *ht_create( int size ) {

    hashtable_t *hashtable = NULL;
    int i;

    if( size < 1 ) return NULL;

    if( ( hashtable = malloc( sizeof( hashtable_t ) ) ) == NULL ) {
        return NULL;
    }

    if( ( hashtable->table = malloc( sizeof( entry_t * ) * size ) ) == NULL ) {
        return NULL;
    }
    for( i = 0; i < size; i++ ) {
        hashtable->table[i] = NULL;
    }

    hashtable->size = size;

    return hashtable;
}

int ht_hash( hashtable_t *hashtable, char *key ) {

    unsigned long int hashval = 1;
    int i = 0;
    int size = 8;

    while( hashval < ULONG_MAX && i < strlen( key ) ) {
        hashval = hashval << size;
        hashval += key[ i ];
        i++;
    }

    return hashval % hashtable->size;
}

entry_t *ht_newpair( char *key, user_t *value ) {

    entry_t *newpair;

    if( ( newpair = malloc( sizeof( entry_t ) ) ) == NULL ) {
        return NULL;
    }

    if( ( newpair->key = strdup( key ) ) == NULL ) {
        return NULL;
    }

    newpair->value = value;

    newpair->next = NULL;

    return newpair;
}

void ht_set( hashtable_t *hashtable, char *key, user_t *value ) {
    int bin = 0;
    entry_t *newpair = NULL;
    entry_t *next = NULL;
    entry_t *last = NULL;

    bin = ht_hash( hashtable, key );
    next = hashtable->table[ bin ];

    while( next != NULL && next->key != NULL && strcmp( key, next->key ) > 0 ) {
        last = next;
        next = next->next;
    }

    if( next != NULL && next->key != NULL && strcmp( key, next->key ) == 0 ) {
        free( next->value );
        next->value = value;
    } else {
        newpair = ht_newpair( key, value );

        if( next == hashtable->table[ bin ] ) {
            newpair->next = next;
            hashtable->table[ bin ] = newpair;
        } else if ( next == NULL ) {
            last->next = newpair;
        } else  {
            newpair->next = next;
            last->next = newpair;
        }
    }
}

user_t *ht_get( hashtable_t *hashtable, char *key ) {
    int bin = 0;
    entry_t *pair;

    bin = ht_hash( hashtable, key );

    pair = hashtable->table[ bin ];
    while( pair != NULL && pair->key != NULL && strcmp( key, pair->key ) > 0 ) {
        pair = pair->next;
    }

    if( pair == NULL || pair->key == NULL || strcmp( key, pair->key ) != 0 ) {
        return NULL;
    } else {
        return pair->value;
    }
}

void checkIt(char *key, char *command, hashtable_t *hashtable) {

    user_t *user = ht_get( hashtable, key);

    if(NULL == user) {
        printf("User [%s] not found in configuration.\n", key);
    } else {
        trieNode_t *tPtr = NULL;
        tPtr = TrieSearch(user->list->children, command);
        if(NULL == tPtr ) {
            printf("User [%s] command [%s] not found.\n", key, command);
        } else {
            printf("User [%s] command [%s] found.\n", key, command);
        }
    }
}

conf_t *conf_create() {

    conf_t *conf = NULL;

    if( ( conf = malloc( sizeof( conf_t ) ) ) == NULL ) {
        return NULL;
    }

    return conf;
}

void ConfDestroy( conf_t* config ) {
    int i;
    int size = config->users->size;
    for( i = 0; i < size; i++ ) {
        if (NULL != config->users->table[i]) {
            delete_entry(config->users->table[i]);
        }
    }
    free(config->users->table);
    free(config->users);
    free(config->log_level);
    free(config->log_file);
    free(config->bind);
    free(config->redis);
    free(config);
}

void delete_entry( entry_t *entry ) {

    entry_t *last = NULL;
    while( entry != NULL && entry->key != NULL ) {
        last = entry;
        entry = entry->next;
        delete_user(last->value);
        free(last->key);
        free(last);
    }
}

void delete_user( user_t *user ) {
    TrieDestroy(user->list);
    free(user->name);
    free(user);
}

int checkAndLoadConfig(config_t *cfg, const char *file) {
    config_init(cfg);
    if (!config_read_file(cfg, file)) {
        fprintf(stderr, "%s:%d - %s\n", config_error_file(cfg),
                config_error_line(cfg), config_error_text(cfg));
        config_destroy(cfg);
        return 1;
    }
    return 0;
}

user_t *create_user(config_setting_t *user, const char *name, int mode, const config_setting_t *array) {

    user_t *us = NULL;
    trieNode_t *tree;
    TrieCreate(&tree);

    if( ( us = malloc( sizeof( user_t ) ) ) == NULL ) {
        return NULL;
    }
    char const *v;
    int k = 0;

    us->name = strdup(name);
    us->mode = mode;

    array = config_setting_get_member(user, "function_list");
    if (array) {
        while (1) {
            if (config_setting_get_elem(array, k) == NULL) {
                break;
            }
            v = config_setting_get_string_elem(array, k);
            TrieAdd(&tree, (char *)v, k);
            ++k;
        }
    }

    us->list = tree;

    return us;
}

hashtable_t *create_users(config_t *cfg) {

    int n = 0;
    int mode;
    const char *name;
    config_setting_t *users, *user;
    users = config_lookup(cfg, "users");
    if (users == NULL) {
        printf("no users\n");
        config_destroy(cfg);
        return NULL;
    }
    int users_count = config_setting_length(users);
    hashtable_t *hashtable = ht_create( users_count );

    while (1) {
        user = config_setting_get_elem(users, n);
        if (user == NULL) {
            break;
        }
        if (config_setting_lookup_string(user, "name", &name)) {
        }

        if (config_setting_lookup_int(user, "mode", &mode)) {
        }

        const config_setting_t *array = config_setting_get_member(user, "function_list");

        user_t *conf_user = create_user(user, name, mode, array);

        ht_set( hashtable, (char*) name, conf_user);

        ++n;
    }
    return hashtable;

}


conf_t *initAndParseConfig(const char *file) {

    config_t cfg;
    checkAndLoadConfig(&cfg, file);
    conf_t * app_config = conf_create();

    const char * log_level;
    const char * log_file;
    const char * bind;
    const char * redis;

    if(config_lookup_string(&cfg, "log_level", &log_level)) {
        app_config->log_level = strdup(log_level);
    } else {
        fprintf(stderr, "No 'log_level' setting in configuration file.\n");
    }
    if(config_lookup_string(&cfg, "log_file", &log_file))
        app_config->log_file = strdup(log_file);
    else
        fprintf(stderr, "No 'log_file' setting in configuration file.\n");

    if(config_lookup_string(&cfg, "bind", &bind))
        app_config->bind = strdup(bind);
    else
        fprintf(stderr, "No 'bind' setting in configuration file.\n");

    if(config_lookup_string(&cfg, "redis", &redis))
        app_config->redis = strdup(redis);
    else
        fprintf(stderr, "No 'redis' setting in configuration file.\n");

    hashtable_t *hashtable = create_users(&cfg);

    app_config->users = hashtable;

    config_destroy(&cfg);

    return app_config;
}