/* ================================================================== *
 * Mint
 * ================================================================== */

#include "mint/eval/Evaluator.h"

#include "mint/intrinsic/Fundamentals.h"
#include "mint/intrinsic/StringRegistry.h"
#include "mint/intrinsic/TypeRegistry.h"

#include "mint/graph/Literal.h"
#include "mint/graph/Object.h"
#include "mint/graph/Oper.h"
#include "mint/graph/String.h"

#include "mint/support/Assert.h"
#include "mint/support/CommandLine.h"
#include "mint/support/Diagnostics.h"
#include "mint/support/OSError.h"
#include "mint/support/OStream.h"

#if HAVE_STDIO_H
#include <stdio.h>
#endif

#if HAVE_ERRNO_H
#include <errno.h>
#endif

#if HAVE_IO_H
#include <io.h>
#endif

namespace mint {

cl::Option<bool> optTraceCondig("trace-config", cl::Group("debug"),
    cl::Description("Print out all shell commands during configuration."));

Node * methodShell(Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 3);
  String * program = String::cast(args[0]);
  Oper * cmdArgs = static_cast<Oper *>(args[1]);
  String * input = String::cast(args[2]);
  SmallString<128> cmd;
  cmd.append(program->value());
  for (Oper::const_iterator it = cmdArgs->begin(), itEnd = cmdArgs->end(); it != itEnd; ++it) {
    cmd.push_back(' ');
    String * arg = String::cast(*it);
    // TODO: Quoting?
    cmd.append(arg->value());
  }
  if (optTraceCondig) {
    diag::debug() << cmd;
  }
  cmd.push_back('\0');
  #if defined(_WIN32)
    FILE * pipe = ::_popen(cmd.data(), "w");
  #else
    FILE * pipe = ::popen(cmd.data(), "w");
  #endif
  if (pipe == NULL) {
    ::perror("error");
    diag::error(loc) << "Command '" << cmd << "' failed to run with error code: " << errno;
    return &Node::UNDEFINED_NODE;
  } else {
    if (input->size() > 0) {
      ::fwrite(input->value().data(), 1, input->size(), pipe);
    }
    #if defined(_MSC_VER)
      int status = ::_pclose(pipe);
    #else
      int status = ::pclose(pipe);
    #endif
    Object * result = new Object(Node::NK_DICT, Location(), NULL);
    result->setType(TypeRegistry::genericDictType());
    if (WIFEXITED(status)) {
      result->attrs()[strings::str("status")] = Node::makeInt(WEXITSTATUS(status));
    }
    return result;
  }
}

void initSubprocessMethods(Fundamentals * fundamentals) {
  Type * typeStringList = TypeRegistry::get().getListType(TypeRegistry::stringType());
  Type * shellArgs[] = { TypeRegistry::stringType(), typeStringList, TypeRegistry::stringType() };
  fundamentals->defineMethod("shell", TypeRegistry::genericDictType(), shellArgs, methodShell);
}

}
