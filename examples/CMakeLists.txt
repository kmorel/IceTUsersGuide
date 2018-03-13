project(IceTExamples C)

set(Examples
  Tutorial
  )

find_package(IceT REQUIRED)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(MPI REQUIRED)

foreach (example ${Examples})
  add_executable(${example} ${example}.c)
  target_compile_options(${example} PRIVATE ${MPI_C_COMPILE_FLAGS})
  target_include_directories(${example} PRIVATE
    ${OPENGL_INCLUDE_DIR}
    ${MPI_C_INCLUDE_PATH}
    ${GLUT_INCLUDE_DIR}
    ${ICET_INCLUDE_DIRS}
    )
  target_link_libraries(${example} PRIVATE
    ${OPENGL_LIBRARIES}
    ${GLUT_LIBRARIES}
    ${MPI_C_LIBRARIES}
    ${ICET_CORE_LIBS}
    ${ICET_GL_LIBS}
    ${ICET_MPI_LIBS}
    )
endforeach (example)
