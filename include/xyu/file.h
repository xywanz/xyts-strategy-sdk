#pragma once

#include <fcntl.h>

#include <string>

namespace xyu {

/**
 * @brief RAII File class
 *
 */
class File {
 public:
  File() noexcept;

  explicit File(int fd, bool owns_fd = false) noexcept;

  explicit File(const char* name, int flags = O_RDONLY, mode_t mod = 0666);
  explicit File(const std::string& name, int flags = O_RDONLY, mode_t mod = 0666);

  File(File&&) noexcept;
  File& operator=(File&&) noexcept;

  ~File();

  static File CreateTempFile();

  int fd() const { return fd_; }

  explicit operator bool() const { return fd_ != -1; }

  File Dup() const;
  File DupCloseOnExec() const;

  void Close();
  bool CloseNoThrow() noexcept;

  int Release() noexcept;

  void Lock();
  bool TryLock();
  void Unlock();

  // for std scope guard
  void lock() { return Lock(); }
  void unlock() { return Unlock(); }

  void LockShared();
  bool TryLockShared();
  void UnlockShared();

  void Swap(File& other) noexcept;

 private:
  void DoLock(int op);
  bool DoTryLock(int op);

  File(const File&) = delete;
  File& operator=(const File&) = delete;

  int fd_ = -1;
  bool owns_fd_ = false;
};

}  // namespace xyu
