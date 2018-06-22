# build the test cases
include_directories(${catkin_INLCUDE_DIRS} ${GTEST_INCLUDE_DIRS} src/src/include/ ${robovision_INCLUDE_DIRS} ${BOOST_INCLUDES})

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


add_executable(test_opengl
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

add_executable(test_rv
  ../src/rv/CompositeParameter.cpp
  ../src/rv/Parameter.cpp
  ../src/rv/ParameterList.cpp
  ../src/rv/ParameterListIterator.cpp
  ../src/rv/PrimitiveParameters.cpp
  ../src/rv/RangeParameter.cpp
  ../src/rv/XmlDocument.cpp
  ../src/rv/yxml.c
  rv/fileutil-test.cpp
  rv/geometry-test.cpp
  rv/math-test.cpp
  rv/parameterlistiterator-test.cpp
  rv/parameterlist-test.cpp
  rv/parameter-test.cpp
  rv/math-test.cpp
  rv/transform-test.cpp
  rv/xml-test.cpp
)




configure_file(scan0.bin scan0.bin COPYONLY)
configure_file(scan1.bin scan1.bin COPYONLY)
configure_file(calib.txt calib.txt COPYONLY)
    
target_link_libraries(test_core PRIVATE ${GTEST_MAIN_LIBRARIES} robovision ${BOOST_LIBRARIES})
target_link_libraries(test_opengl PRIVATE ${GTEST_LIBRARIES} robovision ${BOOST_LIBRARIES} ${glow_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})
target_link_libraries(test_posegraph PRIVATE ${GTEST_MAIN_LIBRARIES} robovision ${BOOST_LIBRARIES} ${glow_LIBRARIES} ${OPENGL_LIBRARIES}  ${GLEW_LIBRARIES} gtsam)
target_link_libraries(test_rv PRIVATE ${GTEST_MAIN_LIBRARIES} robovision)
  
# This runs the test cases always...
# add_custom_target(run_tests_core ALL COMMAND core_tests DEPENDS core_tests)
add_test(test_core test_core)
add_test(test_opengl test_opengl)
add_test(test_rv test_rv)

