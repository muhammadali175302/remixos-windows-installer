/*************************************************
*      Perl-Compatible Regular Expressions       *
*************************************************/

/* PCRE is a library of functions to support regular expressions whose syntax
and semantics are as close as possible to those of the Perl 5 language.

                       Written by Philip Hazel
           Copyright (c) 1997-2005 University of Cambridge

-----------------------------------------------------------------------------
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    * Neither the name of the University of Cambridge nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
-----------------------------------------------------------------------------
*/


/* This module contains some convenience functions for extracting substrings
from the subject string after a regex match has succeeded. The original idea
for these functions came from Scott Wimer. */


#include "pcre_internal.h"
#include <wtf/UnusedParam.h>


/*************************************************
*           Find number for named string         *
*************************************************/

/* This function is used by the two extraction functions below, as well
as being generally available.

Arguments:
  code        the compiled regex
  stringname  the name whose number is required

Returns:      the number of the named parentheses, or a negative number
                (PCRE_ERROR_NOSUBSTRING) if not found
*/

int
pcre_get_stringnumber(const pcre *code, const pcre_char *stringname)
{
/* FIXME: This doesn't work for UTF-16 because the name table has 8-bit characters in it! */
#if !PCRE_UTF16
int rc;
int entrysize;
int top, bot;
uschar *nametable;

if ((rc = pcre_fullinfo(code, NULL, PCRE_INFO_NAMECOUNT, &top)) != 0)
  return rc;
if (top <= 0) return PCRE_ERROR_NOSUBSTRING;

if ((rc = pcre_fullinfo(code, NULL, PCRE_INFO_NAMEENTRYSIZE, &entrysize)) != 0)
  return rc;
if ((rc = pcre_fullinfo(code, NULL, PCRE_INFO_NAMETABLE, &nametable)) != 0)
  return rc;

bot = 0;
while (top > bot)
  {
  int mid = (top + bot) / 2;
  uschar *entry = nametable + entrysize*mid;
  int c = strcmp(stringname, (char *)(entry + 2));
  if (c == 0) return (entry[0] << 8) + entry[1];
  if (c > 0) bot = mid + 1; else top = mid;
  }
#else
 UNUSED_PARAM(code);
 UNUSED_PARAM(stringname);
#endif

return PCRE_ERROR_NOSUBSTRING;
}



/*************************************************
*      Copy captured string to given buffer      *
*************************************************/

/* This function copies a single captured substring into a given buffer.
Note that we use memcpy() rather than strncpy() in case there are binary zeros
in the string.

Arguments:
  subject        the subject string that was matched
  ovector        pointer to the offsets table
  stringcount    the number of substrings that were captured
                   (i.e. the yield of the pcre_exec call, unless
                   that was zero, in which case it should be 1/3
                   of the offset table size)
  stringnumber   the number of the required substring
  buffer         where to put the substring
  size           the size of the buffer

Returns:         if successful:
                   the length of the copied string, not including the zero
                   that is put on the end; can be zero
                 if not successful:
                   PCRE_ERROR_NOMEMORY (-6) buffer too small
                   PCRE_ERROR_NOSUBSTRING (-7) no such captured substring
*/

int
pcre_copy_substring(const pcre_char *subject, int *ovector, int stringcount,
  int stringnumber, pcre_char *buffer, int size)
{
int yield;
if (stringnumber < 0 || stringnumber >= stringcount)
  return PCRE_ERROR_NOSUBSTRING;
stringnumber *= 2;
yield = ovector[stringnumber+1] - ovector[stringnumber];
if (size < yield + 1) return PCRE_ERROR_NOMEMORY;
memcpy(buffer, subject + ovector[stringnumber], yield);
buffer[yield] = 0;
return yield;
}



/*************************************************
*   Copy named captured string to given buffer   *
*************************************************/

/* This function copies a single captured substring into a given buffer,
identifying it by name.

Arguments:
  code           the compiled regex
  subject        the subject string that was matched
  ovector        pointer to the offsets table
  stringcount    the number of substrings that were captured
                   (i.e. the yield of the pcre_exec call, unless
                   that was zero, in which case it should be 1/3
                   of the offset table size)
  stringname     the name of the required substring
  buffer         where to put the substring
  size           the size of the buffer

Returns:         if successful:
                   the length of the copied string, not including the zero
                   that is put on the end; can be zero
                 if not successful:
                   PCRE_ERROR_NOMEMORY (-6) buffer too small
                   PCRE_ERROR_NOSUBSTRING (-7) no such captured substring
*/

int
pcre_copy_named_substring(const pcre *code, const pcre_char *subject, int *ovector,
  int stringcount, const pcre_char *stringname, pcre_char *buffer, int size)
{
int n = pcre_get_stringnumber(code, stringname);
if (n <= 0) return n;
return pcre_copy_substring(subject, ovector, stringcount, n, buffer, size);
}



/*************************************************
*      Copy all captured strings to new store    *
*************************************************/

/* This function gets one chunk of store and builds a list of pointers and all
of the captured substrings in it. A NULL pointer is put on the end of the list.

Arguments:
  subject        the subject string that was matched
  ovector        pointer to the offsets table
  stringcount    the number of substrings that were captured
                   (i.e. the yield of the pcre_exec call, unless
                   that was zero, in which case it should be 1/3
                   of the offset table size)
  listptr        set to point to the list of pointers

Returns:         if successful: 0
                 if not successful:
                   PCRE_ERROR_NOMEMORY (-6) failed to get store
*/

int
pcre_get_substring_list(const pcre_char *subject, int *ovector, int stringcount,
  const pcre_char ***listptr)
{
int i;
int size = (int)sizeof(char *);
int double_count = stringcount * 2;
pcre_char **stringlist;
pcre_char *p;

for (i = 0; i < double_count; i += 2) {
  size += INT_CAST(sizeof(pcre_char *) + (ovector[i+1] - ovector[i] + 1) * sizeof(pcre_char));
}

stringlist = (pcre_char **)(pcre_malloc)(size);
if (stringlist == NULL) return PCRE_ERROR_NOMEMORY;

*listptr = (const pcre_char **)stringlist;
p = (pcre_char *)(stringlist + stringcount + 1);

for (i = 0; i < double_count; i += 2)
  {
  int len = ovector[i+1] - ovector[i];
  memcpy(p, subject + ovector[i], len * sizeof(pcre_char));
  *stringlist++ = p;
  p += len;
  *p++ = 0;
  }

*stringlist = NULL;
return 0;
}



/*************************************************
*   Free store obtained by get_substring_list    *
*************************************************/

/* This function exists for the benefit of people calling PCRE from non-C
programs that can call its functions, but not free() or (pcre_free)() directly.

Argument:   the result of a previous pcre_get_substring_list()
Returns:    nothing
*/

void
pcre_free_substring_list(const pcre_char **pointer)
{
(pcre_free)((void *)pointer);
}



/*************************************************
*      Copy captured string to new store         *
*************************************************/

/* This function copies a single captured substring into a piece of new
store

Arguments:
  subject        the subject string that was matched
  ovector        pointer to the offsets table
  stringcount    the number of substrings that were captured
                   (i.e. the yield of the pcre_exec call, unless
                   that was zero, in which case it should be 1/3
                   of the offset table size)
  stringnumber   the number of the required substring
  stringptr      where to put a pointer to the substring

Returns:         if successful:
                   the length of the string, not including the zero that
                   is put on the end; can be zero
                 if not successful:
                   PCRE_ERROR_NOMEMORY (-6) failed to get store
                   PCRE_ERROR_NOSUBSTRING (-7) substring not present
*/

int
pcre_get_substring(const pcre_char *subject, int *ovector, int stringcount,
  int stringnumber, const pcre_char **stringptr)
{
int yield;
pcre_char *substring;
if (stringnumber < 0 || stringnumber >= stringcount)
  return PCRE_ERROR_NOSUBSTRING;
stringnumber *= 2;
yield = ovector[stringnumber+1] - ovector[stringnumber];
substring = (pcre_char *)(pcre_malloc)((yield + 1) * sizeof(pcre_char));
if (substring == NULL) return PCRE_ERROR_NOMEMORY;
memcpy(substring, subject + ovector[stringnumber], yield * sizeof(pcre_char));
substring[yield] = 0;
*stringptr = substring;
return yield;
}



/*************************************************
*   Copy named captured string to new store      *
*************************************************/

/* This function copies a single captured substring, identified by name, into
new store.

Arguments:
  code           the compiled regex
  subject        the subject string that was matched
  ovector        pointer to the offsets table
  stringcount    the number of substrings that were captured
                   (i.e. the yield of the pcre_exec call, unless
                   that was zero, in which case it should be 1/3
                   of the offset table size)
  stringname     the name of the required substring
  stringptr      where to put the pointer

Returns:         if successful:
                   the length of the copied string, not including the zero
                   that is put on the end; can be zero
                 if not successful:
                   PCRE_ERROR_NOMEMORY (-6) couldn't get memory
                   PCRE_ERROR_NOSUBSTRING (-7) no such captured substring
*/

int
pcre_get_named_substring(const pcre *code, const pcre_char *subject, int *ovector,
  int stringcount, const pcre_char *stringname, const pcre_char **stringptr)
{
int n = pcre_get_stringnumber(code, stringname);
if (n <= 0) return n;
return pcre_get_substring(subject, ovector, stringcount, n, stringptr);
}




/*************************************************
*       Free store obtained by get_substring     *
*************************************************/

/* This function exists for the benefit of people calling PCRE from non-C
programs that can call its functions, but not free() or (pcre_free)() directly.

Argument:   the result of a previous pcre_get_substring()
Returns:    nothing
*/

void
pcre_free_substring(const pcre_char *pointer)
{
(pcre_free)((void *)pointer);
}

/* End of pcre_get.c */
