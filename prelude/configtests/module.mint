# -----------------------------------------------------------------------------
# Configuration tests.
# -----------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# Base prototype for test that involve running a program and acquiring the
# exit result code.
# -----------------------------------------------------------------------------

exit_status_test = object {
  # Message to print
  def message : string = undefined

  # Name of the program to run
  param program : string = undefined

  # Command-line arguments
  param args : list[string] = []

  # Standard input to the program
  def input : string = undefined

  # TODO: Make this work on windows?
  # TODO: Show result of test on the console?
  export lazy param value : bool = do [
      require(message), require(program),
      console.status(message),
      let result = shell(program, args ++ ["2>&1 > /dev/null"], input).status == 0 : [
        console.status(result and "YES\n" or "NO\n"),
        result
      ]
  ]
}

# -----------------------------------------------------------------------------
# Check for the presence of a C include file.
# -----------------------------------------------------------------------------

check_include_file = exit_status_test {
  param header : string = undefined
  param paths : list[string] = [] # Make this the standard paths
  message = "Checking for C header file ${header}..."
  program = "gcc"
  args    = ["-xc", "-E", "-"]
  input   = "#include <${header}>\n"
}

# -----------------------------------------------------------------------------
# Check for the presence of a C++ include file.
# -----------------------------------------------------------------------------

check_include_file_cplus = exit_status_test {
  param header : string = undefined
  param paths : list[string] = []
  message = "Checking for C++ header file ${header}..."
  program = "gcc"
  args    = ["-xc++", "-E", "-"]
  input   = "#include <${header}>\n"
}

# -----------------------------------------------------------------------------
# Check for the presence of a function in one of the system libraries.
# -----------------------------------------------------------------------------

check_function_exists = exit_status_test {
  param function : string = undefined
  message = "Checking for function ${function}..."
  program = "gcc"
  args    = ["-xc", "-"]
  input   = <{char ${function}();
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

check_struct_has_member = exit_status_test {
  # The structure
  param struct : string = undefined
  # The member to test
  param member : string = undefined
  # Header file that the structure is defined in
  param header : string = undefined
  message = "Checking for struct ${struct} member ${member}..."
  program = "gcc"
  args    = ["-xc", "-"]
  input   = <{#include <${header}>
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
#  lazy param test = [ any(file_exists(path, header) for path in paths) ]
}
