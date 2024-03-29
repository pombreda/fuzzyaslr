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
/// @file xed-attribute-enum.h
/// @author Mark Charney <mark.charney@intel.com>

// This file was automatically generated.
// Do not edit this file.

#if !defined(_XED_ATTRIBUTE_ENUM_H_)
# define _XED_ATTRIBUTE_ENUM_H_
#include "xed-common-hdrs.h"
typedef enum {
  XED_ATTRIBUTE_INVALID=0, 
  XED_ATTRIBUTE_ANY_REP_ABLE=1, 
  XED_ATTRIBUTE_ATT_OPERAND_ORDER_EXCEPTION=2, 
  XED_ATTRIBUTE_FAR_XFER=4, 
  XED_ATTRIBUTE_FIXED_BASE0=8, 
  XED_ATTRIBUTE_FIXED_BASE1=16, 
  XED_ATTRIBUTE_REQUIRES_ALIGNMENT=32, 
  XED_ATTRIBUTE_SIMD_PACKED_ALIGNMENT=64, 
  XED_ATTRIBUTE_SIMD_SCALAR=128, 
  XED_ATTRIBUTE_SKIPLOW32=256, 
  XED_ATTRIBUTE_SKIPLOW64=512, 
  XED_ATTRIBUTE_UNALIGNED=1024, 
  XED_ATTRIBUTE_X87_MMX_STATE_CW=2048, 
  XED_ATTRIBUTE_X87_MMX_STATE_R=4096, 
  XED_ATTRIBUTE_X87_MMX_STATE_W=8192, 
  XED_ATTRIBUTE_XMM_STATE_CW=16384, 
  XED_ATTRIBUTE_XMM_STATE_R=32768, 
  XED_ATTRIBUTE_XMM_STATE_W=65536, 
  XED_ATTRIBUTE_LAST
} xed_attribute_enum_t;

XED_DLL_EXPORT xed_attribute_enum_t
str2xed_attribute_enum_t(const char* s);
XED_DLL_EXPORT const char*
xed_attribute_enum_t2str(const xed_attribute_enum_t p);

#endif
