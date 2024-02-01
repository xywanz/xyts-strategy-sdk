#pragma once

#include <cstdio>

namespace xyts {

const char* VERSION_INFO_GIT = "hash: 0d2d56e5bff65de03daedcea2f4998d803fa11d0 date: 2024-02-01 11:26:05 +0800 ref:  (HEAD -> master, local/master)\n";
const char* VERSION_INFO_CXX_COMPILER = "g++-11 (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0";
const char* CMAKE_CXX_COMPILER_ID = "GNU";
const char* CMAKE_CXX_COMPILER = "/usr/bin/g++-11";

void print_build_version() {
  printf("compiler info: %s %s %s\n", CMAKE_CXX_COMPILER_ID, CMAKE_CXX_COMPILER,
         VERSION_INFO_CXX_COMPILER);
  printf("git      info: %s\n", VERSION_INFO_GIT);
}

}  // namespace xyts
