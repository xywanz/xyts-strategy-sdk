#pragma once

#include <sys/types.h>

namespace xyu {

int FLockNoInt(int fd, int operation);

int FTruncateNoInt(int fd, off_t len);

}  // namespace xyu
