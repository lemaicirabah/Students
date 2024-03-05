#ifndef INF3173_KOMPRESS_H
#define INF3173_KOMPRESS_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Structure pour les options de l'application
 */
struct kompress_app {
    const char* operation;  // compression (deflate) ou décompression (inflate)
    const char* method;     // type de transfert (stdio, readwrite, mmap)
    const char* src_path;   // fichier d'entrée (source)
    const char* dst_path;   // fichier de sortie (destination)
    int block_size;         // taille des blocs de transfert (en octet)
    int level;              // niveau de compression
};

/*
 * Structure pour définir les variantes (méthodes)
 */
typedef int (*kompress_func)(struct kompress_app* app);

struct kompress_lib {
    const char* name;
    kompress_func inflate;
    kompress_func deflate;
};
extern struct kompress_lib kompress_libs[];

struct kompress_lib* kompress_get_lib(const char* name);

#ifdef __cplusplus
}
#endif

#endif
