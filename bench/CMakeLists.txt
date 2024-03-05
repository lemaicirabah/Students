add_executable(bench_kompress bench_kompress.cpp)
target_link_libraries(bench_kompress kompresslib testutils benchmark::benchmark)

add_custom_target(run_benchmark
    COMMAND $<TARGET_FILE:bench_kompress>
            --benchmark_out_format=json
            --benchmark_out=${CMAKE_SOURCE_DIR}/bench_kompress.json
    COMMENT "Running the benchmark program"
    DEPENDS bench_kompress
)
