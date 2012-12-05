
SET(SRC_FILES
    UEventsManager.cpp
    UEventsHandler.cpp
    UFile.cpp
    UDirectory.cpp
    UConversion.cpp
    ULogger.cpp
    UThreadNode.cpp
    UTimer.cpp
    UProcessInfo.cpp
)

SET(INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/include
    ${PTHREADS_INCLUDE_DIR}
)

# Make sure the compiler can find include files from our UtiLite library.
INCLUDE_DIRECTORIES(${INCLUDE_DIRS})

# Adds a library called UtiLite (libutilite.so under Linux) from the source files
ADD_LIBRARY( utilite ${SRC_FILES} )
IF(WIN32)
    TARGET_LINK_LIBRARIES(utilite ${PTHREADS_LIBRARY} "-lpsapi")
ELSE(WIN32)
    TARGET_LINK_LIBRARIES(utilite ${PTHREADS_LIBRARY})
ENDIF(WIN32)

SET_TARGET_PROPERTIES(
utilite
PROPERTIES
 VERSION ${UTILITE_VERSION} 
 SOVERSION ${UTILITE_VERSION}
 INSTALL_NAME_DIR ${CMAKE_INSTALL_PREFIX}/lib
)

INSTALL(TARGETS utilite
        RUNTIME DESTINATION bin COMPONENT runtime
        LIBRARY DESTINATION lib COMPONENT devel
        ARCHIVE DESTINATION lib COMPONENT devel)

INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME} 
        DESTINATION include 
		COMPONENT devel
        FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)