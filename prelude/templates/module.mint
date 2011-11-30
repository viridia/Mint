# -----------------------------------------------------------------------------
# Template-generating prototypes
# -----------------------------------------------------------------------------

c_header_template = object {
  param source : string = undefined
  param output : string = undefined
  export lazy param actions : list[any] = [
    let src_abs = path.join(path.current_source_dir(), source),
        out_abs = path.join(path.current_build_dir(), output) : [
      console.status("Generating file ${output} from ${source}...\n"),
      console.warn("Source file is ${src_abs}"),
      console.warn("Output file is ${out_abs}"),
      console.debug(fundamentals.file.read(src_abs)),
      #fundamentals.copy_file(source, output, self)
    ]
  ]
}
