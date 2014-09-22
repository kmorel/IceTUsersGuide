# This directory contains manpage documentation.  The manpages are written
# in LaTeX and included in the main document.  A latex2man perl script is
# used to convert the LaTeX documents into man pages.  There are many
# restrictions placed on the format of the LaTeX document to ensure that
# this conversion happens correctly.

SET(man3pages
  icetAddTile
  icetBoundingBox
  icetBoundingVertices
  icetCompositeImage
  icetCompositeMode
  icetCompositeOrder
  icetCopyState
  icetCreateContext
  icetCreateMPICommunicator
  icetDataReplicationGroup
  icetDataReplicationGroupColor
  icetDestroyContext
  icetDestroyMPICommunicator
  icetDiagnostics
  icetDrawCallback
  icetDrawFrame
  icetEnable
  icetGet
  icetGetContext
  icetGetError
  icetGetSingleImageStrategyName
  icetGetStrategyName
  icetGLDrawCallback
  icetGLDrawFrame
  icetGLInitialize
  icetGLIsInitialized
  icetGLSetReadBuffer
  icetImageCopyColor
  icetImageGetColor
  icetImageGetColorFormat
  icetImageGetNumPixels
  icetImageIsNull
  icetImageNull
  icetIsEnabled
  icetPhysicalRenderSize
  icetResetTiles
  icetSetContext
  icetSetColorFormat
  icetSingleImageStrategy
  icetStrategy
  icetWallTime
  )

# Location of latex2man perl script.
SET(LATEX2MAN ${CMAKE_SOURCE_DIR}/latex2man/latex2man)
SET(LATEX2MAN_ARGS -t${CMAKE_CURRENT_SOURCE_DIR}/icet.trans)

FIND_PACKAGE(Perl REQUIRED)

SET(MAN3DIR ${CMAKE_BINARY_DIR}/man/man3)
MAKE_DIRECTORY(${MAN3DIR})

# The latex2man script supports neither \input nor \include commands
# (grumble).  To get around that, use CMake configuration to insert blocks
# of LaTeX into the man pages that are defined in the enumeration
# directory.
FILE(GLOB enumeration_files ${CMAKE_SOURCE_DIR}/enumerations/*.tex)
FOREACH (enum_file ${enumeration_files})
  GET_FILENAME_COMPONENT(enum_name ${enum_file} NAME_WE)
  FILE(READ ${enum_file} enum_${enum_name})
ENDFOREACH(enum_file)

# Also include a copyright statement
FILE(READ ${CMAKE_CURRENT_SOURCE_DIR}/copyright.tex copyright)

#Create a tex file that includes all of the tex manpages.
SET(MANPAGES_TEX ${CMAKE_CURRENT_BINARY_DIR}/manpages.tex)
FILE(WRITE ${MANPAGES_TEX} "%Include all of the tex manpages.\n\n")

SET(man3files "")
FOREACH(page ${man3pages})
  CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/${page}.tex
    ${CMAKE_CURRENT_BINARY_DIR}/${page}.tex
    @ONLY)
  SET(man3page ${MAN3DIR}/${page}.3)
  ADD_CUSTOM_COMMAND(OUTPUT ${man3page}
    COMMAND ${PERL_EXECUTABLE}
    ARGS ${LATEX2MAN} ${LATEX2MAN_ARGS} ${page}.tex ${man3page}
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${page}.tex icet.trans
    )
  SET(man3files ${man3files} ${man3page})

  FILE(APPEND ${MANPAGES_TEX} "\\input{manpages/${page}.tex}\n")
ENDFOREACH(page)

MACRO(ICET_ALIAS_MAN_PAGE original alias)
  ADD_CUSTOM_COMMAND(OUTPUT ${MAN3DIR}/${alias}.3
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy ${MAN3DIR}/${original}.3 ${MAN3DIR}/${alias}.3
    DEPENDS ${MAN3DIR}/${original}.3
    )
  SET(man3files ${man3files} ${MAN3DIR}/${alias}.3)
ENDMACRO(ICET_ALIAS_MAN_PAGE)

ICET_ALIAS_MAN_PAGE(icetEnable icetDisable)

ICET_ALIAS_MAN_PAGE(icetSetColorFormat icetSetDepthFormat)

ICET_ALIAS_MAN_PAGE(icetImageGetColorFormat icetImageGetDepthFormat)

ICET_ALIAS_MAN_PAGE(icetImageGetNumPixels icetImageGetWidth)
ICET_ALIAS_MAN_PAGE(icetImageGetNumPixels icetImageGetHeight)

ICET_ALIAS_MAN_PAGE(icetImageGetColor icetImageGetColorub)
ICET_ALIAS_MAN_PAGE(icetImageGetColor icetImageGetColorcub)
ICET_ALIAS_MAN_PAGE(icetImageGetColor icetImageGetColorui)
ICET_ALIAS_MAN_PAGE(icetImageGetColor icetImageGetColorcui)
ICET_ALIAS_MAN_PAGE(icetImageGetColor icetImageGetColorf)
ICET_ALIAS_MAN_PAGE(icetImageGetColor icetImageGetColorcf)
ICET_ALIAS_MAN_PAGE(icetImageGetColor icetImageGetDepth)
ICET_ALIAS_MAN_PAGE(icetImageGetColor icetImageGetDepthf)
ICET_ALIAS_MAN_PAGE(icetImageGetColor icetImageGetDepthcf)

ICET_ALIAS_MAN_PAGE(icetImageCopyColor icetImageCopyColorub)
ICET_ALIAS_MAN_PAGE(icetImageCopyColor icetImageCopyColorf)
ICET_ALIAS_MAN_PAGE(icetImageCopyColor icetImageCopyDepth)
ICET_ALIAS_MAN_PAGE(icetImageCopyColor icetImageCopyDepthf)

ICET_ALIAS_MAN_PAGE(icetBoundingBox icetBoundingBoxd)
ICET_ALIAS_MAN_PAGE(icetBoundingBox icetBoundingBoxf)

ICET_ALIAS_MAN_PAGE(icetGet icetGetDoublev)
ICET_ALIAS_MAN_PAGE(icetGet icetGetFloatv)
ICET_ALIAS_MAN_PAGE(icetGet icetGetIntegerv)
ICET_ALIAS_MAN_PAGE(icetGet icetGetBooleanv)
ICET_ALIAS_MAN_PAGE(icetGet icetGetPointerv)

ADD_CUSTOM_TARGET(IceTManPages ALL
  DEPENDS ${man3files}
  )
