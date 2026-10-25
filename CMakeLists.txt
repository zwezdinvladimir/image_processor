cmake_minimum_required(VERSION 3.20)

project(image_processor LANGUAGES CXX)

add_library(image_processor_lib
    color.cpp
    image.cpp
    filters/crop.cpp
    filters/grayscale.cpp
    filters/negative.cpp
    filters/convolution.cpp
    filters/threshold.cpp
    filters/gaussian_blur.cpp
    filters/filter_sequence.cpp
    filters/edge_detection.cpp
    filters/sharpening.cpp
    filters/quadtree_mosaic.cpp
)
target_compile_features(image_processor_lib PUBLIC cxx_std_23)
target_include_directories(image_processor_lib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

add_executable(image_processor main.cpp)
target_link_libraries(image_processor PRIVATE image_processor_lib)

include(CTest)
if(BUILD_TESTING)
    add_executable(image_processor_tests tests/test.cpp)
    target_link_libraries(image_processor_tests PRIVATE image_processor_lib)
    add_test(NAME filters COMMAND image_processor_tests filters)
    add_test(NAME effects COMMAND image_processor_tests effects)
    add_test(NAME bmp_padding COMMAND image_processor_tests bmp_padding)
    add_test(NAME bmp_layout COMMAND image_processor_tests bmp_layout)
    add_test(NAME bmp_round_trip COMMAND image_processor_tests bmp_round_trip)
endif()