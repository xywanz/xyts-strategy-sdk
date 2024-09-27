#pragma once

#include <cstdio>

namespace xyts {

const char* kGitCommitInfo = "hash: e7bae85de005918a97e43e3e3f088b4d5c0f1d67 date: 2024-09-27 11:23:45 +0800 ref:  (HEAD -> master, local/master)\n";
const char* kCxxCompilerVersion = "g++-11 (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0";
const char* kCMakeCompilerId = "GNU";
const char* kCMakeCxxCompiler = "/usr/bin/g++-11";

void PrintVersion() {
  printf("compiler info: %s %s %s\n", kCMakeCompilerId, kCMakeCxxCompiler, kCxxCompilerVersion);
  printf("git      info: %s\n", kGitCommitInfo);
}

}  // namespace xyts
