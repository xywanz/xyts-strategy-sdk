#pragma once

#include <cstdio>

namespace xyts {

const char* VERSION_INFO_GIT = "hash: 0fe7f342624148dd8861eb9c785edea7cfcf687c date: 2024-01-31 15:17:22 +0800 ref:  (HEAD -> master, local/master)\n";
const char* VERSION_INFO_CXX_COMPILER = "g++-11 (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0";
const char* CMAKE_CXX_COMPILER_ID = "GNU";
const char* CMAKE_CXX_COMPILER = "/usr/bin/g++-11";

void print_build_version() {
  printf("compiler info: %s %s %s\n", CMAKE_CXX_COMPILER_ID, CMAKE_CXX_COMPILER,
         VERSION_INFO_CXX_COMPILER);
  printf("git      info: %s\n", VERSION_INFO_GIT);
}

}  // namespace xyts
