#include "config_file.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char* trim_space(char* str)
{
  char* end = str + strlen(str);

  char* first = str;
  while (first != end && isspace(*first))
    first++;

  if (first == end)
    return first;

  char* last = end - 1;
  while (last != first && isspace(*last))
    *last-- = '\0';

  return first;
}

static bool is_commented_line(const char* line)
{
  return line[0] == ';' || line[0] == '#' || line[0] == '\0';
}

static bool is_section_line(const char* line)
{
  const char* end = line + strlen(line);
  return *line == '[' && *(end - 1) == ']';
}

static char* get_section_name(char* str)
{
  str += 1;
  str[strlen(str) - 1] = '\0';
  return trim_space(str);
}

static bool split_key_value(char* str, const char** key, const char** value)
{
  char* pch = strchr(str, '=');
  if (!pch)
    return false;

  *pch = '\0';
  pch++;

  *key = trim_space(str);
  *value = trim_space(pch);

  return true;
}

void config_read_file(const char* filename, struct config_event_handlers* handlers, void* user_data)
{
  FILE* f = fopen(filename, "r");
  if (!f)
    return;

  char* line = NULL;
  size_t len = 0;
  ssize_t nread;

  bool section_found = false;
  void* section_data = NULL;

  while ((nread = getline(&line, &len, f)) != -1) {
    char* trimmed_line = trim_space(line);

    if (!is_commented_line(trimmed_line)) {
      if (is_section_line(trimmed_line)) {
        if (section_found)
          handlers->section_end(section_data, user_data);

        const char* section = get_section_name(trimmed_line);
        section_data = handlers->section_start(section, user_data);
        section_found = true;
      } else {
        const char* key;
        const char* value;
        if (split_key_value(trimmed_line, &key, &value))
          handlers->option_found(section_data, key, value, user_data);
      }
    }
  }

  if (section_found)
    handlers->section_end(section_data, user_data);

  free(line);

  fclose(f);
}
