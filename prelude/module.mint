# Standard Mint prelude

file_target = target {
}

executable = file_target {
}

library = file_target {
}

# TODO - these should be explicitly included, and there should be a means
# to create your own tests.

check_include_file = object {
  param header : string = undefined
  param paths : list[string] = [] # Make this the standard paths
  param message : string = "Checking for ${0}..."
#  lazy param test : bool = [ any(file_exists(path, header) for path in paths) ]
#  lazy param test : bool = [ any(path => path.is_file(path.join(path, header)), paths) ]
# lazy param source_file = tempfile { extension = "c", content = "#include \"${header}\" "}
# lazy param test = cc { sources = [ source_file.path ] }.test
}

check_include_file_cpp = object {
  param header : string = undefined
  param paths : list[string] = []
  param message : string = "Checking for ${0}..."
#  lazy param test = [ any(file_exists(path, header) for path in paths) ]
}

find_library = object {
  param library : string = undefined
  param paths : list[string] = []
  param message : string = "Checking for ${0}..."
#  lazy param test = [ any(file_exists(path, header) for path in paths) ]
}

cpp = tool {
  param sources : list[string] = []
  param include_dirs : list[string] = []
  param library_dirs : list[string] = []
  param flags : list[string] = []
#  lazy param include_flags : list[string] = map(include_dirs, dir => ["-I", dir])
#  lazy param compile : list[function] = [
#    shell("g++", flags ++ include_flags)
#  ]
#  lazy param gendeps : list[function] = [
#    shell("g++", flags ++ include_flags)
#  ]
#  lazy param test : list[function] = [
#    shell("g++", flags ++ include_flags)
#  ]
}
