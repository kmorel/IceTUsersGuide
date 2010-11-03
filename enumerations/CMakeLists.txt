# The IceT API contains several "enumerations."  That is, constant
# identifier groups that are used to select behavior.  These are often
# documented in multiple places such as on a manpage and in the main
# document where the functionality is described.  Rather than enter them
# twice and risk them getting out of sync, list them once in a file here
# and included them elsewhere.

SET(enumerations
  image_color
  image_depth
  single_image_strategy
  strategy
  )

FOREACH(enum ${enumerations})
  CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/${enum}.tex
    ${CMAKE_CURRENT_BINARY_DIR}/${enum}.tex
    COPYONLY
    )
ENDFOREACH(enum)
