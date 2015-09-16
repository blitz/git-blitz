#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <chrono>

#include <boost/program_options.hpp>
#include <boost/format.hpp>

#include "version.hpp"
#include "git.hpp"

namespace po = boost::program_options;
using format = boost::format;

static void erase_reference(std::vector<Git::Reference> &v, std::string const &name)
{
  auto found = std::find_if(v.begin(), v.end(), [&name] (Git::Reference const &r) { return name == r.branch_name(); });
  if (found != v.end()) {
    v.erase(found);

    // Erase all other references that are equal.
    erase_reference(v, name);
  }
}

int main(int argc, char **argv)
{

  po::options_description desc("Options");
  desc.add_options()
    ("help",    "produce help message")
    ("version", "produce version message");

  po::variables_map vm;
  bool parse_error = false;

  try {
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
  } catch (po::error const &e) {
    std::cerr << e.what() << std::endl;
    parse_error = true;
  }

  if (parse_error or vm.count("help") or vm.count("version")) {
      std::cout << format("%1%\n%2%\n") % banner % desc;
      return EXIT_SUCCESS;
  }


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

    auto remote_branches = Git::branches(repo, GIT_BRANCH_REMOTE);
    auto local_branches = Git::branches(repo, GIT_BRANCH_LOCAL);

    std::sort(local_branches.begin(), local_branches.end(), ref_time_larger_sort);

    time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    for (auto &ref : local_branches) {
      auto upstream = ref.branch_upstream();

      if (not upstream) {
        std::cout << format("%1% tracks nothing.\n") % ref.branch_name();
      } else {
        auto commit = Git::Commit::from_reference(repo, ref);
        auto mb     = Git::mergebase(repo, ref, *upstream);

        if (not mb) {
          std::cout << format("%1% tracks %2% (no mergebase?)\n") % ref.branch_name() % upstream->branch_name();
        } else {
          size_t ahead  = revwalk(repo, *mb, ref).size();
          size_t behind = revwalk(repo, *mb, *upstream).size();

          time_t age = now - commit->time();

          std::cout << format("%1% tracks %2% A%3%B%4%, last local commit %5%d ago\n") % ref.branch_name() % upstream->branch_name() % ahead % behind % (age / (3600 * 24));

          erase_reference(remote_branches, upstream->branch_name());
        }
      }
    }

    // remote_branches now contains all remote branches that didn't
    // turn up as upstream branches for any local branch.

    std::sort(remote_branches.begin(), remote_branches.end(), ref_time_larger_sort);

    for (auto &b : remote_branches) {
      std::string branch_name = b.branch_name();

      if ((branch_name.find("origin/") == std::string::npos) or
          (branch_name.find("/HEAD") != std::string::npos)) {
        continue;
      }

      std::cout << b.branch_name() << " is not pointed to by any local branch\n";
    }


  } catch (Git::Error const &ge) {
    std::cerr << format("Git error %1%: %2%\n") % ge.error_code % ge.error->message;
  }

  return 0;
}
