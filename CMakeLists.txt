cmake_minimum_required(VERSION 3.16...3.25)

project(surfel_mapping VERSION 0.1 DESCRIPTION "SuMa --Surfel-based Mapping using LiDAR scans" LANGUAGES C CXX)

set(OPENGL_VERSION "430" CACHE STRING "" FORCE)
set(ENABLE_NVIDIA_EXT On CACHE BOOL "" FORCE)

include(external/find_dependencies.cmake)

set(CMAKE_INCLUDE_CURRENT_DIR ON) # needs to be activated for qt generated files in build directory.
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -UDEBUG_GL -UDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-g0 -O3 -DNDEBUG")

find_package(OpenGL REQUIRED)
find_package(Qt5Gui REQUIRED) 
find_package(Qt5OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem system timer date_time)
find_package(Eigen3 REQUIRED)

# since now everything resides in "bin", we have to copy some stuff.
set(CMAKE_BUILD_TYPE Release)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

include_directories(src)

if(ENABLE_NVIDIA_EXT)
  message(STATUS "[SuMa] Enabling Nvidia OpenGL extensions.")
  add_definitions(-DQUERY_MEMORY_NV)
endif()

add_definitions(-D__GL_VERSION=${OPENGL_VERSION})
message(STATUS "[SuMa] Using OpenGL version ${OPENGL_VERSION}.")

set(COMP_SHADER_SRC ${CMAKE_BINARY_DIR}/computation_shaders.cpp)
set(VIZ_SHADER_SRC ${CMAKE_BINARY_DIR}/visualization_shaders.cpp)

COMPILE_SHADERS(${COMP_SHADER_SRC}
  src/shader/color.glsl
  
  src/shader/empty.vert
  src/shader/empty.frag
  src/shader/quad.geom
  src/shader/passthrough.frag
  
  src/shader/Frame2Model_jacobians.frag
  src/shader/Frame2Model_jacobians.geom
  src/shader/Frame2Model_jacobians.vert
  
  src/shader/render_surfels.frag
  src/shader/render_surfels.geom
  src/shader/render_surfels.vert
  src/shader/render_compose.frag
  src/shader/update_surfels.vert
  src/shader/update_surfels.geom
  src/shader/update_surfels.frag
  src/shader/extract_surfels.vert
  src/shader/init_radiusConf.vert
  src/shader/init_radiusConf.frag
  src/shader/copy_surfels.vert
  src/shader/copy_surfels.geom
  src/shader/gen_surfels.frag
  src/shader/gen_surfels.geom
  src/shader/gen_surfels.vert
  src/shader/gen_surfels330.geom
  src/shader/init_depthimg.frag
  src/shader/avg_vertexmap.frag
  src/shader/gen_depthimg.vert
  src/shader/gen_depthimg.frag
  src/shader/gen_normalmap.frag
  src/shader/gen_vertexmap.frag
  src/shader/gen_vertexmap.vert
  src/shader/gen_indexmap.vert
  src/shader/gen_indexmap.frag
  src/shader/bilateral_filter.frag
  
  # needed for surfelmap:
  src/shader/draw_surfels.frag
  src/shader/draw_surfels.geom
  src/shader/draw_surfels.vert
  src/shader/draw_surfelPoints.vert
  src/shader/draw_submaps.vert
  src/shader/draw_submaps.geom
  src/shader/coloredvertices.frag
  src/shader/coloredvertices.vert
)

COMPILE_SHADERS(${VIZ_SHADER_SRC} 
  src/shader/draw_normalmap3d.geom
  src/shader/draw_normalmap3d.vert
  src/shader/draw_vertexmap3d.vert
  src/shader/draw_depthimg.frag
  src/shader/draw_normalmap.frag

  src/shader/draw_residuals.frag
  src/shader/draw_mesh.vert
  src/shader/draw_mesh.frag
  
  src/shader/draw_posegraph_edge.geom

  src/shader/laserscan.vert
  src/shader/laserscan.frag)

QT5_WRAP_UI(UI_HDRS src/visualizer/visualizer.ui)

add_library(robovision
  src/rv/CompositeParameter.cpp
  src/rv/FileUtil.cpp
  src/rv/Laserscan.cpp
  src/rv/Math.cpp
  src/rv/Parameter.cpp
  src/rv/ParameterList.cpp
  src/rv/ParameterListIterator.cpp
  src/rv/PrimitiveParameters.cpp
  src/rv/Random.cpp
  src/rv/RangeParameter.cpp
  src/rv/string_utils.cpp
  src/rv/Stopwatch.cpp
  src/rv/transform.cpp
  src/rv/XmlDocument.cpp
  src/rv/XmlNode.cpp
  src/rv/yxml.c
)

add_library(suma
  src/core/SurfelMapping.cpp
  src/core/Preprocessing.cpp
  src/core/Frame2Model.cpp
  src/core/SurfelMap.cpp
  src/core/lie_algebra.cpp
  src/core/LieGaussNewton.cpp
  src/core/Posegraph.cpp
  src/core/ImagePyramidGenerator.cpp
  
   ${COMP_SHADER_SRC})
   
add_executable(visualizer
  src/io/KITTIReader.cpp
  src/io/SimulationReader.cpp
  src/io/RobocarReader.cpp
  
  src/opengl/Mesh.cpp
  src/opengl/Model.cpp
  src/opengl/ObjReader.cpp

  src/util/kitti_utils.cpp
  src/util/ScanAccumulator.cpp

  ${UI_HDRS}
  ${VIZ_SHADER_SRC}
   
  src/visualizer/GraphWidget.cpp
  src/visualizer/ViewportWidget.cpp
  src/visualizer/VisualizerWindow.cpp
  src/visualizer/visualizer.cpp)

    
add_executable(genvideo
  ${VIZ_SHADER_SRC}
  
  src/opengl/Mesh.cpp
  src/opengl/Model.cpp
  src/opengl/ObjReader.cpp
  src/util/ScanAccumulator.cpp
  src/visualizer/ViewportWidget.cpp
  src/io/KITTIReader.cpp
  src/util/kitti_utils.cpp
  
  src/genvideo.cpp)
  
enable_testing()
add_subdirectory(test)

target_link_libraries(robovision ${Boost_LIBRARIES})
target_link_libraries(suma robovision glow gtsam pthread)
target_link_libraries(visualizer suma glow_util Qt5::OpenGL Qt5::Widgets)
target_link_libraries(genvideo suma glow_util Qt5::OpenGL Qt5::Widgets)
