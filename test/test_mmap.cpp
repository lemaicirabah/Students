#include <fcntl.h>
#include <gtest/gtest.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

TEST(Mmap, Depassement) {
    // Écrire au delà de la taille du fichier (mais à l'intérieur de la page) ne
    // cause pas SIGSEGV, mais le contenu écrit au delà n'est pas enregistré
    // dans le fichier.
    int block_size = 100;

    int fd = open("mmap_depassement.dat", O_CREAT | O_TRUNC | O_RDWR, 0644);
    ftruncate(fd, block_size);

    void* buf = mmap(NULL,                    //
                     block_size,              //
                     PROT_READ | PROT_WRITE,  //
                     MAP_SHARED,              //
                     fd,                      //
                     0);
    close(fd);

    char* data = (char*)buf;
    for (int i = 0; i < 4096; i++) {
        data[i] = i;
    }
    munmap(buf, block_size);
}

TEST(Mmap, Boucle) {
    int block_size = 10;
    int file_size = block_size;

    int fd = open("mmap_boucle.dat", O_CREAT | O_TRUNC | O_RDWR, 0644);

    // minimum 1 block
    ftruncate(fd, file_size);
    void* buf = mmap(NULL,                    //
                     file_size,               //
                     PROT_READ | PROT_WRITE,  //
                     MAP_SHARED,              //
                     fd,                      //
                     0);

    // Écrire 10 blocs (total de 100 octets)
    int val = 0;
    int n_block = 10;
    int pos = 0;
    int page_size = 4096;
    for (int i = 0; i < n_block; i++) {
        // Agrandir le fichier d'un bloc
        int new_file_size = pos + block_size;
        if (new_file_size > file_size) {
            munmap(buf, block_size);
            ftruncate(fd, new_file_size);
            file_size = new_file_size;
            int offset = pos - (pos % page_size);
            buf = mmap(NULL,                    //
                       block_size,              //
                       PROT_READ | PROT_WRITE,  //
                       MAP_SHARED,              //
                       fd,                      //
                       offset);
        }

        char* data = (char*)buf + (pos % page_size);
        for (int i = 0; i < block_size; i++) {
            data[i] = val++;
        }
        pos += block_size;
    }

    close(fd);
    munmap(buf, block_size);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
