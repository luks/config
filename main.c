#include "hashConf.h"

int main() {

    char *file = "../config.cfg";

    conf_t * config = initAndParseConfig(file);


    checkIt("selfcheck", "SELECT", &config->users);
    checkIt("anton",     "redrussian", &config->users);
    checkIt("anton",     "kurac", &config->users);
    checkIt("admin",     "ZSET", &config->users);
    checkIt("piso",      "SELECT", &config->users);
    checkIt("piso",      "Kurac", &config->users);
    checkIt("piso",      "ZRANGE", &config->users);
    checkIt("piso",      "INFO", &config->users);
    checkIt("piso",      "ZSCAN", &config->users);

    ConfDestroy(config);
    return 0;
}



