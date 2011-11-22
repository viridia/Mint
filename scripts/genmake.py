#!/usr/bin/python

import os
import sys

headers = []
for dirpath, dirnames, filenames in os.walk("include"):
  for file in filenames:
    headers.append(os.path.join(dirpath, file))

sources = []
objects = []
for dirpath, dirnames, filenames in os.walk("lib"):
  for file in filenames:
    sources.append(os.path.join(dirpath, file))
    if file.endswith(".cpp"):
      objects.append(file[:-4] + ".o")

unittest_sources = []
unittest_objects = []
for dirpath, dirnames, filenames in os.walk("test/unit"):
  for file in filenames:
    unittest_sources.append(os.path.join(dirpath, file))
    if file.endswith(".cpp"):
      unittest_objects.append(file[:-4] + ".o")

if not headers:
  print "No header files found!"
  sys.exit(-1)

#if not sources:
#  print "No source files found!"
#  sys.exit(-1)

if not unittest_sources:
  print "No unit test source files found!"
  sys.exit(-1)

fh = open("Makefile.common", "w")

print >> fh, "# Common makefile for Mint.\n"
print >> fh, "MINT_HEADERS =\\\n  " + "\\\n  ".join(headers)
print >> fh
print >> fh, "MINT_SOURCES =\\\n  " + "\\\n  ".join(sources)
print >> fh
print >> fh, "MINT_OBJECTS =\\\n  " + "\\\n  ".join(objects)
print >> fh
print >> fh, "MINT_UNITTEST_SOURCES =\\\n  " + "\\\n  ".join(unittest_sources)
print >> fh
print >> fh, "MINT_UNITTEST_OBJECTS =\\\n  " + "\\\n  ".join(unittest_objects)

fh.close()
