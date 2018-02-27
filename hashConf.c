#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <libconfig.h>
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

    unsigned long int hashval;
    int i = 0;

    while( hashval < ULONG_MAX && i < strlen( key ) ) {
        hashval = hashval << 8;
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

void checkIt(char *key, char *command, hashtable_t **hashtable) {

    user_t *user = ht_get( *hashtable, key);

    if(NULL == user) {
        printf("Key  not found in hash\n");
    } else {
        trieNode_t *tPtr = NULL;
        tPtr = TrieSearch(user->list->children, command);
        if(NULL == tPtr ) {
            printf("Command [%s] not found in key [%s] trie\n", command, key);
        } else {
            printf("Command [%s] found in key [%s] trie\n", command, key);
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

user_t *create_user(config_setting_t *user, char const *name, int mode, const config_setting_t *array) {

    user_t *us = NULL;
    trieNode_t *tree;
    TrieCreate(&tree);

    if( ( us = malloc( sizeof( user_t ) ) ) == NULL ) {
        return NULL;
    }
    char const *v;
    int k = 0;

    us->name = name;
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

conf_t *initAndParseConfig(const char *file) {

    conf_t * config = conf_create();

    struct config_t cfg;
    config_init(&cfg);

    if (!config_read_file(&cfg, file)) {
        printf("failed\n");
        return NULL;
    }

    config_setting_t *users, *user;
    users = config_lookup(&cfg, "users");
    if (users == NULL) {
        printf("no users\n");
        return NULL;
    }

    int n = 0, k = 0, mode;
    char const  *v, *name;
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

        ht_set( hashtable, (char *) name, conf_user);

        ++n;
    }

    config->users = hashtable;

    config_destroy(&cfg);

    printf("Done\n");

    return config;
}


















