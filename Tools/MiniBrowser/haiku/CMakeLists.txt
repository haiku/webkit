SET(MiniBrowser_SOURCES
    ${TOOLS_DIR}/MiniBrowser/haiku/App.cpp
)

INCLUDE_DIRECTORIES(
    "${WEBKIT_DIR}/UIProcess/API/haiku"
)

SET(MiniBrowser_LIBRARIES
    WebKit
    WebCore
    JavaScriptCore
    ${LIBXML2_LIBRARIES}
    ${LIBXSLT_LIBRARIES}
    ${SQLITE_LIBRARIES}
    ${ICU_I18N_LIBRARIES}
    ${ICU_LIBRARIES}
    be bsd network stdc++ translation tracker
)

set(MiniBrowser_LOCAL_INCLUDE_DIRECTORIES
    "${WEBCORE_DIR}/css"
    "${WEBCORE_DIR}/platform/graphics"
    "${WEBCORE_DIR}/platform/graphics/transforms"
    "${WEBCORE_DIR}/rendering/shapes"
)

foreach(inc ${MiniBrowser_LOCAL_INCLUDE_DIRECTORIES})
    ADD_DEFINITIONS(-iquote ${inc})
endforeach(inc)

ADD_EXECUTABLE(MiniBrowser ${MiniBrowser_SOURCES})
TARGET_LINK_LIBRARIES(MiniBrowser ${MiniBrowser_LIBRARIES})
SET_TARGET_PROPERTIES(MiniBrowser PROPERTIES FOLDER "Tools")
SET_TARGET_PROPERTIES(MiniBrowser PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
