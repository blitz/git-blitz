#pragma once

#include <utility>
#include <vector>
#include <string>
#include <experimental/optional>

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

  template <typename... ARGS>
  static auto wrap(void (*fn)(ARGS...))
  {
    return fn;
  }

  class Reference;
  using OptionalReference = std::experimental::optional<Reference>;

  class Reference {
    git_reference *ref;

  public:
    Reference(git_reference *ref_) : ref(ref_) {}
    ~Reference();

    Reference(Reference const &src) = delete;
    Reference &operator=(Reference const &src) = delete;

    Reference(Reference &&src) : ref(nullptr) { std::swap(ref, src.ref); }
    Reference &operator=(Reference &&src) { std::swap(ref, src.ref); return *this; }

    // Wraps git_branch_name.
    std::string branch_name();

    // Wraps git_branch_upstream. Only works on local branches.
    OptionalReference branch_upstream();
  };

  // Wraps git_branch_iterator.
  std::vector<Reference> branches(git_repository *repo, git_branch_t list_flags);
}

// EOF
