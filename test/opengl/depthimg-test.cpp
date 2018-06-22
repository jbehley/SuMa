#include <cmath>

#include <glow/GlState.h>
#include <gtest/gtest.h>

#include <core/Preprocessing.h>
#include <glow/GlSampler.h>
#include <glow/glutil.h>
#include "io/KITTIReader.h"
#include <rv/ParameterList.h>
#include <rv/PrimitiveParameters.h>

#include <fstream>

using namespace rv;
using namespace glow;

const float pi = 3.14159265358979323846f;
const float inv_pi = 0.31830988618379067154f;
const float pi_2 = 1.57079632679;

float length(const vec3& vec) {
  return std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

// float degrees(float radians) {
//  return 180.0f * radians / pi;
//}

bool validVertex(const vec4& vertex) {
  return (vertex.w > 0.5f);
}

TEST(PreprocesingTest, generateImage) {
  ASSERT_NO_THROW(_CheckGlError(__FILE__, __LINE__));

  std::cerr << ">>> WARNING: NEED TO FIX THIS ISSUE. <<< " << std::endl;
  // FIXME: There seems to be some inconsistency in the computation of the pixel coordinates of a point.
  // Problem: How can we ensure that the points are in the correct pixel value!

  return;

  uint32_t width = 720;
  uint32_t height = 64;
  float fov_up = 2.5f;
  float fov_down = -24.8f;

  ParameterList params;
  params.insert(IntegerParameter("data_width", width));
  params.insert(IntegerParameter("data_height", height));
  params.insert(FloatParameter("data_fov_up", fov_up));
  params.insert(FloatParameter("data_fov_down", fov_down));
  params.insert(FloatParameter("max_depth", 100.0f));
  params.insert(FloatParameter("min_depth", 0.5f));

  KITTIReader reader("./scan0.bin");
  std::vector<Laserscan> scans;
  Laserscan scan;

  ASSERT_TRUE(reader.read(scan));
  GlBuffer<rv::Point3f> pts{BufferTarget::ARRAY_BUFFER, BufferUsage::DYNAMIC_READ};
  pts.assign(scan.points());

  Preprocessing preprocessor(params);
  //  Frame frame(width, height);
  std::shared_ptr<Frame> frame = std::make_shared<Frame>(width, height);

  glActiveTexture(GL_TEXTURE0);

  ASSERT_NO_THROW(preprocessor.process(pts, *frame));
  ASSERT_NO_THROW(CheckGlError());

  std::vector<vec4> vertex_data(width * height);
  frame->vertex_map.download(vertex_data);

  frame->vertex_map.save("vertexmap_test.ppm");
  frame->normal_map.save("normalmap_test.ppm");

  float fov = std::abs(fov_up) + std::abs(fov_down);

  // check if projected vertex is actually in the right pixel.
  uint32_t idx = 0;
  for (uint32_t y = 0; y < height; ++y) {
    for (uint32_t x = 0; x < width; ++x) {
      const vec3 position(vertex_data[idx].x, vertex_data[idx].y, vertex_data[idx].z);
      if (!validVertex(vertex_data[idx])) {
        idx += 1;
        continue;
      }
      std::cout << " (" << x << "," << y << ")" << std::flush;

      float depth = length(position);
      float yaw = std::atan2(position.y, position.x);
      float pitch = -std::asin(position.z / depth);  // angle = acos((0,0,1) * p/||p||) - pi/2 = pi/2 - asin(x) + pi/2

      float x_img = 0.5 * ((-yaw * inv_pi) + 1.0) * width;             // in [0, 1]
      float y_img = (1.0 - (degrees(pitch) + fov_up) / fov) * height;  // in [0, 1]

      //      if(x_img != img_coords[i].x )
      ASSERT_NEAR(floor(x_img), x, 0.001) << "[" << x << "," << y << "] vs [" << x_img << ", " << y_img << "]"
                                          << "(" << position.x << "," << position.y << "," << position.z << ")";
      ASSERT_NEAR(floor(y_img), y, 0.001) << "[" << x << "," << y << "] vs [" << x_img << ", " << y_img << "]"
                                          << "(" << position.x << "," << position.y << "," << position.z << ")";
      ;

      idx += 1;
    }
  }
  //  std::cout << "On exit: " << GlState::queryAll() << std::endl;
}

TEST(DepthImageGeneratorTest, quantization) {

  _CheckGlError(__FILE__, __LINE__);
  //  std::cout << "On entry: " << GlState::queryAll() << std::endl;
  uint32_t width = 100, height = 300;
  GlFramebuffer fbo(width, height);

  _CheckGlError(__FILE__, __LINE__);

  GlTexture input{width, height, TextureFormat::RGBA_FLOAT};
  std::vector<float> values(3 * width * height, 0);
  for (uint32_t i = 0; i < width; ++i) {
    for (uint32_t j = 0; j < height; ++j) {
      values[3 * (i + (height - j - 1) * width)] = float(i) / float(width);
      values[3 * (i + (height - j - 1) * width) + 1] = float(j) / float(height);
      values[3 * (i + (height - j - 1) * width) + 2] = 0;
    }
  }
  _CheckGlError(__FILE__, __LINE__);
  input.assign(PixelFormat::RGB, PixelType::FLOAT, &values[0]);
  ASSERT_NO_THROW(_CheckGlError(__FILE__, __LINE__));

  GlTexture output{width, height, TextureFormat::RGBA_FLOAT};
  GlRenderbuffer rbo(width, height, RenderbufferFormat::DEPTH_STENCIL);

  CheckGlError();
  fbo.attach(FramebufferAttachment::COLOR0, output);
  fbo.attach(FramebufferAttachment::DEPTH_STENCIL, rbo);
  CheckGlError();

  GlProgram program;
  program.attach(GlShader::fromCache(ShaderType::VERTEX_SHADER, "shader/empty.vert"));
  program.attach(GlShader::fromCache(ShaderType::GEOMETRY_SHADER, "shader/quad.geom"));
  program.attach(GlShader::fromCache(ShaderType::FRAGMENT_SHADER, "shader/test.frag"));
  program.link();

  program.setUniform(GlUniform<int>("input", 0));
  program.setUniform(GlUniform<float>("offset", 0.25 * 1.0f / width));

  GlSampler sampler;
  sampler.setMagnifyingOperation(TexMagOp::NEAREST);
  sampler.setMinifyingOperation(TexMinOp::NEAREST);

  GlVertexArray vao_no_points;

  fbo.bind();
  vao_no_points.bind();
  glActiveTexture(GL_TEXTURE0);
  input.bind();
  program.bind();

  sampler.bind(0);

  glDisable(GL_DEPTH_TEST);
  glViewport(0, 0, width, height);

  glDrawArrays(GL_POINTS, 0, 1);

  program.release();
  input.release();
  vao_no_points.release();
  fbo.release();

  sampler.release(0);

  glEnable(GL_DEPTH_TEST);

  std::vector<vec4> data;
  output.download(data);

  //  for (uint32_t i = 0; i < 10; ++i)
  //  {
  //    std::cout << "(" << data[i].x << ", " << data[i].y << ")";
  //    std::cout << "<=> " << data[i].x * width << ", " << data[i].y * height << std::endl;
  //  }
  //  std::cout << "On exit: " << GlState::queryAll() << std::endl;
}

TEST(DepthImageGeneratorTest, testNDC2textureCoords) {
  //  std::cout << "On entry: " << GlState::queryAll() << std::endl;
  uint32_t width = 720, height = 64;
  GlFramebuffer fbo(width, height);

  ASSERT_NO_THROW(_CheckGlError(__FILE__, __LINE__));

  GlTexture output{width, height, TextureFormat::RGBA_FLOAT};
  GlRenderbuffer rbo(width, height, RenderbufferFormat::DEPTH_STENCIL);

  fbo.attach(FramebufferAttachment::COLOR0, output);
  CheckGlError();
  fbo.attach(FramebufferAttachment::DEPTH_STENCIL, rbo);
  CheckGlError();

  GlProgram program;
  program.attach(GlShader::fromCache(ShaderType::VERTEX_SHADER, "shader/ndc.vert"));
  program.attach(GlShader::fromCache(ShaderType::FRAGMENT_SHADER, "shader/ndc.frag"));
  program.link();

  GlBuffer<vec4> buffer{BufferTarget::ARRAY_BUFFER, BufferUsage::STATIC_DRAW};

  std::vector<vec4> pixels;
  for (uint32_t i = 0; i < width; ++i) {
    for (uint32_t j = 0; j < height; ++j) {
      vec4 v;
      v.x = 2.0f * (float(i + 0.5f) / float(width)) - 1.0f;
      v.y = 2.0f * (float(j + 0.5f) / float(height)) - 1.0f;
      v.z = i;
      v.w = j;
      pixels.push_back(v);
    }
  }

  buffer.assign(pixels);

  GlVertexArray vao;
  vao.setVertexAttribute(0, buffer, 4, AttributeType::FLOAT, false, 4 * sizeof(float), nullptr);
  vao.enableVertexAttribute(0);

  glDisable(GL_DEPTH_TEST);

  fbo.bind();
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  glViewport(0, 0, width, height);
  program.bind();
  vao.bind();

  glDrawArrays(GL_POINTS, 0, buffer.size());

  vao.release();
  program.release();
  fbo.release();

  glEnable(GL_DEPTH_TEST);

  std::vector<vec4> values;
  output.download(values);

  //  std::ofstream result("pixelized.txt");
  //  uint32_t idx = 0;
  for (uint32_t i = 0; i < width; ++i) {
    for (uint32_t j = 0; j < height; ++j) {
      //      result.precision(2);
      //      result << "([" << values[i + j * width].x << "," << values[i + j * width].y  << "], [" << values[i + j *
      //      width].z << "," << values[i + j * width].w
      //          << "]) ";

      ASSERT_NEAR(i, values[i + j * width].z, 0.000001);
      ASSERT_NEAR(j, values[i + j * width].w, 0.000001);
    }
    //    result << std::endl;
  }

  //  result.close();

  //  std::cout << "On exit: " << GlState::queryAll() << std::endl;
}
