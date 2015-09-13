#include <cstdio>
#include "git.hpp"

static const auto g_repository_open = Git::wrap(git_repository_open);
static const auto g_oid_fromstr     = Git::wrap(git_oid_fromstr);

int main(int argc, char **argv)
{
  git_repository *repo = nullptr;

  git_libgit2_init();

  try {
    g_repository_open(&repo, ".");

  } catch (Git::Error const &ge) {
    fprintf(stderr, "Git error: %s\n", ge.error->message);
  }

  git_libgit2_shutdown();
  
  return 0;
}
