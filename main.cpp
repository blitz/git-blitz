#include <cstdio>
#include <cstdlib>

#include <algorithm>

#include "git.hpp"

static const auto g_repository_open = Git::wrap(git_repository_open);
static const auto g_oid_fromstr     = Git::wrap(git_oid_fromstr);

int main(int argc, char **argv)
{
  git_libgit2_init();
  atexit([] { git_libgit2_shutdown(); });

  try {
    auto repo = Git::Repository::from_path(".");

    auto const ref_time_larger_sort = [&repo] (Git::Reference const &a, Git::Reference const &b) -> bool {
      auto commit_a = Git::Commit::from_reference(repo, a);
      auto commit_b = Git::Commit::from_reference(repo, b);

      if (not commit_a and not commit_b) { return false; }
      if (not commit_a or  not commit_b) { return not commit_a; }

      return commit_a->time() > commit_b->time();
    };

    auto branches = Git::branches(repo, GIT_BRANCH_LOCAL);
    std::sort(branches.begin(), branches.end(), ref_time_larger_sort);

    for (auto &ref : branches) {
      auto commit   = Git::Commit::from_reference(repo, ref);
      auto upstream = ref.branch_upstream();
      printf("%s tracks %s\n", ref.branch_name().c_str(), upstream ? upstream->branch_name().c_str() : "nothing");
    }

  } catch (Git::Error const &ge) {
    fprintf(stderr, "Git error %d: %s\n", ge.error_code, ge.error->message);
  }

  return 0;
}
