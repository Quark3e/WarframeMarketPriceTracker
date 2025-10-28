cmake_minimum_required(VERSION 3.19)

project(WarframeMarketPriceTracker VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 17)


set(DIR_INCLUDE_LIB
    "${CMAKE_SOURCE_DIR}/lib/Pos2d/include/"
)
set(FILES_SOURCES
    "${CMAKE_SOURCE_DIR}/src/wmpt_functions.cpp"
    "${CMAKE_SOURCE_DIR}/src/wmpt_main.cpp"
    "${CMAKE_SOURCE_DIR}/src/wmpt_types.cpp"
    "${CMAKE_SOURCE_DIR}/src/wmpt_variables.cpp"
    "${CMAKE_SOURCE_DIR}/src/wmpt_tui.cpp"
)

add_executable(${PROJECT_NAME}
    ${FILES_SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/include/"
    "${DIR_INCLUDE_LIB}"
)

# include(FetchContent)
# FetchContent_Declare(ftxui
#   GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
#   GIT_TAG v6.1.9
# )
# FetchContent_MakeAvailable(ftxui)

# target_link_libraries(${PROJECT_NAME} PRIVATE
#     # Chose a submodule
#     ftxui::component
#     ftxui::dom
#     ftxui::screen
# )

