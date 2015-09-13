#include "git.hpp"

namespace Git {

  Error::Error(int ec)
    : error_code(ec), error(giterr_last())
  {}
  
  void check_git_error(int error)
  {
    if (error < 0) {
      throw Error(error);
    }
  }

  Reference::~Reference()
  {
    if (ref) {
      wrap(git_reference_free)(ref);
    }
  }

  std::string Reference::branch_name()
  {
    const char *name = nullptr;

    wrap(git_branch_name)(&name, ref);

    return name;
  }

  OptionalReference Reference::branch_upstream()
  {
    git_reference *out;
    int error = git_branch_upstream(&out, ref);

    if (error != GIT_ENOTFOUND) {
      check_git_error(error);
      return Reference(out);
    } else {
      return {};
    }
  }

  std::vector<Reference> branches(git_repository *repo, git_branch_t list_flags)
  {
    std::vector<Reference> result;
    git_branch_iterator *iter = nullptr;

    try {
      wrap(git_branch_iterator_new)(&iter, repo, list_flags);

      git_reference *ref;
      git_branch_t   ref_type;

      int error;
      while ((0 == (error = git_branch_next(&ref, &ref_type, iter)))) {
        result.emplace_back(ref);
      }

      git_branch_iterator_free(iter);

      if (error != GIT_ITEROVER) {
        throw Error(error);
      }
    } catch (Error const &) {
      if (iter) git_branch_iterator_free(iter);
      throw;
    }

    return result;
  }
}

// EOF
