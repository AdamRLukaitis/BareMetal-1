INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

SET(BAREMETAL_TSL2563_SOURCES
    tsl2563.c
)

ADD_LIBRARY(bm_tsl2563 ${BAREMETAL_TSL2563_SOURCES})

INSTALL(TARGETS bm_tsl2563 RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/include/bm/tsl2563.h DESTINATION include/bm/)