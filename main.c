#include "hashConf.h"

int main() {

    char *file = "../config.cfg";

    conf_t * config = initAndParseConfig(file);

    char ks[100] = "selfcheck";
    char cs[100] = "SELECT";
    checkIt(ks, cs, &config->users);

    char s[100] = "anton";
    char c[100] = "redrussian";
    checkIt(s, c, &config->users);


    char a[100] = "admin";
    char b[100] = "INFO";
    checkIt(a, b, &config->users);


    ConfDestroy(config);
    return 0;
}



