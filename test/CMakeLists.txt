# GTest Setup ######
message(STATUS "[SuMa] Fetching GTest.")

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        e2239ee6043f73722e7aa812a459f54a28552929 # release-1.11.0
)

FetchContent_MakeAvailable(googletest)

set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(BUILD_GTEST ON CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(gtest_build_tests OFF CACHE BOOL "" FORCE)

# build the test cases
include_directories(${GTEST_INCLUDE_DIRS} ${robovision_INCLUDE_DIRS} ${BOOST_INCLUDES})

file(GLOB SHADER_FILES "../src/shader/*")

set(TEST_SHADER_SRC ${CMAKE_BINARY_DIR}/test_shaders.cpp)

COMPILE_SHADERS(
  ${TEST_SHADER_SRC} 
  ${SHADER_FILES} 
  opengl/pix2ndc.frag
  opengl/pix2ndc.vert
  opengl/rounding.vert
  opengl/rounding.frag
  opengl/rounding_access.frag
  opengl/rounding_access_rect.frag
  opengl/test.frag
  opengl/ndc.frag
  opengl/ndc.vert)
  
add_executable(test_core
  ../src/util/kitti_utils.cpp
  ../src/core/ImagePyramidGenerator.cpp
  ../src/core/lie_algebra.cpp
  
  core/PyramidTest.cpp

  core/CalibTest.cpp
  
  core/EvalTest.cpp
  core/matrix.cpp
  core/lie_test.cpp
)

add_executable(test_posegraph
  ../src/core/Posegraph.cpp
  core/PosegraphTest.cpp
)


add_executable(test_suma_opengl
  ../src/io/KITTIReader.cpp
  
  ../src/core/Preprocessing.cpp
  ../src/core/Frame2Model.cpp
  
  ../src/core/ImagePyramidGenerator.cpp

  ../src/core/lie_algebra.cpp
  ../src/core/LieGaussNewton.cpp
  
  ../src/util/kitti_utils.cpp
  
  ${TEST_SHADER_SRC}

  opengl/icp-test.cpp
  opengl/depthimg-test.cpp
  opengl/main-opengl.cpp
  opengl/testNDC.cpp  
  opengl/jacobian-test.cpp
)

configure_file(scan0.bin scan0.bin COPYONLY)
configure_file(scan1.bin scan1.bin COPYONLY)
configure_file(calib.txt calib.txt COPYONLY)
    
target_link_libraries(test_core PRIVATE gtest_main robovision glow glow_util)
target_link_libraries(test_suma_opengl PRIVATE gtest_main robovision glow glow_util)
target_link_libraries(test_posegraph PRIVATE gtest_main robovision glow glow_util gtsam)

  
# This runs the test cases always...
# add_custom_target(run_tests_core ALL COMMAND core_tests DEPENDS core_tests)
add_test(test_core test_core)
add_test(test_suma_opengl test_suma_opengl)
