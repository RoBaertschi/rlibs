// ml2 ft=c
#ifndef RDIR_H_
#define RDIR_H_

#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
enum rdir_entrytypes {
    RDIR_ENTRYTYPE_FILE = 0,
    RDIR_ENTRYTYPE_SYMLINK,
    RDIR_ENTRYTYPE_DIRECTORY,
    // Unknown or not suported entry type
    RDIR_ENTRYTYPE_UNKNOWN,

    RDIR_ENTRYTYPE_MAX,
};

struct rdir_dir;
struct rdir_entry {
    char* name;
    int type;
};

// Returns NULL on error.
struct rdir_dir *rdir_open_dir(const char* dir);
// Returns NULL if it has reached the end of the directory or on an error. There is no difference currently.
struct rdir_entry *rdir_read_dir(struct rdir_dir* dir);
// Creates a dir, returns false on error.
bool rdir_mdkir(const char* dir);
// Returns NULL on error. User must free returned buffer after use.
char *rdir_getcwd();

void rdir_destroy_dir(struct rdir_dir* dir);
void rdir_destroy_entry(struct rdir_entry* entry);

// Entry has to be valid. This will be asserted.
const char* rdir_entrytype_str(enum rdir_entrytypes entry);

#ifndef RDIR_IMPLEMENTATION
#include <stdlib.h>

#if defined (__unix__) || defined (__APPLE__) && defined (__MACH__)
#define RDIR_POSIX
#define _POSIX_C_SOURCE 200809L

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#elif defined (_WIN32) || defined (_WIN64)
#include <windows.h>

#define RDIR_WIN
#else
#error "Unsupported operating system"
#endif

struct rdir_dir {
#ifdef RDIR_POSIX
    DIR* posix_dir;
#elif defined(RDIR_WIN)
    HANDLE windows_dir;
#endif // RDIR_POSIX
};

// Returns NULL on error.
struct rdir_dir *rdir_open_dir(const char* directory) {
    struct rdir_dir *dir = (struct rdir_dir*) malloc(sizeof(struct rdir_dir));
    if (dir == NULL) {
        return dir;
    }
    memset(dir, 0, sizeof(struct rdir_dir));

#ifdef RDIR_POSIX
    dir->posix_dir = opendir(directory);
#elif defined(RDIR_WIN)
    size_t directory_wchar_len = _mbstowcs_l(NULL, directory, 0, NULL);

    if (directory_wchar_len == -1) {
        return NULL;
    }

    wchar_t* buffer = malloc((directory_wchar_len + 1) * sizeof(wchar_t));
    directory_wchar_len = _mbstowcs_l(buffer, directory, directory_wchar_len + 1, NULL);

    size_t dirname_len = strlen(directory);
    wchar_t* buffer = malloc(dirname_len);

    dir->windows_dir = CreateFile(mbstowcs);
#endif // RDIR_POSIX

    return dir;
}

struct rdir_entry *rdir_read_dir(struct rdir_dir* dir) {
    struct rdir_entry *entry = (struct rdir_entry*) malloc(sizeof(struct rdir_dir));
    if (entry == NULL) {
        return entry;
    }

#ifdef RDIR_POSIX
    struct dirent *posix_entry = readdir(dir->posix_dir);
    if (posix_entry == NULL) {
        return NULL;
    }

    entry->name = (char*) malloc(strlen(posix_entry->d_name) * sizeof(char));
    if (entry->name == NULL) {
        return NULL;
    }
    strcpy(entry->name, posix_entry->d_name);


    switch (posix_entry->d_type) {
    case DT_REG:
        entry->type = RDIR_ENTRYTYPE_FILE;
        break;
    case DT_LNK:
        entry->type = RDIR_ENTRYTYPE_SYMLINK;
        break;
    case DT_DIR:
        entry->type = RDIR_ENTRYTYPE_DIRECTORY;
        break;
    default:
        entry->type = RDIR_ENTRYTYPE_UNKNOWN;
        break;
    }
#endif // RDIR_POSIX

    return entry;
}

bool rdir_mdkir(const char* dir) {
#ifdef RDIR_POSIX
    if(mkdir(dir, 0755) != 0) {
        return false;
    }
#endif // RDIR_POSIX
    return true;
}

// Don't forget to free the returned buffer.
char *rdir_getcwd() {
#ifdef RDIR_POSIX
    // Snippet from https://www.gnu.org/software/libc/manual/html_node/Working-Directory.html
    size_t size = 128;

    while (true) {
        char *buffer = (char *) malloc(size);
        if (buffer == NULL) {
            return NULL;
        }
        if (getcwd(buffer, size) == buffer) {
            return buffer;
        }
        free(buffer);
        if (errno != ERANGE) {
            return NULL;
        }
        size *= 2;
    }
#endif // RDIR_POSIX
}

void rdir_destroy_dir(struct rdir_dir* dir) {
    free(dir);
}
void rdir_destroy_entry(struct rdir_entry* entry) {
    free(entry->name);
    free(entry);
}


const char* rdir_entrytype_str(enum rdir_entrytypes entry) {
    assert(entry < RDIR_ENTRYTYPE_MAX && "invalid rdir_entrytypes passed to rdir_entrytype_str");
    assert(entry >= 0 && "invalid rdir_entrytypes passed to rdir_entrytype_str");
    switch (entry) {
    case RDIR_ENTRYTYPE_FILE:
        return "file";
    case RDIR_ENTRYTYPE_SYMLINK:
        return "symlink";
    case RDIR_ENTRYTYPE_DIRECTORY:
        return "directory";
    case RDIR_ENTRYTYPE_UNKNOWN:
        return "unknown";
    case RDIR_ENTRYTYPE_MAX:
    default:
        return "invalid";
  }
}

#endif // RDIR_IMPLEMENTATION
#endif // RDIR_H_
