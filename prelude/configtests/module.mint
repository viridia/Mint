# -----------------------------------------------------------------------------
# Configuration tests.
# -----------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# Check for the presence of a C include file.
# -----------------------------------------------------------------------------

check_include_file = object {
  param header : string = undefined
  param paths : list[string] = [] # Make this the standard paths
  param message : string = "Checking for ${0}..."
#  lazy param test : bool = [ any(file_exists(path, header) for path in paths) ]
#  lazy param test : bool = [ any(path => path.is_file(path.join(path, header)), paths) ]
# lazy param source_file = tempfile { extension = "c", content = "#include \"${header}\" "}
# lazy param test = cc { sources = [ source_file.path ] }.test
}

# -----------------------------------------------------------------------------
# Check for the presence of a C++ include file.
# -----------------------------------------------------------------------------

check_include_file_cpp = object {
  param header : string = undefined
  param paths : list[string] = []
  param message : string = "Checking for ${0}..."
#  lazy param test = [ any(file_exists(path, header) for path in paths) ]
}

# -----------------------------------------------------------------------------
# Check for the presence of a static library.
# -----------------------------------------------------------------------------

find_library = object {
  param library : string = undefined
  param paths : list[string] = []
  param message : string = "Checking for ${0}..."
#  lazy param test = [ any(file_exists(path, header) for path in paths) ]
}
