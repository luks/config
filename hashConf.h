#include "trie.h"

#define STRING_SIZE 100

static int const USER_WHITE_LIST_MODE = 1;
static int const USER_BLACK_LIST_MODE = 2;

typedef struct users_s {
    int mode;
    const char *name;
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

typedef struct serv_s {
    char host[STRING_SIZE];
    char port[STRING_SIZE];
} serv_t;

typedef struct conf_s {
    serv_t *proxy;
    serv_t *redis;
    hashtable_t *users;
} conf_t;


hashtable_t *ht_create( int size );

void ht_set( hashtable_t *hashtable, char *key, user_t *value );

void checkIt(char *key, char *command, hashtable_t **hashtable);

conf_t *initAndParseConfig(const char *file);

conf_t *conf_create();


