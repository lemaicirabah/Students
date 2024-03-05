#include <gtest/gtest.h>

#include <filesystem>

#include "inf3173.h"
#include "kompress.h"
#include "testutils.h"

struct KompressParam {
    std::string method;
    int file_size;
    int block_size;
    std::string name;
    uint32_t deflated_size;
    uint32_t deflated_checksum;
};

class KompressTest : public testing::TestWithParam<struct KompressParam> {
  protected:
    std::string basename;
    std::string f1;
    std::string f2;
    std::string f3;

    void SetUp() override {
        struct KompressParam param = GetParam();
        // std::cout << "SetUp " << param.method << " " << param.name << std::endl;
        // Créer le fichier d'entrée
        basename = std::string(BINARY_DIR) + "testdata_" + param.method + "_"
                   + param.name;
        f1 = basename + ".a";
        f2 = basename + ".b";
        f3 = basename + ".c";
        generate_file(f1, param.file_size, 42);
    }

    void TearDown() override {
        std::cout << "TearDown" << std::endl;
    }
};

TEST_P(KompressTest, Deflate) {
    struct KompressParam param = GetParam();
    std::cout << "Deflate " << param.method << " " << param.name << std::endl;

    struct kompress_lib* lib = kompress_get_lib(param.method.c_str());
    struct kompress_app app = {.src_path = f1.c_str(),
                               .dst_path = f2.c_str(),
                               .block_size = param.block_size,
                               .level = 1};
    EXPECT_EQ(lib->deflate(&app), 0);

    // Vérifier la taille du fichier compressé
    EXPECT_EQ(std::filesystem::file_size(f2), param.deflated_size);

    // Vérifier le contenu du fichier (avec une somme de contrôle)
    EXPECT_EQ(file_checksum(f2), param.deflated_checksum);
}

INSTANTIATE_TEST_SUITE_P(
    KompressTestInstantiation, KompressTest,
    testing::Values(
        // stdio
        KompressParam{"stdio", 128, 32, "BlockSize32", 139, 576167732},
        KompressParam{"stdio", 128, 15, "Block15", 139, 576167732},
        KompressParam{"stdio", 128, 256, "LargeBlock", 139, 576167732},
        KompressParam{"stdio", 1073, 32, "OddFileSize", 305, 2265013641},

        // readwrite
        KompressParam{"readwrite", 128, 32, "BlockSize32", 139, 576167732},
        KompressParam{"readwrite", 128, 15, "Block15", 139, 576167732},
        KompressParam{"readwrite", 128, 256, "LargeBlock", 139, 576167732},
        KompressParam{"readwrite", 1073, 32, "OddFileSize", 305, 2265013641},

        // mmap
        KompressParam{"mmap", 128, 32, "BlockSize32", 139, 576167732},
        KompressParam{"mmap", 128, 15, "Block15", 139, 576167732},
        KompressParam{"mmap", 128, 256, "LargeBlock", 139, 576167732},
        KompressParam{"mmap", 1073, 32, "OddFileSize", 305, 2265013641}),

    [](const testing::TestParamInfo<KompressTest::ParamType>& info) {
        std::string name = info.param.method + info.param.name;
        return name;
    });

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
