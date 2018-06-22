#include <gtest/gtest.h>

#include <core/Frame2Model.h>
#include <core/Preprocessing.h>
#include <rv/PrimitiveParameters.h>
#include "io/KITTIReader.h"

#include <core/LieGaussNewton.h>

using namespace rv;
using namespace glow;

// ground truth pose: 9.990498e-01 -1.649780e-03 4.355194e-02 5.154656e-02 1.760423e-03 9.999953e-01 -2.502237e-03
// -2.424883e-02 -4.354760e-02 2.576529e-03 9.990480e-01 1.000725e+00

TEST(IcpTest, testFrame2Model_opengl) {
  ASSERT_NO_THROW(_CheckGlError(__FILE__, __LINE__));

  uint32_t width = 720;
  uint32_t height = 64;
  ParameterList params;
  params.insert(IntegerParameter("data_width", width));
  params.insert(IntegerParameter("data_height", height));
  params.insert(FloatParameter("data_fov_up", 2.5));
  params.insert(FloatParameter("data_fov_down", -24.8));
  params.insert(FloatParameter("min_depth", 0.5f));
  params.insert(FloatParameter("max_depth", 100.0f));
  params.insert(FloatParameter("icp-max-angle", 50.0f));
  params.insert(FloatParameter("icp-max-distance", 2.0f));
  params.insert(FloatParameter("cutoff_threshold", 100.0f));

  KITTIReader reader("./scan0.bin");
  std::vector<Laserscan> scans;
  Laserscan scan;
  while (reader.read(scan)) {
    scans.push_back(scan);
  }

  ASSERT_EQ(2, scans.size());

  glow::GlBuffer<rv::Point3f> pts{glow::BufferTarget::ARRAY_BUFFER, glow::BufferUsage::DYNAMIC_READ};

  Preprocessing preprocessor(params);

  std::shared_ptr<Frame> frame1 = std::make_shared<Frame>(width, height);
  std::shared_ptr<Frame> frame2 = std::make_shared<Frame>(width, height);

  pts.assign(scans[0].points());
  preprocessor.process(pts, *frame1);

  pts.assign(scans[1].points());
  preprocessor.process(pts, *frame2);

  Frame2Model objective(params);
  objective.setData(frame1, frame1);

  Eigen::MatrixXd JtJ(6, 6);
  Eigen::MatrixXd Jtf(6, 1);

  std::cout << "First sanity check: " << std::endl;
  ASSERT_FLOAT_EQ(0.0f, objective.jacobianProducts(JtJ, Jtf));

  LieGaussNewton gn;
  ParameterList gn_params;
  gn_params.insert(IntegerParameter("max iterations", 100));
  gn_params.insert(FloatParameter("stopping threshold", 0.000001f));
  gn.setParameters(gn_params);

  std::cout << "Second sanity check: " << std::endl;
  gn.minimize(objective, Eigen::Matrix4d::Identity());
  ASSERT_EQ(true, gn.pose().isIdentity()) << gn.pose();
  std::cout << "Finished." << std::endl;

  std::cout << "Third sanity check (with small rotation): " << std::endl;
  gn.minimize(objective, glRotateX(Radians(1.0f)).cast<double>());
  std::cout << "Finished." << std::endl;

  Eigen::Matrix4d pose = gn.pose();
  for (uint32_t i = 0; i < 4; ++i) {
    for (uint32_t j = 0; j < 4; ++j) {
      if (i == j)
        ASSERT_NEAR(1.0f, pose(i, j), 0.0001f);
      else
        ASSERT_NEAR(0.0f, pose(i, j), 0.0001f);
    }
  }
  //  ASSERT_EQ(true, gn.pose().isIdentity()) << gn.pose();

  //  ASSERT_FLOAT_EQ(0.0f, objective.jacobianProducts(JtJ, Jtf));

  ASSERT_NO_THROW(glow::_CheckGlError(__FILE__, __LINE__));
}

// TODO: add tests for Frame2Model, Model2Frame.

// TEST(IcpTest, testRounding)
//{
//
//  //  result.close();
//
//  uint32_t width = 720, height = 64;
//  GlFramebuffer fbo(width, height);
//
//  rv::GlBuffer<vec4> input{BufferTarget::ARRAY_BUFFER, BufferUsage::STATIC_DRAW};
//  std::vector<vec4> values;
//  for (uint32_t i = 0; i < width; ++i)
//  {
//    for (uint32_t j = 0; j < height; ++j)
//    {
//      float x = 2.0f * (float(i + 0.5) / width) - 1.0f;
//      float y = 2.0f * (float(j + 0.5) / height) - 1.0f;
//      float z = i;
//      float w = j;
//
//      values.push_back(vec4(x, y, z, w));
//    }
//  }
//
//  std::cout << values[0] << ", " << values[1] << ", " << values[2] << std::endl;
//
//  input.assign(values);
//
//  ASSERT_NO_THROW(_CheckGlError(__FILE__, __LINE__));
//
//  rv::GlTexture ndc_values{width, height, TextureFormat::RGBA_FLOAT};
//  ndc_values.setMagnifyingOperation(TexMagOp::NEAREST);
//  ndc_values.setMinifyingOperation(TexMinOp::NEAREST);
//
//  GlRenderbuffer rbo(width, height, RenderbufferFormat::DEPTH_STENCIL);
//  fbo.attach(FramebufferAttachment::COLOR0, ndc_values);
//  fbo.attach(FramebufferAttachment::DEPTH_STENCIL, rbo);
//
//  // Filling the texture with points expressed in NDCs
//  GlProgram fill_program;
//  fill_program.attach(GlShader::fromFile(ShaderType::VERTEX_SHADER, "shader/rounding.vert"));
//  fill_program.attach(GlShader::fromFile(ShaderType::FRAGMENT_SHADER, "shader/rounding.frag"));
//  fill_program.link();
//
//  GlSampler sampler;
//  sampler.setMagnifyingOperation(TexMagOp::NEAREST);
//  sampler.setMinifyingOperation(TexMinOp::NEAREST);
//  sampler.setWrapOperation(TexWrapOp::CLAMP_TO_BORDER, TexWrapOp::CLAMP_TO_BORDER);
//
//  GlVertexArray vao_points;
//  vao_points.enableVertexAttribute(0);
//  vao_points.setVertexAttribute(0, input, 4, AttributeType::FLOAT, false, 4 * sizeof(float), nullptr);
//
////  program.setUniform(GlUniform<int>("input", 0));
////  program.setUniform(GlUniform<float>("offset", 0));
//
//  fbo.bind();
//  vao_points.bind();
//  fill_program.bind();
//
//  glDisable(GL_DEPTH_TEST);
//  glViewport(0, 0, width, height);
//
//  glDrawArrays(GL_POINTS, 0, input.size());
//
//  fill_program.release();
//  vao_points.release();
//  fbo.release();
//
//  glFinish();
//
//  std::vector<vec4> data;
//  ndc_values.download(data);
//
//  std::cout << "data contains: " << std::endl;
//  for (uint32_t j = 0; j < 3; ++j)
//  {
//    for (uint32_t i = 0; i < 3; ++i)
//    {
//
//      std::cout << data[i + j * width] << ", ";
//    }
//    std::cout << "..." << std::endl;
//  }
//
//  std::cout << std::endl;
//
//  for (uint32_t i = 0; i < width; ++i)
//  {
//    for (uint32_t j = 0; j < height; ++j)
//    {
////      std::cout << data[i + j * width] << std::endl;
//      ASSERT_EQ(i, data[i + j * width].z);
//      ASSERT_EQ(j, data[i + j * width].w);
//    }
//  }
//
//  GlTexture access_result(width, height, TextureFormat::RGBA_FLOAT);
//  GlRenderbuffer rbo2(width, height, RenderbufferFormat::DEPTH_STENCIL);
//  // now access the same points with texture coordinates.
//  GlFramebuffer access_fbo(width, height);
//  access_fbo.attach(FramebufferAttachment::COLOR0, access_result);
//  access_fbo.attach(FramebufferAttachment::DEPTH_STENCIL, rbo2);
//
//  GlProgram access_program;
//  access_program.attach(GlShader::fromFile(ShaderType::VERTEX_SHADER, "shader/empty.vert"));
//  access_program.attach(GlShader::fromFile(ShaderType::GEOMETRY_SHADER, "shader/quad.geom"));
//  access_program.attach(GlShader::fromFile(ShaderType::FRAGMENT_SHADER, "shader/rounding_access.frag"));
//  access_program.link();
//
//  access_program.setUniform(GlUniform<int32_t>("tex_input", 0));
//  access_program.setUniform(GlUniform<int32_t>("tex_values", 1));
//
//  GlTexture access_texture(width, height, TextureFormat::RGBA_FLOAT);
//  std::vector<vec4> access_values(width * height);
//
//  float x_start = 0.0f;
//  float y_start = 0.0f;
//  uint32_t num_values = 30;
//
//  for (uint32_t i = 0; i < num_values; ++i)
//  {
//    access_values[i].x = (x_start + i * 0.1f) / width;
//    access_values[i].y = (y_start) / height;
//    access_values[i].z = floor(access_values[i].x * width);
//    access_values[i].w = floor(access_values[i].y * height);
//  }
//
//  for (uint32_t i = 0; i < num_values; ++i)
//  {
//    access_values[i + num_values].x = (10 + 0.5f) / width;
//    access_values[i + num_values].y = (10 + 0.5f + i * 0.1f) / height;
//    access_values[i + num_values].z = floor(access_values[i + num_values].x * width);
//    access_values[i + num_values].w = floor(access_values[i + num_values].y * height);
//  }
//
//  access_texture.assign(PixelFormat::RGBA, PixelType::FLOAT, &access_values[0]);
//
//  glActiveTexture(GL_TEXTURE0);
//  access_texture.bind();
//  sampler.bind(0);
//
//  glActiveTexture(GL_TEXTURE1);
//  ndc_values.bind();
//  sampler.bind(1);
//
//  access_fbo.bind();
//  GlVertexArray vao_no_points;
//  vao_no_points.bind();
//  access_program.bind();
//
//  glDrawArrays(GL_POINTS, 0, 1);
//
//  access_program.release();
//  vao_no_points.release();
//  access_texture.release();
//  glEnable(GL_DEPTH_TEST);
//  sampler.release(0);
//  sampler.release(1);
//
//  access_result.download(data);
//  access_fbo.release();
//
//  for (uint32_t i = 0; i < 60; ++i)
//  {
////    std::cout ;
////    std::cout << " ==> (" << data[i].z << ", " << data[i].w << ") [" << data[i].x << ", " << data[i].y << "]"
////        << std::endl;
//
//    ASSERT_EQ(access_values[i].z, data[i].z)<< access_values[i].z << ", " << access_values[i].w << " -- " <<
//    access_values[i].x * width << ", "
//    << access_values[i].y * height << " ==> (" << data[i].z << ", " << data[i].w << ") [" << data[i].x << ", " <<
//    data[i].y << "]"
//    << std::endl;
//    ASSERT_EQ(access_values[i].w, data[i].w) << access_values[i].z << ", " << access_values[i].w << " -- " <<
//    access_values[i].x * width << ", "
//    << access_values[i].y * height << " ==> (" << data[i].z << ", " << data[i].w << ") [" << data[i].x << ", " <<
//    data[i].y << "]"
//    << std::endl;
//  }
//}

TEST(IcpTest, testRounding_rect) {
  return;
  //  result.close();
  ASSERT_NO_THROW(_CheckGlError(__FILE__, __LINE__));

  uint32_t width = 720, height = 64;
  GlFramebuffer fbo(width, height);

  glow::GlBuffer<vec4> input{BufferTarget::ARRAY_BUFFER, BufferUsage::STATIC_DRAW};
  std::vector<vec4> values;
  for (uint32_t i = 0; i < width; ++i) {
    for (uint32_t j = 0; j < height; ++j) {
      float x = 2.0f * (float(i + 0.5) / width) - 1.0f;
      float y = 2.0f * (float(j + 0.5) / height) - 1.0f;
      float z = i;
      float w = j;

      values.push_back(vec4(x, y, z, w));
    }
  }

  ASSERT_NO_THROW(_CheckGlError(__FILE__, __LINE__));

  //  std::cout << values[0] << ", " << values[1] << ", " << values[2] << std::endl;

  input.assign(values);

  ASSERT_NO_THROW(_CheckGlError(__FILE__, __LINE__));

  glow::GlTextureRectangle ndc_values{width, height, TextureFormat::RGBA_FLOAT};  // FIXME: This causes problems?

  ASSERT_NO_THROW(_CheckGlError(__FILE__, __LINE__));
  ndc_values.setMagnifyingOperation(TexRectMagOp::NEAREST);
  ndc_values.setMinifyingOperation(TexRectMinOp::NEAREST);

  ASSERT_NO_THROW(_CheckGlError(__FILE__, __LINE__));

  GlRenderbuffer rbo(width, height, RenderbufferFormat::DEPTH_STENCIL);
  fbo.attach(FramebufferAttachment::COLOR0, ndc_values);
  fbo.attach(FramebufferAttachment::DEPTH_STENCIL, rbo);

  ASSERT_NO_THROW(_CheckGlError(__FILE__, __LINE__));

  // Filling the texture with points expressed in NDCs
  GlProgram fill_program;
  fill_program.attach(GlShader::fromFile(ShaderType::VERTEX_SHADER, "shader/rounding.vert"));
  fill_program.attach(GlShader::fromFile(ShaderType::FRAGMENT_SHADER, "shader/rounding.frag"));
  fill_program.link();

  GlSampler sampler;
  sampler.setMagnifyingOperation(TexMagOp::NEAREST);
  sampler.setMinifyingOperation(TexMinOp::NEAREST);
  sampler.setWrapOperation(TexWrapOp::CLAMP_TO_BORDER, TexWrapOp::CLAMP_TO_BORDER);

  GlVertexArray vao_points;
  vao_points.enableVertexAttribute(0);
  vao_points.setVertexAttribute(0, input, 4, AttributeType::FLOAT, false, 4 * sizeof(float), nullptr);

  //  program.setUniform(GlUniform<int>("input", 0));
  //  program.setUniform(GlUniform<float>("offset", 0));

  fbo.bind();
  vao_points.bind();
  fill_program.bind();

  glDisable(GL_DEPTH_TEST);
  glViewport(0, 0, width, height);

  glDrawArrays(GL_POINTS, 0, input.size());

  fill_program.release();
  vao_points.release();
  fbo.release();

  glFinish();

  ASSERT_NO_THROW(_CheckGlError(__FILE__, __LINE__));

  std::vector<vec4> data;
  ndc_values.download(data);

  //  std::cout << "data contains: " << std::endl;
  //  for (uint32_t j = 0; j < 3; ++j)
  //  {
  //    for (uint32_t i = 0; i < 3; ++i)
  //    {
  //
  //      std::cout << data[i + j * width] << ", ";
  //    }
  //    std::cout << "..." << std::endl;
  //  }
  //
  //  std::cout << std::endl;

  for (uint32_t i = 0; i < width; ++i) {
    for (uint32_t j = 0; j < height; ++j) {
      //      std::cout << data[i + j * width] << std::endl;
      ASSERT_EQ(i, data[i + j * width].z);
      ASSERT_EQ(j, data[i + j * width].w);
    }
  }

  ASSERT_NO_THROW(_CheckGlError(__FILE__, __LINE__));

  GlTextureRectangle access_result(width, height, TextureFormat::RGBA_FLOAT);
  GlRenderbuffer rbo2(width, height, RenderbufferFormat::DEPTH_STENCIL);
  // now access the same points with texture coordinates.
  GlFramebuffer access_fbo(width, height);
  access_fbo.attach(FramebufferAttachment::COLOR0, access_result);
  access_fbo.attach(FramebufferAttachment::DEPTH_STENCIL, rbo2);

  GlProgram access_program;
  access_program.attach(GlShader::fromFile(ShaderType::VERTEX_SHADER, "shader/empty.vert"));
  access_program.attach(GlShader::fromFile(ShaderType::GEOMETRY_SHADER, "shader/quad.geom"));
  access_program.attach(GlShader::fromFile(ShaderType::FRAGMENT_SHADER, "shader/rounding_access_rect.frag"));
  access_program.link();

  access_program.setUniform(GlUniform<int32_t>("tex_input", 0));
  access_program.setUniform(GlUniform<int32_t>("tex_values", 1));

  GlTexture access_texture(width, height, TextureFormat::RGBA_FLOAT);
  std::vector<vec4> access_values(width * height);

  float x_start = 0.0f;
  float y_start = 0.0f;
  uint32_t num_values = 30;

  for (uint32_t i = 0; i < num_values; ++i) {
    access_values[i].x = (x_start + i * 0.1f);
    access_values[i].y = (y_start);
    access_values[i].z = floor(access_values[i].x);
    access_values[i].w = floor(access_values[i].y);
  }

  for (uint32_t i = 0; i < num_values; ++i) {
    access_values[i + num_values].x = (10 + 0.5f);
    access_values[i + num_values].y = (10 + 0.5f + i * 0.1f);
    access_values[i + num_values].z = floor(access_values[i + num_values].x);
    access_values[i + num_values].w = floor(access_values[i + num_values].y);
  }

  ASSERT_NO_THROW(_CheckGlError(__FILE__, __LINE__));

  access_texture.assign(PixelFormat::RGBA, PixelType::FLOAT, &access_values[0]);

  glActiveTexture(GL_TEXTURE0);
  access_texture.bind();
  sampler.bind(0);

  glActiveTexture(GL_TEXTURE1);
  ndc_values.bind();
  sampler.bind(1);

  access_fbo.bind();
  GlVertexArray vao_no_points;
  vao_no_points.bind();
  access_program.bind();

  glDrawArrays(GL_POINTS, 0, 1);

  access_program.release();
  vao_no_points.release();
  access_texture.release();
  glEnable(GL_DEPTH_TEST);
  sampler.release(0);
  sampler.release(1);

  access_fbo.release();

  ASSERT_NO_THROW(_CheckGlError(__FILE__, __LINE__));

  access_result.download(data);

  ASSERT_NO_THROW(_CheckGlError(__FILE__, __LINE__));

  for (uint32_t i = 0; i < 60; ++i) {
    //    std::cout << access_values[i].z << ", " << access_values[i].w << " -- " << access_values[i].x << ", "
    //        << access_values[i].y;
    //    std::cout << " ==> (" << data[i].z << ", " << data[i].w << ") [" << data[i].x << ", " << data[i].y << "]"
    //        << std::endl;

    ASSERT_EQ(access_values[i].z, data[i].z) << access_values[i].z << ", " << access_values[i].w << " -- "
                                             << access_values[i].x * width << ", " << access_values[i].y * height
                                             << " ==> (" << data[i].z << ", " << data[i].w << ") [" << data[i].x << ", "
                                             << data[i].y << "]" << std::endl;
    ASSERT_EQ(access_values[i].w, data[i].w) << access_values[i].z << ", " << access_values[i].w << " -- "
                                             << access_values[i].x * width << ", " << access_values[i].y * height
                                             << " ==> (" << data[i].z << ", " << data[i].w << ") [" << data[i].x << ", "
                                             << data[i].y << "]" << std::endl;
  }

  ASSERT_NO_THROW(_CheckGlError(__FILE__, __LINE__));
}
