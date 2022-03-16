cmake_minimum_required(VERSION 3.14)

foreach(SUB_CMAKELISTS_DIR libsrt)
    add_subdirectory(${SUB_CMAKELISTS_DIR})
endforeach(SUB_CMAKELISTS_DIR)
