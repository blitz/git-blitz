#include <git2.h>
#include <cstdio>
#include <utility>
#include <functional>

struct GitError {
  const int        error_code;
  const git_error *error;

  GitError(int ec)
    : error_code(ec), error(giterr_last())
  {}
};

void check_git_error(int error)
{
  if (error < 0) {
    throw GitError(error);
  }
}

template <typename... ARGS>
std::function<void(ARGS...)> git_wrap(int (*fn)(ARGS...))
{
  return [fn] (ARGS... a) {
    check_git_error(fn(std::forward<ARGS>(a)...));
  };
}

const auto g_repository_open = git_wrap(&git_repository_open);
const auto g_oid_fromstr = git_wrap(&git_oid_fromstr);

int main(int argc, char **argv)
{
  git_repository *repo = nullptr;

  git_libgit2_init();

  try {
    g_repository_open(&repo, ".");

  } catch (GitError const &ge) {
    fprintf(stderr, "Git error: %s\n", ge.error->message);
  }

  git_libgit2_shutdown();
  
  return 0;
}
