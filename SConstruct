# -*- Mode: Python -*-

OPTFLAGS = " -g -Os "

env = Environment(CCFLAGS   = OPTFLAGS,
                  LINKFLAGS = OPTFLAGS,
                  CXX       = "clang++",
                  LINK      = "clang++",
                  CXXFLAGS  = " -std=c++14 ")

# We use libgit2 for git repository access.
env.ParseConfig("pkg-config libgit2 --cflags --libs")


env.Program("git-blitz", ["main.cpp", "git.cpp"])

# EOF
