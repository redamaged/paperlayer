#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/reda/Projects/paperlayer/cmake/build
  make -f /Users/reda/Projects/paperlayer/cmake/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/reda/Projects/paperlayer/cmake/build
  make -f /Users/reda/Projects/paperlayer/cmake/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/reda/Projects/paperlayer/cmake/build
  make -f /Users/reda/Projects/paperlayer/cmake/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/reda/Projects/paperlayer/cmake/build
  make -f /Users/reda/Projects/paperlayer/cmake/build/CMakeScripts/ReRunCMake.make
fi

