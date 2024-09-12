#pragma once

namespace xyu {

struct MoveOnly {
  constexpr MoveOnly() = default;
  ~MoveOnly() = default;

  MoveOnly(MoveOnly&&) = default;
  MoveOnly& operator=(MoveOnly&&) = default;
  MoveOnly(const MoveOnly&) = delete;
  MoveOnly& operator=(const MoveOnly&) = delete;
};

struct NonCopyableNonMoveable {
  constexpr NonCopyableNonMoveable() = default;
  ~NonCopyableNonMoveable() = default;

  NonCopyableNonMoveable(const NonCopyableNonMoveable&) = delete;
  NonCopyableNonMoveable(NonCopyableNonMoveable&&) = delete;
  NonCopyableNonMoveable& operator=(NonCopyableNonMoveable const&) = delete;
  NonCopyableNonMoveable& operator=(NonCopyableNonMoveable&&) = delete;
};

}  // namespace xyu
