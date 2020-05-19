#include "proto.h"

unsigned get_value_from_string (const char * string, unsigned length) {
  if (!(string && length && *string)) return -1;
  char * copy = malloc(length + 1);
  memcpy(copy, string, length);
  copy[length] = 0;
  char * errp;
  unsigned long long result = strtoull(copy, &errp, 0);
  char err = *errp;
  free(copy);
  if (err) return -1;
  if (result > 0xfffffffeULL) return -1;
  return result;
}

int convert_string_to_number (const char * string, int check_only) {
  if (!string) return 0;
  char * errp;
  long long value = strtoll(string, &errp, 0);
  if (*errp) return 0;
  if (value > 0xffffffffLL) return 0;
  if (value < ~0x7fffffffLL) return 0;
  return check_only ? 1 : value;
}

void concatenate (char ** string, unsigned * length, ...) {
  va_list ap;
  va_start(ap, length);
  const char * next;
  unsigned nextlen;
  while (next = va_arg(ap, const char *)) {
    nextlen = strlen(next);
    *string = realloc(*string, *length + nextlen + 1);
    memcpy(*string + *length, next, nextlen);
    *length += nextlen;
  }
  va_end(ap);
  (*string)[*length] = 0;
}

char * duplicate_string (const char * string) {
  char * result = malloc(strlen(string) + 1);
  strcpy(result, string);
  return result;
}

char ** split_by_spaces (const char * string) {
  return split_by(string, " \t");
}

char ** split_by (const char * string, const char * delimiters) {
  if (!string) return NULL;
  char ** result = NULL;
  unsigned length, count = 0;
  string += strspn(string, delimiters);
  while (*string) {
    length = strcspn(string, delimiters);
    result = realloc(result, (count + 1) * sizeof *result);
    result[count] = malloc(length + 1);
    memcpy(result[count], string, length);
    result[count][length] = 0;
    count ++;
    string += length;
    string += strspn(string, delimiters);
  }
  result = realloc(result, sizeof(char **) * (count + 1));
  result[count] = NULL;
  return result;
}

unsigned string_array_size (char ** array) {
  unsigned result;
  for (result = 0; *array; array ++, result ++);
  return result;
}

void destroy_string_array (char ** array) {
  if (!array) return;
  char ** current;
  for (current = array; *current; current ++) free(*current);
  free(array);
}

const char * find_first_non_space (const char * string) {
  return string + strspn(string, " \t");
}

void generate_initial_indented_line (char ** string, unsigned * length) {
  unsigned current_length;
  if (global_settings.indent_lines >= 10) {
    *string = malloc(2);
    **string = '\t';
    1[*string] = 0;
    if (length) *length = 1;
    return;
  }
  *string = malloc(global_settings.indent_lines + 1);
  for (current_length = 0; current_length < global_settings.indent_lines; current_length ++) current_length[*string] = ' ';
  current_length[*string] = 0;
  if (length) *length = current_length;
}

void generate_UTF8_character (char * string, unsigned codepoint) {
  *string = 0;
  if (!codepoint || (codepoint > 0x10ffff) || ((codepoint & -0x800u) == 0xd800)) return;
  unsigned char remaining;
  if (codepoint < 0x80)
    remaining = 0;
  else if (codepoint < 0x2000) {
    *string = 0xc0;
    remaining = 1;
  } else if (codepoint < 0x10000) {
    *string = 0xe0;
    remaining = 2;
  } else {
    *string = 0xf0;
    remaining = 3;
  }
  string[remaining + 1] = 0;
  while (remaining) {
    string[remaining] = 0x80 + (codepoint & 0x3f);
    remaining --;
    codepoint >>= 6;
  }
  *string += codepoint;
}
