# Removes trailing whitespace from a text file. The text file is specified with
# the variable FILENAME. You run this with a command like
#
# cmake -D FILENAME=<filename> -P RemoveTrailingWhitespace.cmake
#
# The edit is done in place, so be wary. Don't run this on any file you are
# not prepared to mangle.

if(NOT FILENAME)
  message(SEND_ERROR "Must set FILENAME variable.")
endif()

if(NOT EXISTS ${FILENAME})
  message(SEND_ERROR "File '${FILENAME}' does not exist.")
endif()

file(READ ${FILENAME} file_contents)

string(REGEX REPLACE "[ \t]+\n" "\n" file_contents "${file_contents}")

file(WRITE ${FILENAME} "${file_contents}")
