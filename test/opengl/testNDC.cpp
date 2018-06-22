#include <glow/glbase.h>
#include <glow/glutil.h>
#include <gtest/gtest.h>

#include <glow/GlBuffer.h>
#include <glow/GlFramebuffer.h>
#include <glow/GlProgram.h>
#include <glow/GlVertexArray.h>
#include <glow/ScopedBinder.h>

#include <algorithm>
#include <random>
#include <vector>

//using namespace rv;
using namespace glow;

std::vector<vec2> initializeData(uint32_t width, uint32_t height, uint32_t size) {
  std::vector<vec2> data;
  data.reserve(width * height);
  for (uint32_t i = 0; i < width; ++i) {
    for (uint32_t j = 0; j < height; ++j) {
      data.push_back(vec2(i, j));
    }
  }

  std::mt19937 gen(1337);
  std::shuffle(data.begin(), data.end(), gen);

  data.resize(size);

  // boundary cases.
  data.push_back(vec2(0, 0));
  data.push_back(vec2(1, 1));
  data.push_back(vec2(2, 2));
  data.push_back(vec2(width - 1, 0));
  data.push_back(vec2(0, height - 1));
  data.push_back(vec2(width - 1, height - 1));

  return data;
}

std::vector<vec4> generateOutput(uint32_t width, uint32_t height, std::vector<vec2>& pixels) {
  GlProgram program;
  program.attach(GlShader::fromCache(ShaderType::VERTEX_SHADER, "shader/pix2ndc.vert"));
  program.attach(GlShader::fromCache(ShaderType::FRAGMENT_SHADER, "shader/pix2ndc.frag"));
  program.link();

  program.setUniform(GlUniform<int32_t>("width", width));
  program.setUniform(GlUniform<int32_t>("height", height));

  GlFramebuffer fbo(width, height, FramebufferTarget::DRAW);

  GlTexture tex_output{width, height, TextureFormat::RGBA_FLOAT};
  GlRenderbuffer rbo(width, height, RenderbufferFormat::DEPTH_STENCIL);
  fbo.attach(FramebufferAttachment::COLOR0, tex_output);
  fbo.attach(FramebufferAttachment::DEPTH_STENCIL, rbo);
  CheckGlError();

  GlBuffer<vec2> buffer{BufferTarget::ARRAY_BUFFER, BufferUsage::STATIC_DRAW};
  buffer.assign(pixels);

  GlVertexArray vao;
  vao.setVertexAttribute(0, buffer, 2, AttributeType::FLOAT, false, 2 * sizeof(GL_FLOAT), nullptr);
  vao.enableVertexAttribute(0);

  ScopedBinder<GlFramebuffer> fbo_bind(fbo);
  ScopedBinder<GlProgram> program_bind(program);
  ScopedBinder<GlVertexArray> vao_bind(vao);

  glDisable(GL_DEPTH_TEST);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  glViewport(0, 0, width, height);

  glDrawArrays(GL_POINTS, 0, buffer.size());

  std::vector<vec4> output;
  tex_output.download(output);

  return output;
}

TEST(NdcTest, testFramebuffer_mini) {
  uint32_t width = 32, height = 32;

  std::vector<vec2> pixels = initializeData(width, height, 10);
  std::vector<vec4> output = generateOutput(width, height, pixels);

  for (uint32_t i = 0; i < pixels.size(); ++i) {
    const vec2& expected = pixels[i];
    vec4& out = output[expected.x + expected.y * width];

    ASSERT_EQ(int(expected.x), int(out.x));
    ASSERT_EQ(int(expected.y), int(out.y));

    //    std::cout << expected.x << ", " << expected.y << " and got " << out.x << ", " << out.y << std::endl;

    out.x = out.y = out.z = out.w = 0.0f;
  }

  for (uint32_t i = 0; i < output.size(); ++i) {
    vec4& out = output[i];

    ASSERT_NEAR(0.0f, out.x, 0.0000001f);
    ASSERT_NEAR(0.0f, out.y, 0.0000001f);
    ASSERT_NEAR(0.0f, out.z, 0.0000001f);
    ASSERT_NEAR(0.0f, out.w, 0.0000001f);
  }
}

TEST(NdcTest, testFramebuffer_mini_uneven) {
  uint32_t width = 11, height = 33;

  std::vector<vec2> pixels = initializeData(width, height, 10);
  std::vector<vec4> output = generateOutput(width, height, pixels);

  for (uint32_t i = 0; i < pixels.size(); ++i) {
    const vec2& expected = pixels[i];
    vec4& out = output[expected.x + expected.y * width];

    ASSERT_EQ(int(expected.x), int(out.x));
    ASSERT_EQ(int(expected.y), int(out.y));

    //    std::cout << expected.x << ", " << expected.y << " and got " << out.x << ", " << out.y << std::endl;

    out.x = out.y = out.z = out.w = 0.0f;
  }

  for (uint32_t i = 0; i < output.size(); ++i) {
    vec4& out = output[i];

    ASSERT_NEAR(0.0f, out.x, 0.0000001f);
    ASSERT_NEAR(0.0f, out.y, 0.0000001f);
    ASSERT_NEAR(0.0f, out.z, 0.0000001f);
    ASSERT_NEAR(0.0f, out.w, 0.0000001f);
  }
}

TEST(NdcTest, testFramebuffer_square) {
  uint32_t width = 512, height = 512;

  std::vector<vec2> pixels = initializeData(width, height, 100);
  std::vector<vec4> output = generateOutput(width, height, pixels);

  for (uint32_t i = 0; i < pixels.size(); ++i) {
    const vec2& expected = pixels[i];
    vec4& out = output[expected.x + expected.y * width];

    ASSERT_EQ(int(expected.x), int(out.x));
    ASSERT_EQ(int(expected.y), int(out.y));

    out.x = out.y = out.z = out.w = 0.0f;
  }

  for (uint32_t i = 0; i < output.size(); ++i) {
    vec4& out = output[i];

    ASSERT_NEAR(0.0f, out.x, 0.0000001f);
    ASSERT_NEAR(0.0f, out.y, 0.0000001f);
    ASSERT_NEAR(0.0f, out.z, 0.0000001f);
    ASSERT_NEAR(0.0f, out.w, 0.0000001f);
  }
}

TEST(NdcTest, testFramebuffer_rectangle) {
  uint32_t width = 512, height = 256;

  std::vector<vec2> pixels = initializeData(width, height, 100);
  std::vector<vec4> output = generateOutput(width, height, pixels);

  for (uint32_t i = 0; i < pixels.size(); ++i) {
    const vec2& expected = pixels[i];
    vec4& out = output[expected.x + expected.y * width];

    ASSERT_EQ(int(expected.x), int(out.x));
    ASSERT_EQ(int(expected.y), int(out.y));

    out.x = out.y = out.z = out.w = 0.0f;
  }

  for (uint32_t i = 0; i < output.size(); ++i) {
    vec4& out = output[i];

    ASSERT_NEAR(0.0f, out.x, 0.0000001f);
    ASSERT_NEAR(0.0f, out.y, 0.0000001f);
    ASSERT_NEAR(0.0f, out.z, 0.0000001f);
    ASSERT_NEAR(0.0f, out.w, 0.0000001f);
  }
}

TEST(NdcTest, testFramebuffer_uneven) {
  uint32_t width = 103, height = 55;

  std::vector<vec2> pixels = initializeData(width, height, 100);
  std::vector<vec4> output = generateOutput(width, height, pixels);

  for (uint32_t i = 0; i < pixels.size(); ++i) {
    const vec2& expected = pixels[i];
    vec4& out = output[expected.x + expected.y * width];

    ASSERT_EQ(int(expected.x), int(out.x));
    ASSERT_EQ(int(expected.y), int(out.y));

    out.x = out.y = out.z = out.w = 0.0f;
  }

  for (uint32_t i = 0; i < output.size(); ++i) {
    vec4& out = output[i];

    ASSERT_NEAR(0.0f, out.x, 0.0000001f);
    ASSERT_NEAR(0.0f, out.y, 0.0000001f);
    ASSERT_NEAR(0.0f, out.z, 0.0000001f);
    ASSERT_NEAR(0.0f, out.w, 0.0000001f);
  }
}

TEST(NdcTest, testFramebuffer_uneven_large) {
  uint32_t width = 1783, height = 1233;

  std::vector<vec2> pixels = initializeData(width, height, 1000);
  std::vector<vec4> output = generateOutput(width, height, pixels);

  for (uint32_t i = 0; i < pixels.size(); ++i) {
    const vec2& expected = pixels[i];
    vec4& out = output[expected.x + expected.y * width];

    ASSERT_EQ(int(expected.x), int(out.x));
    ASSERT_EQ(int(expected.y), int(out.y));

    out.x = out.y = out.z = out.w = 0.0f;
  }

  for (uint32_t i = 0; i < output.size(); ++i) {
    vec4& out = output[i];

    ASSERT_NEAR(0.0f, out.x, 0.0000001f);
    ASSERT_NEAR(0.0f, out.y, 0.0000001f);
    ASSERT_NEAR(0.0f, out.z, 0.0000001f);
    ASSERT_NEAR(0.0f, out.w, 0.0000001f);
  }
}
