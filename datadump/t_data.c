#include "proto.h"

char * script_transform_skip (struct script_value value, int parameter, struct script_value * result) {
  result -> type = value.type;
  result -> data = NULL;
  result -> value = value.value - parameter;
  if (parameter < 0)
    return duplicate_string("negative skip count");
  else if (value.type && (value.type < 4))
    return duplicate_string("type mismatch");
  else if (parameter > value.value)
    return duplicate_string("skip count is greater than length");
  unsigned length_factor;
  switch (value.type) {
    case 0: case 4: case 5:
      length_factor = 1;
      break;
    case 6:
      length_factor = sizeof(short);
      break;
    case 7:
      length_factor = sizeof(int);
      break;
    default:
      return duplicate_string("internal error");
  }
  result -> data = malloc(length_factor * result -> value);
  memcpy(result -> data, ((char *) (value.data)) + length_factor * parameter, length_factor * result -> value);
  return NULL;
}

char * script_transform_item (struct script_value value, int parameter, struct script_value * result) {
  result -> type = value.type - 4;
  result -> data = NULL;
  if (parameter < 0)
    return duplicate_string("negative index");
  else if (value.type < 5)
    return duplicate_string("type mismatch");
  else if (parameter >= value.value)
    return duplicate_string("index out of bounds");
  switch (value.type) {
    case 5:
      result -> value = parameter[(signed char *) (value.data)];
      break;
    case 6:
      result -> value = parameter[(short *) (value.data)];
      break;
    case 7:
      result -> value = parameter[(int *) (value.data)];
      break;
    default:
      return duplicate_string("internal error");
  }
  return NULL;
}

char * script_transform_copy (struct script_value value, int parameter, struct script_value * result) {
  result -> type = value.type;
  result -> data = NULL;
  result -> value = parameter;
  if (parameter < 0)
    return duplicate_string("negative length");
  else if (value.type && (value.type < 4))
    return duplicate_string("type mismatch");
  else if (parameter > value.value)
    return duplicate_string("length is greater than data size");
  unsigned length_factor;
  switch (value.type) {
    case 0: case 4: case 5:
      length_factor = 1;
      break;
    case 6:
      length_factor = sizeof(short);
      break;
    case 7:
      length_factor = sizeof(int);
      break;
    default:
      return duplicate_string("internal error");
  }
  result -> data = malloc(length_factor * parameter);
  memcpy(result -> data, value.data, length_factor * parameter);
  return NULL;
}

char * script_transform_count_values (struct script_value value, int parameter, struct script_value * result, int mode) {
  result -> type = 3;
  result -> data = NULL;
  if (value.type && (value.type < 4)) return duplicate_string("type mismatch");
  int * data = malloc(sizeof(int) * value.value);
  unsigned p, count;
  switch (value.type) {
    case 0: case 4: case 5: {
      signed char * data8 = value.data;
      for (p = 0; p < value.value; p ++) data[p] = data8[p];
      if ((parameter & ~127) == 128) parameter -= 256;
    } break;
    case 6: {
      short * data16 = value.data;
      for (p = 0; p < value.value; p ++) data[p] = data16[p];
      if ((parameter & ~32767) == 32768) parameter -= 65536;
    } break;
    case 7:
      memcpy(data, value.data, sizeof(int) * value.value);
  }
  for (count = p = 0; p < value.value; p ++) if (data[p] == parameter) count ++;
  free(data);
  switch (mode) {
    case 0: result -> value = count; break;
    case 1: result -> value = count > 0; break;
    case 2: result -> value = count == value.value; break;
  }
  return NULL;
}

char * script_transform_count (struct script_value value, int parameter, struct script_value * result) {
  return script_transform_count_values(value, parameter, result, 0);
}

char * script_transform_any (struct script_value value, int parameter, struct script_value * result) {
  return script_transform_count_values(value, parameter, result, 1);
}

char * script_transform_all (struct script_value value, int parameter, struct script_value * result) {
  return script_transform_count_values(value, parameter, result, 2);
}
