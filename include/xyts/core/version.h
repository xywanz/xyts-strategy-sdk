#pragma once

#include <cstdio>

namespace xyts {

const char* kGitCommitInfo = "hash: 4d86b4685c9687fe8cb6d0f0201f94e5a38f99c9 date: 2024-09-27 15:48:48 +0800 ref:  (HEAD -> master, local/master)\n";
const char* kCxxCompilerVersion = "g++-11 (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0";
const char* kCMakeCompilerId = "GNU";
const char* kCMakeCxxCompiler = "/usr/bin/g++-11";

void PrintVersion() {
  printf("compiler info: %s %s %s\n", kCMakeCompilerId, kCMakeCxxCompiler, kCxxCompilerVersion);
  printf("git      info: %s\n", kGitCommitInfo);
}

}  // namespace xyts
