# -----------------------------------------------------------------------------
# Mint tests written in Mint.
# -----------------------------------------------------------------------------

do assertEq(path.top_level_source_dir(), path.current_source_dir())
do assertEq(path.top_level_build_dir(), path.current_build_dir())
