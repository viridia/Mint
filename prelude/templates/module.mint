# -----------------------------------------------------------------------------
# Template-generating prototypes
# -----------------------------------------------------------------------------

c_header_template = object {
  param source : string = undefined
  param output : string = undefined
  export lazy param actions : list[any] = [
    console.info("Writing file ${output}."),
    #fundamentals.copy_file(source, output, self)
  ]
}
