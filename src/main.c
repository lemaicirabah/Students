#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kompress.h"
#include "kompress_stdio.h"

void print_usage() {
    fprintf(stderr,
            "Usage: %s "
            "[deflate|inflate] "
            "input "
            "output "
            "--method [mmap|stdio|readwrite] "
            "--block-size N "
            "--help]\n",
            "kompress");
    exit(1);
}

int main(int argc, char** argv) {
    struct option options[] = {
        {"method", 1, 0, 'm'},      //
        {"block-size", 1, 0, 's'},  //
        {"level", 1, 0, 'l'},       //
        {"help", 0, 0, 'h'},        //
        {0, 0, 0, 0}                //
    };

    struct kompress_app app = {
        .operation = NULL,
        .method = "stdio",
        .src_path = NULL,
        .dst_path = NULL,
        .block_size = 1024,
        .level = 1,
    };

    int opt;
    int idx;
    while ((opt = getopt_long(argc, argv, "m:s:l:h", options, &idx)) != -1) {
        switch (opt) {
            case 'm':
                app.method = optarg;
                break;
            case 's':
                app.block_size = atoi(optarg);
                break;
            case 'l':
                app.level = atoi(optarg);
                break;
            case 'h':
            default:
                print_usage();
        }
    }

    if (argc - optind < 3) {
        print_usage();
    }

    app.operation = argv[optind + 0];
    app.src_path = argv[optind + 1];
    app.dst_path = argv[optind + 2];

    // Affichage des options
    {
        printf("options:\n");
        printf("  operation: %s\n", app.operation);
        printf("  method: %s\n", app.method);
        printf("  src_path: %s\n", app.src_path);
        printf("  dst_path: %s\n", app.dst_path);
        printf("  block_size: %d\n", app.block_size);
        printf("  level: %d\n", app.level);
    }
    fflush(stdout);

    // Obtenir la structure qui correspond à la méthode
    struct kompress_lib* lib = kompress_get_lib(app.method);
    if (!lib) {
        printf("ERREUR: methode invalide: %s\n", app.method);
        print_usage();
    }

    int res = 0;
    if (strcmp(app.operation, "deflate") == 0) {
        res = lib->deflate(&app);
    } else if (strcmp(app.operation, "inflate") == 0) {
        res = lib->inflate(&app);
    } else {
        printf("unkown operation\n");
    }

    return res == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
