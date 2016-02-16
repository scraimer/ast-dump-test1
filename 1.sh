#!/bin/bash

/home/shalom/bin/toolchains/clang-3.7/bin/clang++ -x c++ -fsyntax-only  -I. -I.. -Iinclude -Iincludes -I../include -I../includes  -I/home/shalom/sandbox/clang-llvm/build/include -I/home/shalom/sandbox/clang-llvm/build/tools/clang/include -DCLANG_ENABLE_ARCMT -DCLANG_ENABLE_OBJC_REWRITER -DCLANG_ENABLE_STATIC_ANALYZER ''-DGTEST_HAS_RTTI=0'' -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -fvisibility-inlines-hidden -Wall -W -Wno-unused-parameter -Wwrite-strings -Wcast-qual -Wno-missing-field-initializers -pedantic -Wno-long-long -Wno-maybe-uninitialized -Wdelete-non-virtual-dtor -Wno-comment ''-std=c++11'' -ffunction-sections -fdata-sections -fno-common -Woverloaded-virtual -fno-strict-aliasing -DNDEBUG -fno-exceptions loop-convert/LoopConvert.cpp


