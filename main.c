#include "hashConf.h"

int main() {

    char *file = "../config.cfg";

    conf_t * config = initAndParseConfig(file);

    char ks[100] = "selfcheck";
    char cs[100] = "SELECT";
    checkIt(ks, cs, &config->users);

    return 0;
}



