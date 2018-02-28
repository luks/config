#include <libconfig.h>
#include "trie.h"

#define STRING_SIZE 100

static int const USER_WHITE_LIST_MODE = 1;
static int const USER_BLACK_LIST_MODE = 2;

typedef struct users_s {
    int mode;
    char *name;
    trieNode_t *list;
} user_t;

typedef struct entry_s {
    char *key;
    user_t *value;
    struct entry_s *next;
} entry_t;

typedef struct hashtable_s {
    int size;
    struct entry_s **table;
} hashtable_t;

typedef struct conf_s {
    char * log_level;
    char * log_file;
    char * bind;
    char * redis;
    hashtable_t *users;
} conf_t;


hashtable_t *ht_create( int size );

void ht_set( hashtable_t *hashtable, char *key, user_t *value );

void checkIt(char *key, char *command, hashtable_t *hashtable);

conf_t *initAndParseConfig(const char *file);

conf_t *conf_create();

void ConfDestroy( conf_t* config );

void delete_user( user_t *us );

void delete_entry( entry_t *entry );

user_t *ht_get( hashtable_t *hashtable, char *key );

int checkAndLoadConfig(config_t *cfg, const char *file);
