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

  // This is just here to make you think less when you wrap git_*
  // library calls.
  template <typename... ARGS>
  static auto wrap(void (*fn)(ARGS...))
  {
    return fn;
  }

  class Reference;
  class Commit;
  using OptionalReference = std::experimental::optional<Reference>;
  using OptionalCommit    = std::experimental::optional<Commit>;
  using OptionalOID       = std::experimental::optional<git_oid>;


  class Repository {
    git_repository *repo;

  public:
    Repository(git_repository *repo_) : repo(repo_) {}
    ~Repository();

    Repository(Repository const &src) = delete;
    Repository &operator=(Repository const &src) = delete;

    Repository(Repository &&src) : repo(nullptr) { std::swap(repo, src.repo); }
    Repository &operator=(Repository &&src) { std::swap(repo, src.repo); return *this; }

    static Repository from_path(const char *path);

    operator git_repository *() { return repo; }
  };

  class Reference {
    git_reference *ref;

  public:
    Reference(git_reference *ref_) : ref(ref_) {}
    ~Reference();

    Reference(Reference const &src) = delete;
    Reference &operator=(Reference const &src) = delete;

    Reference(Reference &&src) : ref(nullptr) { std::swap(ref, src.ref); }
    Reference &operator=(Reference &&src) { std::swap(ref, src.ref); return *this; }

    operator git_reference *() { return ref; }

    // Wraps git_branch_name.
    std::string branch_name() const;

    // Wraps git_branch_upstream. Only works on local branches.
    OptionalReference branch_upstream() const;

    // Wraps git_reference_target. OID is passed by value. There is no
    // ownership relationship between this Reference and the returned
    // OID.
    OptionalOID target() const;
  };

  class Commit {
    git_commit *commit;

  public:
    Commit(Commit const &src) = delete;
    Commit &operator=(Commit const &src) = delete;

    Commit(Commit &&src) : commit(nullptr) { std::swap(commit, src.commit); }
    Commit &operator=(Commit &&src) { std::swap(commit, src.commit); return *this; }

    operator git_commit *() { return commit; }

    git_time_t time() const { return git_commit_time(commit); }

    Commit(git_commit *commit_) : commit(commit_) { }
    ~Commit();

    static OptionalCommit from_reference(git_repository *repo, Reference const &ref);
  };


  // Wraps git_branch_iterator.
  std::vector<Reference> branches(git_repository *repo, git_branch_t list_flags);
}

// EOF
