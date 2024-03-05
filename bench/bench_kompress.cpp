#include <benchmark/benchmark.h>

#include <filesystem>
#include <iostream>

#include "inf3173.h"
#include "kompress.h"
#include "testutils.h"

static const char* datadir = BINARY_DIR "/bench";
static const char* fname_deflate_src = BINARY_DIR "/bench/deflate_src.dat";
static const char* fname_deflate_dst = BINARY_DIR "/bench/deflate_dst.dat";
static const char* fname_inflate_src = BINARY_DIR "/bench/inflate_src.dat";
static const char* fname_inflate_dst = BINARY_DIR "/bench/inflate_dst.dat";
static const int fsize = (1 << 20);  // 1 Mo


class KompressFixture : public benchmark::Fixture
{
  public:

    struct kompress_app app_deflate;
    struct kompress_app app_inflate;

    void SetUp(::benchmark::State& state) {

        if (!std::filesystem::is_directory(datadir)) {
            std::filesystem::create_directory(datadir);
        }

        if (!std::filesystem::is_regular_file(fname_deflate_src)) {
            if (generate_file(fname_deflate_src, fsize, 0) < 0) {
                state.SkipWithError("Echec creation fichier");
            }
        }

        app_deflate = {
            .operation = NULL,
            .method = NULL,
            .src_path = fname_deflate_src,
            .dst_path = fname_deflate_dst,
            .block_size = 0,
            .level = 1,
        };

        app_inflate = {
            .operation = NULL,
            .method = NULL,
            .src_path = fname_inflate_src,
            .dst_path = fname_inflate_dst,
            .block_size = 0,
            .level = 1,
        };

        // Création du fichier compressé pour benchmark inflate
        // avec l'implémentation de référence stdio
        struct kompress_lib *lib = kompress_get_lib("stdio");
        struct kompress_app tmp = {
            .operation = NULL,
            .method = NULL,
            .src_path = fname_deflate_src,
            .dst_path = fname_inflate_src,
            .block_size = 1024,
            .level = 1,
        };

        if (lib->deflate(&tmp) < 0) {
            state.SkipWithError("Echec compression fichier");
        }
    }

};

/*
 * DEFLATE STDIO
 */
BENCHMARK_DEFINE_F(KompressFixture, BM_KompressDeflateStdio)(benchmark::State& state) {
    struct kompress_lib *lib = kompress_get_lib("stdio");
    app_deflate.block_size = state.range(0);

    // Boucle de mesure
    for (auto _ : state) {
        (void)_;
        if (lib->deflate(&app_deflate) < 0) {
            state.SkipWithError("erreur deflate");
        }
    }

    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * fsize);
}

BENCHMARK_REGISTER_F(KompressFixture, BM_KompressDeflateStdio) //
    ->RangeMultiplier(2) //
    ->Range(1, (1 << 16));


/*
 * DEFLATE READWRITE
 */
BENCHMARK_DEFINE_F(KompressFixture, BM_KompressDeflateReadwrite)(benchmark::State& state) {
    struct kompress_lib *lib = kompress_get_lib("readwrite");
    app_deflate.block_size = state.range(0);

    // Boucle de mesure
    for (auto _ : state) {
        (void)_;
        if (lib->deflate(&app_deflate) < 0) {
            state.SkipWithError("erreur deflate");
        }
    }

    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * fsize);
}

BENCHMARK_REGISTER_F(KompressFixture, BM_KompressDeflateReadwrite) //
    ->RangeMultiplier(2) //
    ->Range(1, (1 << 16));

/*
 * DEFLATE MMAP
 */
BENCHMARK_DEFINE_F(KompressFixture, BM_KompressDeflateMMap)(benchmark::State& state) {
    struct kompress_lib *lib = kompress_get_lib("mmap");
    app_deflate.block_size = state.range(0);

    // Boucle de mesure
    for (auto _ : state) {
        (void)_;
        if (lib->deflate(&app_deflate) < 0) {
            state.SkipWithError("erreur deflate");
        }
    }

    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * fsize);
}

BENCHMARK_REGISTER_F(KompressFixture, BM_KompressDeflateMMap) //
    ->RangeMultiplier(2) //
    ->Range(1, (1 << 16));


/*
 * INFLATE STDIO
 */
BENCHMARK_DEFINE_F(KompressFixture, BM_KompressInflateStdio)(benchmark::State& state) {
    struct kompress_lib *lib = kompress_get_lib("stdio");
    app_inflate.block_size = state.range(0);

    // Boucle de mesure
    for (auto _ : state) {
        (void)_;
        if (lib->inflate(&app_inflate) < 0) {
            state.SkipWithError("erreur inflate");
        }
    }

    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * fsize);
}

BENCHMARK_REGISTER_F(KompressFixture, BM_KompressInflateStdio) //
    ->RangeMultiplier(2) //
    ->Range(1, (1 << 16));


/*
 * INFLATE READWRITE
 */
BENCHMARK_DEFINE_F(KompressFixture, BM_KompressInflateReadwrite)(benchmark::State& state) {
    struct kompress_lib *lib = kompress_get_lib("readwrite");
    app_inflate.block_size = state.range(0);

    // Boucle de mesure
    for (auto _ : state) {
        (void)_;
        if (lib->inflate(&app_inflate) < 0) {
            state.SkipWithError("erreur inflate");
        }
    }

    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * fsize);
}

BENCHMARK_REGISTER_F(KompressFixture, BM_KompressInflateReadwrite) //
    ->RangeMultiplier(2) //
    ->Range(1, (1 << 16));

/*
 * INFLATE MMAP
 */
BENCHMARK_DEFINE_F(KompressFixture, BM_KompressInflateMMap)(benchmark::State& state) {
    struct kompress_lib *lib = kompress_get_lib("mmap");
    app_inflate.block_size = state.range(0);

    // Boucle de mesure
    for (auto _ : state) {
        (void)_;
        if (lib->inflate(&app_inflate) < 0) {
            state.SkipWithError("erreur inflate");
        }
    }

    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * fsize);
}

BENCHMARK_REGISTER_F(KompressFixture, BM_KompressInflateMMap) //
    ->RangeMultiplier(2) //
    ->Range(1, (1 << 16));

BENCHMARK_MAIN();
