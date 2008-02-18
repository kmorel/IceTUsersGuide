
SET(man3pages
  icetAddTile
  icetBoundingBox
  icetBoundingVertices
  icetCompositeOrder
  icetCopyState
  icetCreateContext
  icetCreateMPICommunicator
  icetDataReplicationGroup
  icetDataReplicationGroupColor
  icetDestroyContext
  icetDestroyMPICommunicator
  icetDiagnostics
  icetDrawFrame
  icetDrawFunc
  icetEnable
  icetGet
  icetGetColorBuffer
  icetGetContext
  icetGetError
  icetGetStrategyName
  icetInputOutputBuffers
  icetIsEnabled
  icetResetTiles
  icetSetContext
  icetStrategy
  icetWallTime
  )

# Location of latex2man perl script.
SET(LATEX2MAN ${CMAKE_SOURCE_DIR}/latex2man/latex2man)
SET(LATEX2MAN_ARGS -t${CMAKE_CURRENT_SOURCE_DIR}/icet.trans)

FIND_PACKAGE(Perl REQUIRED)

SET(MAN3DIR ${CMAKE_BINARY_DIR}/man/man3)
MAKE_DIRECTORY(${MAN3DIR})

#Create a tex file that includes all of the tex manpages.
SET(MANPAGES_TEX ${CMAKE_CURRENT_BINARY_DIR}/manpages.tex)
FILE(WRITE ${MANPAGES_TEX} "%Include all of the tex manpages.\n\n")

SET(man3files "")
FOREACH(page ${man3pages})
  CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/${page}.tex
    ${CMAKE_CURRENT_BINARY_DIR}/${page}.tex
    COPYONLY)
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
ICET_ALIAS_MAN_PAGE(icetGetColorBuffer icetGetDepthBuffer)

ADD_CUSTOM_TARGET(IceTManPages ALL
  DEPENDS ${man3files}
  )
