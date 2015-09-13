#include <cstdio>
#include <cstdlib>

#include "git.hpp"

static const auto g_repository_open = Git::wrap(git_repository_open);
static const auto g_oid_fromstr     = Git::wrap(git_oid_fromstr);

int main(int argc, char **argv)
{
  git_repository *repo = nullptr;

  git_libgit2_init();
  atexit([] { git_libgit2_shutdown(); });

  try {
    g_repository_open(&repo, ".");

    for (auto &ref : Git::branches(repo, GIT_BRANCH_ALL)) {
      printf("%s\n", ref.branch_name().c_str());
    }

  } catch (Git::Error const &ge) {
    fprintf(stderr, "Git error: %s\n", ge.error->message);
  }

  return 0;
}
