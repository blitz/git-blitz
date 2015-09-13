#pragma once

#include <utility>

#include <git2.h>

namespace Git {

  struct Error {
    const int        error_code;
    const git_error *error;

    Error(int ec);
  };

  void check_git_error(int error);

  template <typename... ARGS>
  static auto wrap(int (*fn)(ARGS...))
  {
    return [fn] (ARGS... a) {
      check_git_error(fn(std::forward<ARGS>(a)...));
    };
  }

}

// EOF
