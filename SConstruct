# -*- Mode: Python -*-

OPTFLAGS = " -Os "

env = Environment(CCFLAGS   = OPTFLAGS,
                  LINKFLAGS = OPTFLAGS,
                  CXX       = "clang++",
                  LINK      = "clang++",
                  CXXFLAGS  = " -std=c++14 ")

AlwaysBuild(Command('version.inc', ['version.hpp'],
                    """( cd `dirname $SOURCE` && git describe --always --dirty || echo UNKNOWN ) | sed 's/^\\(.*\\)$/"\\1"/' > $TARGET"""))

# We use libgit2 for git repository access.
env.ParseConfig("pkg-config libgit2 --cflags --libs")

# We use Boost::program_options
env['LIBS'].append('boost_program_options')

env.Program("git-blitz", Glob("*.cpp"))

# EOF
