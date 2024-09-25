#pragma once

#include <cstdio>

namespace xyts {

const char* kGitCommitInfo = "hash: 8165e92bc2d0d0f7ab70bbc9a85f3c90eebd0252 date: 2024-09-25 16:09:32 +0800 ref:  (HEAD -> master, local/master)\n";
const char* kCxxCompilerVersion = "g++-11 (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0";
const char* kCMakeCompilerId = "GNU";
const char* kCMakeCxxCompiler = "/usr/bin/g++-11";

void PrintVersion() {
  printf("compiler info: %s %s %s\n", kCMakeCompilerId, kCMakeCxxCompiler, kCxxCompilerVersion);
  printf("git      info: %s\n", kGitCommitInfo);
}

}  // namespace xyts
