# -----------------------------------------------------------------------------
# Template-generating prototypes
# -----------------------------------------------------------------------------

c_header_template = object {
  param source : string = undefined
  param output : string = undefined
  param re_defineflag = fundamentals.re.compile("#defineflag\\s+(\\w+)\\s+(\\w+)")
  param re_varref = fundamentals.re.compile("\\$\\{(\\w+)\\}")
  param env : any => self.module
  cached param actions : list[any] => [
    console.status("Generating file ${output} from ${source}...\n"),
    let src_abs = path.join(self.module.source_dir, source),
        out_abs = path.join(self.module.output_dir, output),
        src_text = fundamentals.file.read(src_abs) : [
      file.write(out_abs, re_defineflag.subst_all(
          re_varref.subst_all(src_text,
              match => if (env[match.group[1]]) "${env[match.group[1]]}" else "UNDEFINED"),
          match => if (env[match.group[1]])
                     "#define ${match.group[1]} ${match.group[2]}"
                   else
                     "/* #undef ${match.group[1]} */")),
    ]
  ]
}
