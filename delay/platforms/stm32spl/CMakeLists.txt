FIND_PACKAGE(CMSIS REQUIRED)

INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/include
    ${CMSIS_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

SET(BAREMETAL_DELAY_SOURCES
    delay_stm32spl.c
)

STM32_GENERATE_LIBRARIES(bm_delay_stm32spl ${BAREMETAL_DELAY_SOURCES} BAREMETAL_DELAY_LIBRARIES)
INSTALL(TARGETS ${BAREMETAL_DELAY_LIBRARIES} RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)