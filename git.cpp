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

}

// EOF
