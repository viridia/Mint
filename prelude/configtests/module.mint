# -----------------------------------------------------------------------------
# Configuration tests.
# -----------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# Base prototype for test that involve running a program and acquiring the
# exit result code.
# -----------------------------------------------------------------------------

exit_status_test = object {
  # Message to print
  param message : string = undefined

  # Name of the program to run
  param program : string = undefined

  # Command-line arguments
  param args : list[string] = []

  # Standard input to the program
  param input : string = undefined

  # TODO: Make this work on windows?
  # TODO: Show result of test on the console?
  cached param value : bool => do [
      require(message), require(program),
      console.status(message),
      let result = shell(program, args ++ ["2> /dev/null 1> /dev/null"], input).status == 0 : [
        console.status(result and "YES\n" or "NO\n"),
        result
      ]
  ]
}

# -----------------------------------------------------------------------------
# Check that C source code compiles without error.
# -----------------------------------------------------------------------------

check_c_source_compiles = exit_status_test {
#  def env : object = self.module
#  def includes : list[string] = env['includes'].map(inc => ["-I", inc])
  program = "gcc"
  args    = ["-xc", "-o", "/dev/null", "-"]
}

# -----------------------------------------------------------------------------
# Check that C source code runs through the preprocessor without error.
# -----------------------------------------------------------------------------

check_c_source_preprocesses = exit_status_test {
  program = "gcc"
  args    = ["-xc", "-E", "-"]
}

# -----------------------------------------------------------------------------
# Check that C++ source code compiles without error.
# -----------------------------------------------------------------------------

check_cplus_source_compiles = exit_status_test {
  program = "gcc"
  args    = ["-xc++", "-"]
}

# -----------------------------------------------------------------------------
# Check that C++ source code runs through the preprocessor without error.
# -----------------------------------------------------------------------------

check_cplus_source_preprocesses = exit_status_test {
  program = "gcc"
  args    = ["-xc++", "-E", "-"]
}

# -----------------------------------------------------------------------------
# Check for the presence of a C include file.
# -----------------------------------------------------------------------------

check_include_file = check_c_source_preprocesses {
  param header : string = undefined
  message => "Checking for C header file ${header}..."
  input   => "#include <${header}>\n"
}

# -----------------------------------------------------------------------------
# Check for the presence of a C++ include file.
# -----------------------------------------------------------------------------

check_include_file_cplus = check_cplus_source_preprocesses {
  param header : string = undefined
  message => "Checking for C++ header file ${header}..."
  input   => "#include <${header}>\n"
}

# -----------------------------------------------------------------------------
# Check for the presence of a function in one of the system libraries.
# -----------------------------------------------------------------------------

check_function_exists = check_c_source_compiles {
  param function : string = undefined
  message => "Checking for function ${function}..."
  input   => <{char ${function}();
               int main(int argc, char *argv[]) {
                 (void)argc;
                 (void)argv;
                  ${function}();
                 return 0;
               }
               }>
}

# -----------------------------------------------------------------------------
# Check that the specified structure has a given member.
# -----------------------------------------------------------------------------

check_struct_has_member = check_c_source_compiles {
  param struct : string = undefined  # The structure
  param member : string = undefined  # The member to test
  param header : string = undefined  # Header file that the structure is defined in
  message => "Checking for struct ${struct} member ${member}..."
  input   => <{#include <${header}>
               int main(int argc, char *argv[]) {
                 (void)argc;
                 void * p = (void *)&((struct ${struct}*)argv)->${member};
                 return 0;
               }
               }>
}

# -----------------------------------------------------------------------------
# Check for the presence of a static library.
# -----------------------------------------------------------------------------

find_library = object {
  param library : string = undefined
  param paths : list[string] = []
  param message : string = "Checking for ${library}..."
#  param test => [ any(file_exists(path, header) for path in paths) ]
}
