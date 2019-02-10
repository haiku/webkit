list(APPEND WebKit_SOURCES
    Platform/IPC/unix/AttachmentUnix.cpp
    Platform/IPC/unix/ConnectionUnix.cpp

    Platform/haiku/LoggingHaiku.cpp
    Platform/haiku/ModuleHaiku.cpp

    Platform/unix/SharedMemoryUnix.cpp

    PluginProcess/unix/PluginControllerProxyUnix.cpp
    PluginProcess/unix/PluginProcessMainUnix.cpp
    PluginProcess/unix/PluginProcessUnix.cpp

    Shared/WebCoreArgumentCoders.cpp

    Shared/haiku/ShareableBitmapHaiku.cpp

    Shared/haiku/ProcessExecutablePathHaiku.cpp

    Shared/haiku/WebCoreArgumentCodersHaiku.cpp
    Shared/haiku/WebMemorySamplerHaiku.cpp

    Shared/unix/ChildProcessMain.cpp

    UIProcess/DefaultUndoController.cpp

    UIProcess/API/C/haiku/WKView.cpp

    UIProcess/API/haiku/WebView.cpp

    UIProcess/Launcher/haiku/ProcessLauncherHaiku.cpp

    UIProcess/Plugins/unix/PluginInfoStoreUnix.cpp
    UIProcess/Plugins/unix/PluginProcessProxyUnix.cpp

    UIProcess/haiku/BackingStoreHaiku.cpp
    UIProcess/haiku/TextCheckerHaiku.cpp
    UIProcess/haiku/WebContextHaiku.cpp
    UIProcess/haiku/WebInspectorProxyHaiku.cpp
    UIProcess/haiku/WebPageProxyHaiku.cpp
    UIProcess/haiku/WebPreferencesHaiku.cpp
    UIProcess/haiku/WebProcessProxyHaiku.cpp
    UIProcess/haiku/WebView.cpp

    WebProcess/Cookies/haiku/WebCookieManagerHaiku.cpp

    WebProcess/InjectedBundle/haiku/InjectedBundleHaiku.cpp

    WebProcess/Plugins/Netscape/unix/PluginProxyUnix.cpp

    WebProcess/WebCoreSupport/haiku/WebContextMenuClientHaiku.cpp
    WebProcess/WebCoreSupport/haiku/WebErrorsHaiku.cpp
    WebProcess/WebCoreSupport/haiku/WebFrameNetworkingContext.cpp
    WebProcess/WebCoreSupport/haiku/WebPopupMenuHaiku.cpp

    WebProcess/WebPage/DrawingAreaImpl.cpp

    WebProcess/WebPage/CoordinatedGraphics/CoordinatedLayerTreeHost.cpp

    WebProcess/WebPage/haiku/WebInspectorHaiku.cpp
    WebProcess/WebPage/haiku/WebPageHaiku.cpp

    WebProcess/haiku/WebProcessHaiku.cpp
    WebProcess/haiku/WebProcessMainHaiku.cpp
)

list(APPEND WebKit_INCLUDE_DIRECTORIES
    "${WEBCORE_DIR}/platform/graphics/opentype"
    "${WEBCORE_DIR}/platform/network/haiku"
    "${WEBKIT_DIR}/NetworkProcess/unix"
    "${WEBKIT_DIR}/Platform"
    "${WEBKIT_DIR}/Shared/API/c/haiku"
    "${WEBKIT_DIR}/Shared/CoordinatedGraphics"
    "${WEBKIT_DIR}/Shared/unix"
    "${WEBKIT_DIR}/UIProcess/API/C/CoordinatedGraphics"
    "${WEBKIT_DIR}/UIProcess/API/C/haiku"
    "${WEBKIT_DIR}/UIProcess/API/haiku"
    "${WEBKIT_DIR}/UIProcess/haiku"
    "${WEBKIT_DIR}/UIProcess/CoordinatedGraphics"
    "${WEBKIT_DIR}/WebProcess/unix"
    "${WEBKIT_DIR}/WebProcess/WebCoreSupport/haiku"
    "${WEBKIT_DIR}/WebProcess/WebPage/CoordinatedGraphics"
    ${LIBXML2_INCLUDE_DIR}
    ${LIBXSLT_INCLUDE_DIRS}
    ${SQLITE_INCLUDE_DIRS}
    ${WTF_DIR}
    "${FORWARDING_HEADERS_DIR}/WebCore"
    "${DERIVED_SOURCES_WEBCORE_DIR}"
)

list(APPEND WebKit_LIBRARIES
    ${CMAKE_DL_LIBS}
    ${FONTCONFIG_LIBRARIES}
    ${FREETYPE2_LIBRARIES}
    ${JPEG_LIBRARIES}
    ${LIBXML2_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${PNG_LIBRARIES}
    ${SQLITE_LIBRARIES}
)

list(APPEND WebProcess_SOURCES
    WebProcess/EntryPoint/unix/WebProcessMain.cpp
)

list(APPEND NetworkProcess_SOURCES
    NetworkProcess/EntryPoint/unix/NetworkProcessMain.cpp
)

list(APPEND WebProcess_LIBRARIES
    ${LIBXML2_LIBRARIES}
    ${LIBXSLT_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${SQLITE_LIBRARIES}
)

add_custom_target(forwarding-headerHaiku
    COMMAND ${PERL_EXECUTABLE} ${WEBKIT_DIR}/Scripts/generate-forwarding-headers.pl ${WEBKIT_DIR} ${DERIVED_SOURCES_WEBKIT_DIR}/include haiku
    COMMAND ${PERL_EXECUTABLE} ${WEBKIT_DIR}/Scripts/generate-forwarding-headers.pl ${WEBKIT_DIR} ${DERIVED_SOURCES_WEBKIT_DIR}/include CoordinatedGraphics
)

set(WEBKIT_EXTRA_DEPENDENCIES
    forwarding-headerHaiku
)

add_definitions(
    -DLIBEXECDIR=\"${EXEC_INSTALL_DIR}\"
    -DWEBPROCESSNAME=\"WebProcess\"
    -DPLUGINPROCESSNAME=\"PluginProcess\"
    -DNETWORKPROCESSNAME=\"NetworkProcess\"
)

set(WebKit_FORWARDING_HEADERS_DIRECTORIES
   Shared/API/c
   Shared/API/c/haiku
)

WEBKIT_CREATE_FORWARDING_HEADERS(WebKit FILES ${WebKit_FORWARDING_HEADERS_FILES} DIRECTORIES 
${WebKit_FORWARDING_HEADERS_DIRECTORIES})
