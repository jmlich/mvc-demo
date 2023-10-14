cmake_minimum_required(VERSION 3.14)

project(mvc-demo LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Core Quick Xml Widgets Charts LinguistTools REQUIRED)

file(GLOB TS_FILES *.ts)

add_executable(mvc-demo
    main.cpp
    filereader.cpp
    qml.qrc
    ${TS_FILES}
    )

QT6_CREATE_TRANSLATION(QT_FILES "${CMAKE_SOURCE_DIR}"
    OPTIONS "-no-obsolete")

QT6_ADD_TRANSLATION(QM_FILES ${TS_FILES})
add_custom_target(translations DEPENDS ${QM_FILES})
add_dependencies(mvc-demo translations)


target_compile_definitions(mvc-demo
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(mvc-demo
  PRIVATE Qt6::Core Qt6::Gui Qt6::Quick Qt6::Widgets Qt6::Charts Qt6::Xml)

qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

#install(TARGETS mvc-demo RUNTIME
#        DESTINATION ${CMAKE_INSTALL_BINDIR})

#install(FILES ${QM_FILES}
#        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/mvc-demo/translations)
