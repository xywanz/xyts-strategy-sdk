#pragma once

#include <cstdio>

namespace xyts {

const char* kGitCommitInfo = "hash: f5c03ca1c3d86eb4cf6a37b5f18b043529cebc96 date: 2024-09-25 10:51:48 +0800 ref:  (HEAD -> master, local/master)\n";
const char* kCxxCompilerVersion = "g++-11 (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0";
const char* kCMakeCompilerId = "GNU";
const char* kCMakeCxxCompiler = "/usr/bin/g++-11";

void PrintVersion() {
  printf("compiler info: %s %s %s\n", kCMakeCompilerId, kCMakeCxxCompiler, kCxxCompilerVersion);
  printf("git      info: %s\n", kGitCommitInfo);
}

}  // namespace xyts
