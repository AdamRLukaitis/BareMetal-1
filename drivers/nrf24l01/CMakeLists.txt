INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

SET(BAREMETAL_NRF24L01_SOURCES
    nrf24l01.c
)

ADD_LIBRARY(bm_nrf24l01 ${BAREMETAL_NRF24L01_SOURCES})

INSTALL(TARGETS bm_nrf24l01 RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/include/bm/nrf24l01.h DESTINATION include/bm/)