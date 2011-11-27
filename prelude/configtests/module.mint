# -----------------------------------------------------------------------------
# Configuration tests.
# -----------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# Base prototype for test that involve running a program and acquiring the
# exit result code.
# -----------------------------------------------------------------------------

exit_status_test = object {
  # Message to print
  lazy param message : string = ""

  # Name of the program to run
  param program : string = undefined

  # Command-line arguments
  param args : list[string] = []

  # Standard input to the program
  lazy param input : string = undefined

  export lazy param value : bool =
      console.status(message)
      and shell(program, args, input).status
}

# -----------------------------------------------------------------------------
# Check for the presence of a C include file.
# -----------------------------------------------------------------------------

check_include_file = exit_status_test {
  param header : string = undefined
  param paths : list[string] = [] # Make this the standard paths
  message = "Checking for C header file ${header}..."
  program = "cpp"
  args    = ["-xc"]
  input   = "#include <${header}>\n"
}

# -----------------------------------------------------------------------------
# Check for the presence of a C++ include file.
# -----------------------------------------------------------------------------

check_include_file_cpp = exit_status_test {
  param header : string = undefined
  param paths : list[string] = []
  message = "Checking for C++ header file ${header}..."
  program = "cpp"
  args    = ["-xc++"]
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
