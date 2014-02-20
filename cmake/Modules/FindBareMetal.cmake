IF(NOT BareMetal_PLATFORM)
    MESSAGE(STATUS "No BareMetal_PLATFORM specified.")
ENDIF()

IF(BareMetal_PLATFORM STREQUAL "stm32spl")
    SET(BareMetal_PLATFORM_COMPONENTS spi i2c gpio delay)
    
    STRING(TOLOWER ${STM32_FAMILY} STM32_FAMILY_LOWER)
    IF(STM32_CHIP_TYPE)
        STRING(TOLOWER ${STM32_CHIP_TYPE} STM32_CHIP_TYPE_LOWER)
        SET(BareMetal_LIB_SUFFIXIES stm32spl_${STM32_FAMILY_LOWER}_${STM32_CHIP_TYPE_LOWER})
    ENDIF()
ENDIF()

IF(BareMetal_FIND_COMPONENTS) 
    FOREACH(comp ${BareMetal_FIND_COMPONENTS}) 
        SET(BareMetal_${comp}_FIND_LIBS bm_${comp})

        LIST(FIND BareMetal_PLATFORM_COMPONENTS ${comp} BareMetal_HAS_PLATFORM)
 
        IF(NOT BareMetal_HAS_PLATFORM EQUAL -1)
            FOREACH(BareMetal_LIB_SUFFIX ${BareMetal_LIB_SUFFIXIES})
                LIST(APPEND BareMetal_${comp}_FIND_LIBS bm_${comp}_${BareMetal_LIB_SUFFIX})
            ENDFOREACH() 
        ENDIF()
        
        FOREACH(BareMetal_LIB_NAME ${BareMetal_${comp}_FIND_LIBS})
            FIND_LIBRARY(BareMetal_${BareMetal_LIB_NAME}_LIBRARY
                NAMES ${BareMetal_LIB_NAME}
                PATH_SUFFIXES lib
            )
            SET(BareMetal_LIBRARIES ${BareMetal_LIBRARIES} ${BareMetal_${BareMetal_LIB_NAME}_LIBRARY})
        ENDFOREACH() 
        LIST(APPEND BareMetal_FIND_INCLUDES bm/${comp}.h) 
    ENDFOREACH() 
ENDIF()

FIND_PATH(BareMetal_INCLUDE_DIRS 
    ${BareMetal_FIND_INCLUDES}
    PATH_SUFFIXES include
) 

FIND_PACKAGE_HANDLE_STANDARD_ARGS(BareMetal DEFAULT_MSG BareMetal_LIBRARIES BareMetal_INCLUDE_DIRS) 
