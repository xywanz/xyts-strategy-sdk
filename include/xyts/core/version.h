#pragma once

#include <cstdio>

namespace xyts {

const char* kGitCommitInfo = "hash: 5d2eb5fa3a1c14e7df67b82c684cd9425031f100 date: 2024-09-26 15:55:35 +0800 ref:  (HEAD -> master, local/master)\n";
const char* kCxxCompilerVersion = "g++-11 (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0";
const char* kCMakeCompilerId = "GNU";
const char* kCMakeCxxCompiler = "/usr/bin/g++-11";

void PrintVersion() {
  printf("compiler info: %s %s %s\n", kCMakeCompilerId, kCMakeCxxCompiler, kCxxCompilerVersion);
  printf("git      info: %s\n", kGitCommitInfo);
}

}  // namespace xyts
