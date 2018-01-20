#include "proto.h"

int main (int argc, char ** argv) {
  exe_name = *argv;
  int mode = parse_options(argv + 1, argc - 1);
  if (!repository_path) error_exit(1, "repository path not given");
  if (!command_line_filename_count) error_exit(1, "no input files");
  switch (mode) {
    case MODE_INTERACTIVE:
      interactive_mode();
      return 0;
    case MODE_AUTO_DATA_8:
    case MODE_AUTO_DATA_16:
    case MODE_AUTO_DATA_32:
      auto_data_dump_mode(mode); // mode constants are chosen to work this way
      return 0;
    case MODE_AUTO_DATA_PTR:
      auto_data_dump_mode(0);
      return 0;
    case MODE_AUTO_SCRIPT:
      // ...
    default:
      return 1;
  }
}

void error_exit (int status, const char * fmt, ...) {
  va_list ap;
  fputs("error: ", stderr);
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  putc('\n', stderr);
  va_end(ap);
  exit(status);
}

void interactive_mode (void) {
  FILE * in;
  FILE * out;
  if (command_line_filename_count > 2) error_exit(1, "only a single input and output file can be given in interactive mode");
  in = fopen(*command_line_filenames, "r");
  if (!in) error_exit(2, "could not open file %s for reading", *command_line_filenames);
  if (command_line_filename_count == 2) {
    out = fopen(command_line_filenames[1], "w");
    if (!out) error_exit(2, "could not open file %s for writing", command_line_filenames[1]);
    dump_incbins(in, out);
    fclose(in);
    fclose(out);
  } else {
    global_temporary_file = tmpfile();
    if (!global_temporary_file) error_exit(2, "could not create temporary file");
    dump_incbins(in, global_temporary_file);
    fclose(in);
    in = fopen(*command_line_filenames, "w");
    if (!in) {
      fclose(global_temporary_file);
      error_exit(2, "could not open file %s for writing", *command_line_filenames);
    }
    transfer_temporary_to_file(in);
    fclose(in);
  }
}

void auto_data_dump_mode (unsigned char width) {
  // width = 0 means pointers
  unsigned file_number;
  FILE * file;
  for (file_number = 0; file_number < command_line_filename_count; file_number ++) {
    file = fopen(command_line_filenames[file_number], "r");
    if (!file) {
      printf("err: skipping file %s (could not open)\n", command_line_filenames[file_number]);
      continue;
    }
    global_temporary_file = tmpfile();
    if (!global_temporary_file) error_exit(2, "could not create temporary file");
    printf("file %s\n", command_line_filenames[file_number]);
    dump_incbins_to_data(file, width);
    fclose(file);
    file = fopen(command_line_filenames[file_number], "w");
    if (!file) error_exit(2, "could not open file %s for writing", command_line_filenames[file_number]);
    transfer_temporary_to_file(file);
    fclose(file);
  }
}
