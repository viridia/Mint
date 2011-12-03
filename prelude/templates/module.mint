# -----------------------------------------------------------------------------
# Template-generating prototypes
# -----------------------------------------------------------------------------

c_header_template = object {
  param source : string = undefined
  param output : string = undefined
  param re_defineflag = fundamentals.re.compile("#defineflag\\s+(\\w+)\\s+(\\w+)")
  def env : dict[string, any] = {}
  export lazy param actions : list[any] = [
    console.status("Generating file ${output} from ${source}...\n"),
    let src_abs = path.join(self.module.source_dir(), source),
        out_abs = path.join(self.module.build_dir(), output) : [
      file.write(out_abs, re_defineflag.subst_all(
          fundamentals.file.read(src_abs),
          match => if (env[match.group[1]])
                     "#define ${match.group[1]} ${match.group[2]}"
                   else
                     "/* #undef ${match.group[1]} */")),
    ]
  ]
}
