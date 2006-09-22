// Copyright (c) 2006, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <stdio.h>
#include <string>
#include "processor/source_line_resolver.h"
#include "google/stack_frame.h"

using std::string;
using google_airbag::SourceLineResolver;
using google_airbag::StackFrame;

#define ASSERT_TRUE(cond) \
  if (!(cond)) {                                                        \
    fprintf(stderr, "FAILED: %s at %s:%d\n", #cond, __FILE__, __LINE__); \
    return false; \
  }

#define ASSERT_FALSE(cond) ASSERT_TRUE(!(cond))

#define ASSERT_EQ(e1, e2) ASSERT_TRUE((e1) == (e2))

static bool VerifyEmpty(const StackFrame &frame) {
  ASSERT_TRUE(frame.function_name.empty());
  ASSERT_TRUE(frame.source_file_name.empty());
  ASSERT_EQ(frame.source_line, 0);
  return true;
}

static void ClearSourceLineInfo(StackFrame *frame) {
  frame->function_name.clear();
  frame->source_file_name.clear();
  frame->source_line = 0;
}

static bool RunTests() {
  string testdata_dir = string(getenv("srcdir") ? getenv("srcdir") : ".") +
                        "/src/processor/testdata";

  SourceLineResolver resolver;
  ASSERT_TRUE(resolver.LoadModule("module1", testdata_dir + "/module1.out"));
  ASSERT_TRUE(resolver.LoadModule("module2", testdata_dir + "/module2.out"));

  StackFrame frame;
  frame.instruction = 0x1000;
  frame.module_name = "module1";
  resolver.FillSourceLineInfo(&frame);
  ASSERT_EQ(frame.function_name, "Function1_1");
  ASSERT_EQ(frame.source_file_name, "file1_1.cc");
  ASSERT_EQ(frame.source_line, 44);

  ClearSourceLineInfo(&frame);
  frame.instruction = 0x800;
  resolver.FillSourceLineInfo(&frame);
  ASSERT_TRUE(VerifyEmpty(frame));

  frame.instruction = 0x1280;
  resolver.FillSourceLineInfo(&frame);
  ASSERT_EQ(frame.function_name, "Function1_3");
  ASSERT_TRUE(frame.source_file_name.empty());
  ASSERT_EQ(frame.source_line, 0);

  frame.instruction = 0x1380;
  resolver.FillSourceLineInfo(&frame);
  ASSERT_EQ(frame.function_name, "Function1_4");
  ASSERT_TRUE(frame.source_file_name.empty());
  ASSERT_EQ(frame.source_line, 0);

  frame.instruction = 0x2180;
  frame.module_name = "module2";
  resolver.FillSourceLineInfo(&frame);
  ASSERT_EQ(frame.function_name, "Function2_2");
  ASSERT_EQ(frame.source_file_name, "file2_2.cc");
  ASSERT_EQ(frame.source_line, 21);

  ASSERT_FALSE(resolver.LoadModule("module3",
                                   testdata_dir + "/module3_bad.out"));
  ASSERT_FALSE(resolver.LoadModule("module4",
                                   testdata_dir + "/invalid-filename"));
  return true;
}

int main(int argc, char **argv) {
  if (!RunTests()) {
    return 1;
  }
  return 0;
}