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

// stackwalker.h: Generic stackwalker.
//
// The Stackwalker class is an abstract base class providing common generic
// methods that apply to stacks from all systems.  Specific implementations
// will extend this class by providing GetContextFrame and GetCallerFrame
// methods to fill in system-specific data in a StackFrame structure.
// Stackwalker assembles these StackFrame strucutres into a vector of
// StackFrames.
//
// Author: Mark Mentovai


#ifndef PROCESSOR_STACKWALKER_H__
#define PROCESSOR_STACKWALKER_H__


#include "google/stack_frame.h"
#include "processor/memory_region.h"


namespace google_airbag {


class MinidumpModuleList;
class SymbolSupplier;


class Stackwalker {
 public:
  virtual ~Stackwalker() {}

  // Fills the given vector of StackFrames by calling GetContextFrame and
  // GetCallerFrame, and populating the returned frames with all available
  // data.
  void Walk(StackFrames *frames);

 protected:
  // memory identifies a MemoryRegion that provides the stack memory
  // for the stack to walk.  modules, if non-NULL, is a MinidumpModuleList
  // that is used to look up which code module each stack frame is
  // associated with.  supplier is an optional caller-supplied SymbolSupplier
  // implementation.  If supplier is NULL, source line info will not be
  // resolved.
  Stackwalker(MemoryRegion* memory,
              MinidumpModuleList* modules,
              SymbolSupplier* supplier);

  // The stack memory to walk.  Subclasses will require this region to
  // get information from the stack.
  MemoryRegion* memory_;

 private:
  // Obtains the context frame, the innermost called procedure in a stack
  // trace.  Returns false on failure.
  virtual bool GetContextFrame(StackFrame* frame) = 0;

  // Obtains a caller frame.  Each call to GetCallerFrame should return the
  // frame that called the last frame returned by GetContextFrame or
  // GetCallerFrame.  GetCallerFrame should return false on failure or
  // when there are no more caller frames (when the end of the stack has
  // been reached).
  virtual bool GetCallerFrame(StackFrame* frame) = 0;

  // A list of modules, for populating each StackFrame's module information.
  // This field is optional and may be NULL.
  MinidumpModuleList* modules_;

  // The optional SymbolSupplier for resolving source line info.
  SymbolSupplier* supplier_;
};


} // namespace google_airbag


#endif // PROCESSOR_STACKWALKER_H__