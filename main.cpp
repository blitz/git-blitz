#include <cstdlib>

#include <algorithm>
#include <iostream>

#include <boost/program_options.hpp>
#include <boost/format.hpp>

#include "version.hpp"
#include "git.hpp"

namespace po = boost::program_options;
using format = boost::format;

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

    auto branches = Git::branches(repo, GIT_BRANCH_LOCAL);
    std::sort(branches.begin(), branches.end(), ref_time_larger_sort);

    for (auto &ref : branches) {
      auto commit   = Git::Commit::from_reference(repo, ref);
      auto upstream = ref.branch_upstream();
      std::cout << format("%1% tracks %2%\n") % ref.branch_name() % (upstream ? upstream->branch_name() : "nothing");
    }

  } catch (Git::Error const &ge) {
    std::cerr << format("Git error %1%: %2%\n") % ge.error_code % ge.error->message;
  }

  return 0;
}
