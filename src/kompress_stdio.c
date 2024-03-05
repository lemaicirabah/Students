#include "kompress_stdio.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "kompress_utils.h"
#include "miniz.h"

int kompress_deflate_stdio(struct kompress_app* app) {
    int ret = -1;
    z_stream stream;

    unsigned char* src_buf = malloc(app->block_size);
    if (src_buf == NULL) {
        printf("Erreur malloc source");
        goto err_malloc_src;
    }

    unsigned char* dst_buf = malloc(app->block_size);
    if (dst_buf == NULL) {
        printf("Erreur malloc destination");
        goto err_malloc_dst;
    }

    // Ouvrir les fichiers
    FILE* src_file = fopen(app->src_path, "rb");
    if (src_file == NULL) {
        printf("Erreur ouverture fichier source\n");
        goto err_open_src;
    }

    FILE* dst_file = fopen(app->dst_path, "wb");
    if (src_file == NULL) {
        printf("Erreur ouverture fichier destination\n");
        goto err_open_dst;
    }

    fseek(src_file, 0, SEEK_END);
    int src_file_size = ftell(src_file);
    fseek(src_file, 0, SEEK_SET);

    memset(&stream, 0, sizeof(stream));
    stream.next_in = src_buf;
    stream.avail_in = 0;
    stream.next_out = dst_buf;
    stream.avail_out = app->block_size;

    // boucle de compression
    uint src_remaining = src_file_size;
    if (deflateInit(&stream, app->level) != Z_OK) {
        printf("Erreur initialisation deflate\n");
        goto err_traitement;
    }

    while (1) {
        if (!stream.avail_in) {
            // l'entrée est traitée, lire un autre bloc, mais pas plus que la
            // fin du fichier
            uint chunk = my_min(app->block_size, src_remaining);

            if (fread(src_buf, 1, chunk, src_file) != chunk) {
                printf("Failed reading from input file!\n");
                goto err_traitement;
            }

            // printf("next_in=%p src_buf=%p\n", stream.next_in, src_buf);
            stream.next_in = src_buf;
            stream.avail_in = chunk;

            src_remaining -= chunk;
            // printf("Input bytes remaining: %u\n", src_remaining);
        }

        // Si c'est la fin du fichier, alors passer Z_FINISH pour finaliser la
        // sortie et indiquer qu'il n'y aura plus de données ensuite. Le flag
        // Z_NO_FLUSH fait en sorte la librairie décide quand il est optimal de
        // générer des données dans le tampon de sortie. Pour une compression
        // optimale, il est normal que la sortie soit en retard par rapport à
        // l'entrée.
        int flag = src_remaining ? Z_NO_FLUSH : Z_FINISH;

        // Appel principal à la fonction de compression
        int status = deflate(&stream, flag);

        if ((status == Z_STREAM_END) || (!stream.avail_out)) {
            // Le tampon de sortie est plein, ou compression terminée.
            // On doit écrire la sortie dans le fichier.

            // La librairie rapporte l'espace libre dans le tampon de sortie,
            // donc on doit calculer la taille en octets de la sortie prête à
            // écrire.
            uint chunk = app->block_size - stream.avail_out;
            if (fwrite(dst_buf, 1, chunk, dst_file) != chunk) {
                printf("Failed writing to output file!\n");
                goto err_traitement;
            }
            // printf("next_out=%p dst_buf=%p\n", stream.next_in, src_buf);
            stream.next_out = dst_buf;
            stream.avail_out = app->block_size;
        }

        if (status == Z_STREAM_END) {
            break;
        } else if (status != Z_OK) {
            printf("deflate() failed with status %i!\n", status);
            goto err_traitement;
        }
    }

    // Succès
    ret = 0;

err_traitement:
    fclose(dst_file);
err_open_dst:
    fclose(src_file);
err_open_src:
    free(dst_buf);
err_malloc_dst:
    free(src_buf);
err_malloc_src:
    return ret;
}

int kompress_inflate_stdio(struct kompress_app* app) {
    int ret = -1;
    z_stream stream;

    unsigned char* src_buf = malloc(app->block_size);
    if (src_buf == NULL) {
        printf("Erreur malloc source");
        goto err_malloc_src;
    }

    unsigned char* dst_buf = malloc(app->block_size);
    if (dst_buf == NULL) {
        printf("Erreur malloc destination");
        goto err_malloc_dst;
    }

    // Ouvrir les fichiers
    FILE* src_file = fopen(app->src_path, "rb");
    if (src_file == NULL) {
        printf("Erreur ouverture fichier source\n");
        goto err_open_src;
    }

    FILE* dst_file = fopen(app->dst_path, "wb");
    if (dst_file == NULL) {
        printf("Erreur ouverture fichier destination\n");
        goto err_open_dst;
    }

    fseek(src_file, 0, SEEK_END);
    int src_file_size = ftell(src_file);
    fseek(src_file, 0, SEEK_SET);

    memset(&stream, 0, sizeof(stream));
    stream.next_in = src_buf;
    stream.avail_in = 0;
    stream.next_out = dst_buf;
    stream.avail_out = app->block_size;

    // boucle de compression
    uint src_remaining = src_file_size;
    if (inflateInit(&stream) != Z_OK) {
        printf("Erreur initialisation inflate\n");
        goto err_traitement;
    }

    while (1) {
        if (!stream.avail_in) {
            // l'entrée est traitée, lire un autre bloc, mais pas plus que la
            // fin du fichier
            uint chunk = my_min(app->block_size, src_remaining);

            if (fread(src_buf, 1, chunk, src_file) != chunk) {
                printf("Failed reading from input file!\n");
                goto err_traitement;
            }

            // next_in est modifié (avancé)
            // printf("next_in=%p src_buf=%p\n", stream.next_in, src_buf);
            stream.next_in = src_buf;
            stream.avail_in = chunk;

            src_remaining -= chunk;
            // printf("Input bytes remaining: %u\n", src_remaining);
        }

        // Appel principal à la fonction de décompression
        // L'option Z_SYNC_FLUSH indique que l'on veut décompresser par bloc
        int status = inflate(&stream, Z_SYNC_FLUSH);

        if ((status == Z_STREAM_END) || (!stream.avail_out)) {
            // Le tampon de sortie est plein, ou compression terminée.
            // On doit écrire la sortie dans le fichier.

            // La librairie rapporte l'espace libre dans le tampon de sortie,
            // donc on doit calculer la taille en octets de la sortie prête à
            // écrire.
            uint chunk = app->block_size - stream.avail_out;
            if (fwrite(dst_buf, 1, chunk, dst_file) != chunk) {
                printf("Failed writing to output file!\n");
                goto err_traitement;
            }
            // printf("next_out=%p dst_buf=%p\n", stream.next_in, src_buf);
            stream.next_out = dst_buf;
            stream.avail_out = app->block_size;
        }

        if (status == Z_STREAM_END) {
            break;
        } else if (status != Z_OK) {
            printf("inflate() failed with status %i!\n", status);
            goto err_traitement;
        }
    }

    if (inflateEnd(&stream) != Z_OK) {
        printf("inflateEnd() failed!\n");
        goto err_traitement;
    }

    // Succès
    ret = 0;

err_traitement:
    fclose(dst_file);
err_open_dst:
    fclose(src_file);
err_open_src:
    free(dst_buf);
err_malloc_dst:
    free(src_buf);
err_malloc_src:
    return ret;
}
