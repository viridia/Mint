# -----------------------------------------------------------------------------
# Configuration tests.
# -----------------------------------------------------------------------------

from platform import platform
from compilers.compiler import compiler

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

  cached var value : bool => do [
      require(message), require(program),
      console.status(message),
      let result = shell(program, args ++ ["2> /dev/null 1> /dev/null"], input).status == 0 : [
        console.status(result and "YES\n" or "NO\n"),
        result
      ]
  ]
}

# -----------------------------------------------------------------------------
# Exit status test based on the result of compilation.
# -----------------------------------------------------------------------------

compilation_test = object {
  # Message to print
  param message : string = undefined

  # Compiler to use
  param comp : compiler = platform.c_compiler_default

  # Standard input to the program
  param input : string = undefined

  # Source language
  param source_language : string = undefined

  # Whether or not to preprocess
  param preprocess_only : bool = false

  # Where to put the output
  param outputs : list[string] = [ '/dev/null' ]

  cached var value : bool => do [
    require(message), require(comp),
    console.status(message),
    let result = comp.compose(self).check_compile(input).status == 0 : [
      console.status(result and "YES\n" or "NO\n"),
      result
    ]
  ]
}

# -----------------------------------------------------------------------------
# Check that C source code compiles without error.
# -----------------------------------------------------------------------------

check_c_source_compiles = compilation_test {
  source_language = 'c'
}

# -----------------------------------------------------------------------------
# Check that C source code runs through the preprocessor without error.
# -----------------------------------------------------------------------------

check_c_source_preprocesses = compilation_test {
  source_language = 'c'
  preprocess_only = true
}

# -----------------------------------------------------------------------------
# Check that C++ source code compiles without error.
# -----------------------------------------------------------------------------

check_cplus_source_compiles = compilation_test {
  source_language = 'c++'
}

# -----------------------------------------------------------------------------
# Check that C++ source code runs through the preprocessor without error.
# -----------------------------------------------------------------------------

check_cplus_source_preprocesses = compilation_test {
  source_language = 'c++'
  preprocess_only = true
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
  input   => <{char {% function %}();
               int main(int argc, char *argv[]) {
                 (void)argc;
                 (void)argv;
                  {% function %}();
                 return 0;
               }
               }>
}

# -----------------------------------------------------------------------------
# Check that the specified type is defined.
# -----------------------------------------------------------------------------

check_type_exists = check_c_source_compiles {
  param typename : string = undefined  # The name of the type
  param headers : list[string] = undefined  # Header file that the type is defined in
  message => "Checking if type ${typename} exists..."
  input   => <{
             {% for header in headers %}
               #include <{% header %}>
             {% endfor %}
             int main(int argc, char *argv[]) {
                 (void)argc;
                 {% typename %} t;
                 (void)t;
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
  param headers : list[string] = undefined  # Header file that the structure is defined in
  message => "Checking for struct ${struct} member ${member}..."
  input   => <{
             {% for header in headers %}
               #include <{% header %}>
             {% endfor %}
             int main(int argc, char *argv[]) {
               (void)argc;
               void * p = (void *)&((struct {% struct %}*)argv)->{% member %};
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

# -----------------------------------------------------------------------------
# Base prototype for a test that involves compiling and running a program, and
# returning the result.
# -----------------------------------------------------------------------------

compile_and_run_test = object {
  # Message to print
  param message : string = undefined

  # Compiler to use
  param comp : compiler = platform.c_compiler_default

  # Standard input to the program
  param input : string = undefined

  # Source language
  param source_language : string = undefined

  # Whether or not to preprocess
  param preprocess_only : bool = false

  # Where to put the output
  #param outputs : list[string] => [ path.add_ext(path.tempname(), platform.executable_ext) ]

  cached var value : int => do [
    require(message), require(comp),
    console.status(message),
    let out = path.add_ext(path.tempname(), platform.executable_ext),
      cstatus = comp.compose(self, { 'outputs' = [ out ] }).check_compile(input).status == 0 : [
      if (cstatus) do [
        let result = shell(out, [], '').status : [
          console.status("${result}\n"),
          shell("rm", ['-f', out], ''),
          result
        ]
      ] else do [
        console.status("ERROR status ${cstatus}\n"),
        undefined
      ]
    ]
  ]
  
  def toString() -> string : "${value}"
}

# -----------------------------------------------------------------------------
# Check the size of a type.
# -----------------------------------------------------------------------------

check_sizeof_type = compile_and_run_test {
  param typename : string = undefined  # The bane if the type
  param headers : list[string] = []  # Header files that the structure is defined in
  source_language = 'c'
  message => "Checking size of type ${typename}..."
  input   => <{
             {% for header in headers %}
               #include <{% header %}>
             {% endfor %}
             int main(int argc, char *argv[]) {
               (void)argc;
               return (int)sizeof({% typename %});
             }
             }>
}

check_sizeof_cplus_type = compile_and_run_test {
  param typename : string = undefined  # The bane if the type
  param headers : list[string] = []  # Header files that the structure is defined in
  source_language = 'c++'
  message => "Checking size of type ${typename}..."
  input   => <{
             {% for header in headers %}
               #include <{% header %}>
             {% endfor %}
             int main(int argc, char *argv[]) {
                (void)argc;
                return int(sizeof({% typename %}));
             }
             }>
}
