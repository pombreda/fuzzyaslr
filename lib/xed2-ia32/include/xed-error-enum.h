/*BEGIN_LEGAL 
Intel Open Source License 

Copyright (c) 2002-2008 Intel Corporation 
All rights reserved. 
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.  Redistributions
in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.  Neither the name of
the Intel Corporation nor the names of its contributors may be used to
endorse or promote products derived from this software without
specific prior written permission.
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL OR
ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
END_LEGAL */
/// @file xed-error-enum.h
/// @author Mark Charney <mark.charney@intel.com>

// This file was automatically generated.
// Do not edit this file.

#if !defined(_XED_ERROR_ENUM_H_)
# define _XED_ERROR_ENUM_H_
#include "xed-common-hdrs.h"
typedef enum {
  XED_ERROR_NONE, ///< There was no error
  XED_ERROR_BUFFER_TOO_SHORT, ///< There were not enough bytes in the given buffer
  XED_ERROR_GENERAL_ERROR, ///< XED could not decode the given instruction
  XED_ERROR_BAD_REGISTER, ///< XED could not decode the given instruction because an invalid register encoding was used.
  XED_ERROR_BAD_LOCK_PREFIX, ///< A lock prefix was found where none is allowed.
  XED_ERROR_BAD_REP_PREFIX, ///< An F2 or F3 prefix was found where none is allowed.
  XED_ERROR_NO_OUTPUT_POINTER, ///< The output pointer for
  XED_ERROR_NO_AGEN_CALL_BACK_REGISTERED, ///< One or both of the callbacks for
  XED_ERROR_BAD_MEMOP_INDEX, ///< Memop indices must be 0 or 1.
  XED_ERROR_CALLBACK_PROBLEM, ///< The register or segment callback for
  XED_ERROR_LAST
} xed_error_enum_t;

XED_DLL_EXPORT xed_error_enum_t
str2xed_error_enum_t(const char* s);
XED_DLL_EXPORT const char*
xed_error_enum_t2str(const xed_error_enum_t p);

#endif
