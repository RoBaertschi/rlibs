#include <stdlib.h>
#include <stdio.h>
#define RDIR_IMPLEMENTATION
#include "../rdir.h"


int main(void) {
    struct rdir_dir* dir = rdir_open_dir(".");

    struct rdir_entry* entry;
    while ((entry = rdir_read_dir(dir)) != NULL) {
        printf("entry: %s \ttype: %s\n", entry->name, rdir_entrytype_str(entry->type));
    }
    return EXIT_SUCCESS;
}
