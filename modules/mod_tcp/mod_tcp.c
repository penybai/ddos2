#include <ddos2/ddos2.h>
#include <ddos2/arguments.h>
#include <ddos2/hashtable.h>
#include <ddos2/message.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "socket.h"

char* mod_name="mod_tcp";
char* mod_author="DedAzaMarks@github.com";
char* mod_version="ERROR PRE ALPHA (WOULDN'T EVER WORK)";
char* mod_description="TCP implementation";


module_config_t* mod_config_pull(program_config_t* config){
   ddos2_begin(config);
   return ddos2_modconfig(mod_name,mod_author,mod_description,mod_version);
}

void mod_on_init(void){
   printf("mod_on_init() called.\n");
}
