# -----------------------------------------------------------------------------
# Configuration tests.
# -----------------------------------------------------------------------------

from platform import platform

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
# Base prototype for a test that involves running a program and getting the
# exit result code as an integer.
# -----------------------------------------------------------------------------

int_exit_status_test = object {
  # Message to print
  param message : string = undefined

  # Name of the program to run
  param program : string = undefined

  # Command-line arguments
  param args : list[string] = []

  # Standard input to the program
  param input : string = undefined

  cached var value : int => do [
      require(message), require(program),
      console.status(message),
      let result = shell(program, args ++ ["2> /dev/null 1> /dev/null"], input).status : [
        console.status(result ++ "\n"),
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
  param header : string = undefined  # Header file that the type is defined in
  message => "Checking if type ${typename} exists..."
  input   => <{#include <{% header %}>
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
  param header : string = undefined  # Header file that the structure is defined in
  message => "Checking for struct ${struct} member ${member}..."
  input   => <{#include <{% header %}>
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
# Check the size of a type.
# -----------------------------------------------------------------------------

check_sizeof_type = int_exit_status_test {
  param typename : string = undefined  # The bane if the type
  param headers : list[string] = []  # Header files that the structure is defined in
  program = platform.c_compiler_default.program
  message => "Checking size of type ${typename}..."
  input   => <{
             {% for header in headers %}
               #include <{% header %}>
             {% endfor %}
             int main(int argc, char *argv[]) {
               (void)argc;
               return int(sizeof(${typename});
             }
             }>
}

check_sizeof_cplus_type = int_exit_status_test {
  param typename : string = undefined  # The bane if the type
  param headers : list[string] = []  # Header files that the structure is defined in
  program = platform.cplus_compiler_default.program
  message => "Checking size of type ${typename}..."
  input   => <{
             {% for header in headers %}
               #include <{% header %}>
             {% endfor %}
             int main(int argc, char *argv[]) {
                (void)argc;
                return int(sizeof({% typename %});
             }
             }>
}
