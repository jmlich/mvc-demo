find_package(Qt6 COMPONENTS Core Test REQUIRED)

add_executable(tst_filereader
    tst_filereader.cpp
    ${CMAKE_SOURCE_DIR}/src/filereader.cpp
    ${CMAKE_SOURCE_DIR}/qml/qml.qrc
    )

target_include_directories(tst_filereader PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(tst_filereader PRIVATE Qt6::Core Qt6::Test)

add_test(NAME tst_filereader COMMAND tst_filereader)
