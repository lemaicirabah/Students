add_library(kompresslib
  miniz.c
  miniz.h

  kompress_mmap.c
  kompress_mmap.h

  kompress_stdio.c
  kompress_stdio.h

  kompress_readwrite.c
  kompress_readwrite.h

  kompress_utils.h

  kompress.h
  kompress.c
)
target_include_directories(kompresslib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(kompress main.c)
target_link_libraries(kompress kompresslib)
