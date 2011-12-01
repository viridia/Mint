# -----------------------------------------------------------------------------
# Configuration tests.
# -----------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# Base prototype for test that involve running a program and acquiring the
# exit result code.
# -----------------------------------------------------------------------------

exit_status_test = object {
  # Message to print
  def message : string = ""

  # Name of the program to run
  param program : string = undefined

  # Command-line arguments
  param args : list[string] = []

  # Standard input to the program
  def input : string = undefined

  # TODO: Make this work on windows?
  # TODO: Show result of test on the console?
  export lazy param value : bool = do [
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

check_include_file_cpp = exit_status_test {
  param header : string = undefined
  param paths : list[string] = []
  message = "Checking for C++ header file ${header}..."
  program = "gcc"
  args    = ["-xc++", "-E", "-"]
  input   = "#include <${header}>\n"
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
