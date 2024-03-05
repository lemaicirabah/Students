#ifndef INF3173_WRAPPER_H
#define INF3173_WRAPPER_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream);
size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream);
ssize_t read(int fd, void* ptr, size_t size);
ssize_t write(int fd, const void* ptr, size_t size);
int pipe(int fd[2]);

void wrapper_clear();
int wrapper_fread_count();
int wrapper_fwrite_count();
int wrapper_read_count();
int wrapper_write_count();
int wrapper_read_size_max();
int wrapper_write_size_max();
int wrapper_pipe_count();

#ifdef __cplusplus
}
#endif

#endif
