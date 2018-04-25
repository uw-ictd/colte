/*
 * Copyright (c) 2015, EURECOM (www.eurecom.fr)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the FreeBSD Project.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define G_LOG_DOMAIN ("PARSER")

#include "pointer_type.h"
#include "ui_interface.h"

int
pointer_dissect_from_buffer (
  types_t * type,
  ui_set_signal_text_cb_t ui_set_signal_text_cb,
  gpointer user_data,
  buffer_t * buffer,
  uint32_t offset,
  uint32_t parent_offset,
  int indent,
  gboolean new_line)
{
  int                                     length = 0,
    i;
  char                                    cbuf[200];

  DISPLAY_PARSE_INFO ("pointer", type->name, offset, parent_offset);
  memset (cbuf, 0, 200);
  uint8_t                                 value[type->size / 8];

  buffer_fetch_nbytes (buffer, parent_offset + offset, type->size / 8, value);
  indent = new_line ? indent : 0;

  if (indent > 0) {
    DISPLAY_TYPE ("Ptr");
  }

  if (type->child && type->child->name) {
    /*
     * INDENTED(stdout, indent, fprintf(stdout, "<%s>0x%08x</%s>\n",
     * type->child->name, value, type->child->name));
     */
    INDENTED_STRING (cbuf, indent, length = sprintf (cbuf, "(%s *) 0x", type->child->name));
  } else {
    /*
     * INDENTED(stdout, indent, fprintf(stdout, "<Pointer>0x%08x</Pointer>\n",
     * value));
     */
    INDENTED_STRING (cbuf, indent, length = sprintf (cbuf, "(void *) 0x"));
  }

  /*
   * Append the value
   */
  for (i = type->size / 8 - 1; i >= 0; i--) {
    length += sprintf (&cbuf[length], "%02x", value[i]);
  }

  length += sprintf (&cbuf[length], ";\n");
  ui_set_signal_text_cb (user_data, cbuf, length);
  return 0;
}

int
pointer_type_file_print (
  types_t * type,
  int indent,
  FILE * file)
{
  if (type == NULL)
    return -1;

  INDENTED (file, indent, fprintf (file, "<Pointer>\n"));
  INDENTED (file, indent + 4, fprintf (file, "Id .........: %d\n", type->id));
  INDENTED (file, indent + 4, fprintf (file, "Type .......: %d\n", type->type_xml));
  INDENTED (file, indent + 4, fprintf (file, "Size .......: %d\n", type->size));
  INDENTED (file, indent + 4, fprintf (file, "Align ......: %d\n", type->align));

  if (type->child != NULL) {
    if (type->child->type == TYPE_FUNCTION) {
      INDENTED (file, indent + 4, fprintf (file, "<Function>\n"));
      INDENTED (file, indent + 8, fprintf (file, "<Args>To be done</Args>\n"));
      INDENTED (file, indent + 4, fprintf (file, "</Function>\n"));
    } else {
      type->child->type_file_print (type->child, indent + 4, file);
    }
  }

  if (type->file_ref != NULL)
    type->file_ref->type_file_print (type->file_ref, indent + 4, file);

  INDENTED (file, indent, fprintf (file, "</Pointer>\n"));
  return 0;
}

int
pointer_type_hr_display (
  types_t * type,
  int indent)
{
  return pointer_type_file_print (type, indent, stdout);
}
