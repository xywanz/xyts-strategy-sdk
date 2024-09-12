#pragma once

#include <x86intrin.h>

#include <cstdint>

namespace xyu {

#if defined(__s390__) || defined(__s390x__)
#define BOOST_LOCKFREE_CACHELINE_BYTES 256
#elif defined(powerpc) || defined(__powerpc__) || defined(__ppc__)
#define BOOST_LOCKFREE_CACHELINE_BYTES 128
#else
#define BOOST_LOCKFREE_CACHELINE_BYTES 64
#endif

constexpr std::size_t kCacheLineSize = BOOST_LOCKFREE_CACHELINE_BYTES;

inline void cpu_pause() { _mm_pause(); }

}  // namespace xyu
