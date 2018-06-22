/*
 * Preprocessing.cpp
 *
 *  Created on: Jul 29, 2016
 *      Author: behley
 */

#include "core/Preprocessing.h"

#include <glow/GlState.h>
#include <glow/glutil.h>

using namespace rv;
using namespace glow;

// -- OpenGL-based preprocessing:

inline void PASS(const char* sFile, const int nLine) {
  std::cout << "Passed: " << sFile << " on Line: " << nLine << std::endl;
}

Preprocessing::Preprocessing(const rv::ParameterList& params)
    : width_(params["data_width"]),
      height_(params["data_height"]),
      framebuffer_(width_, height_, FramebufferTarget::BOTH),
      temp_vertices_(width_, height_, TextureFormat::RGBA_FLOAT) {

  depth_program_.attach(GlShader::fromCache(ShaderType::VERTEX_SHADER, "shader/gen_vertexmap.vert"));
  depth_program_.attach(GlShader::fromCache(ShaderType::FRAGMENT_SHADER, "shader/gen_vertexmap.frag"));
  depth_program_.link();

  avg_program_.attach(GlShader::fromCache(ShaderType::VERTEX_SHADER, "shader/empty.vert"));
  avg_program_.attach(GlShader::fromCache(ShaderType::GEOMETRY_SHADER, "shader/quad.geom"));
  avg_program_.attach(GlShader::fromCache(ShaderType::FRAGMENT_SHADER, "shader/avg_vertexmap.frag"));
  avg_program_.link();

  bilateral_program_.attach(GlShader::fromCache(ShaderType::VERTEX_SHADER, "shader/empty.vert"));
  bilateral_program_.attach(GlShader::fromCache(ShaderType::GEOMETRY_SHADER, "shader/quad.geom"));
  bilateral_program_.attach(GlShader::fromCache(ShaderType::FRAGMENT_SHADER, "shader/bilateral_filter.frag"));
  bilateral_program_.link();

  normal_program_.attach(GlShader::fromCache(ShaderType::VERTEX_SHADER, "shader/empty.vert"));
  normal_program_.attach(GlShader::fromCache(ShaderType::GEOMETRY_SHADER, "shader/quad.geom"));
  normal_program_.attach(GlShader::fromCache(ShaderType::FRAGMENT_SHADER, "shader/gen_normalmap.frag"));
  normal_program_.link();

  // 1. setup framebuffer.
  GlRenderbuffer rbo(width_, height_, RenderbufferFormat::DEPTH_STENCIL);
  framebuffer_.attach(FramebufferAttachment::DEPTH_STENCIL, rbo);

  sampler_.setMinifyingOperation(TexMinOp::NEAREST);
  sampler_.setMagnifyingOperation(TexMagOp::NEAREST);
  sampler_.setWrapOperation(TexWrapOp::CLAMP_TO_BORDER, TexWrapOp::CLAMP_TO_BORDER);

  // finally set parameters of shaders, etc.
  setParameters(params);
}

void Preprocessing::setParameters(const rv::ParameterList& params) {
  float fov_up = params["data_fov_up"];
  float fov_down = params["data_fov_down"];
  float sigma_space = 0.0f, sigma_range = 0.0f;

  useFilteredVertexmap_ = false;
  filterVertexmap_ = false;
  avgVertexmap_ = false;
  if (params.hasParam("filter_vertexmap")) filterVertexmap_ = params["filter_vertexmap"];
  if (params.hasParam("avg_vertexmap")) avgVertexmap_ = params["avg_vertexmap"];
  if (filterVertexmap_) {
    sigma_space = params["bilateral_sigma_space"];
    sigma_range = params["bilateral_sigma_range"];
    useFilteredVertexmap_ = params["use_filtered_vertexmap"];
  }

  fov_up = std::abs(fov_up);
  fov_down = std::abs(fov_down);

  depth_program_.setUniform(GlUniform<float>("width", width_));
  depth_program_.setUniform(GlUniform<float>("height", height_));
  depth_program_.setUniform(GlUniform<float>("fov_up", fov_up));
  depth_program_.setUniform(GlUniform<float>("fov_down", fov_down));
  depth_program_.setUniform(GlUniform<float>("min_depth", (float)params["min_depth"]));
  depth_program_.setUniform(GlUniform<float>("max_depth", (float)params["max_depth"]));

  bilateral_program_.setUniform(GlUniform<float>("width", width_));
  bilateral_program_.setUniform(GlUniform<float>("height", height_));
  bilateral_program_.setUniform(GlUniform<float>("sigma_space", sigma_space));
  bilateral_program_.setUniform(GlUniform<float>("sigma_range", sigma_range));
}

/** \brief pre-process the point cloud (validity map, vertex map, normal map, ... etc.) and store results in frame. **/
void Preprocessing::process(glow::GlBuffer<rv::Point3f>& points, Frame& frame) {
  CheckGlError();

  frame.points.assign(points);

  GLboolean depthTest;
  GLint ov[4], depthFunc, old_fbo;
  GLfloat cc[4];
  GLfloat psize;

  glGetIntegerv(GL_VIEWPORT, ov);
  glGetBooleanv(GL_DEPTH_TEST, &depthTest);
  glGetIntegerv(GL_DEPTH_FUNC, &depthFunc);
  glGetFloatv(GL_COLOR_CLEAR_VALUE, cc);
  glGetFloatv(GL_POINT_SIZE, &psize);
  glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &old_fbo);

  glPointSize(1.0f);

  vao_points_.setVertexAttribute(0, points, 4, AttributeType::FLOAT, false, 4 * sizeof(float), nullptr);
  vao_points_.enableVertexAttribute(0);

  // 1. pass: generate raw vertex map:
  if (avgVertexmap_)
    framebuffer_.attach(FramebufferAttachment::COLOR0, temp_vertices_);
  else
    framebuffer_.attach(FramebufferAttachment::COLOR0, frame.vertex_map);

  assert(framebuffer_.valid());
  framebuffer_.bind();
  sampler_.bind(0);

  glDepthFunc(GL_LESS);

  if (avgVertexmap_) {
    // average vertex map.
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);  // computing the sum of values
  } else {
    // z-buffered vertex map
    glEnable(GL_DEPTH_TEST);
  }

  glClearColor(0, 0, 0, 0);
  glViewport(0, 0, width_, height_);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // reset depth/vertexmap

  vao_points_.bind();
  depth_program_.bind();

  glDrawArrays(GL_POINTS, 0, points.size());

  depth_program_.release();
  vao_points_.release();
  framebuffer_.release();
  glFinish();

  if (avgVertexmap_) {
    glDisable(GL_BLEND);

    framebuffer_.attach(FramebufferAttachment::COLOR0, frame.vertex_map);
    framebuffer_.bind();
    vao_no_points_.bind();
    avg_program_.bind();

    glActiveTexture(GL_TEXTURE0);
    temp_vertices_.bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_POINTS, 0, 1);

    temp_vertices_.release();

    vao_no_points_.release();
    avg_program_.release();
    framebuffer_.release();

    glEnable(GL_DEPTH_TEST);
  }

  // step 1.5: do bilateral filtering of vertex map.
  if (filterVertexmap_) {
    framebuffer_.attach(FramebufferAttachment::COLOR0, temp_vertices_);

    framebuffer_.bind();
    vao_no_points_.bind();
    glActiveTexture(GL_TEXTURE0);

    frame.vertex_map.bind();

    bilateral_program_.bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_POINTS, 0, 1);

    vao_no_points_.release();
    bilateral_program_.release();
    frame.vertex_map.release();
    framebuffer_.release();

    if (useFilteredVertexmap_) frame.vertex_map.copy(temp_vertices_);
  }

  // 2. pass: generate normal map:
  glDisable(GL_DEPTH_TEST);

  framebuffer_.attach(FramebufferAttachment::COLOR0, frame.normal_map);
  assert(framebuffer_.valid());
  framebuffer_.bind();

  glActiveTexture(GL_TEXTURE0);
  if (filterVertexmap_)
    temp_vertices_.bind();
  else
    frame.vertex_map.bind();

  vao_no_points_.bind();
  normal_program_.bind();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // reset depth/normalmap
  glDrawArrays(GL_POINTS, 0, 1);

  normal_program_.release();
  vao_no_points_.release();

  if (filterVertexmap_)
    temp_vertices_.bind();
  else
    frame.vertex_map.bind();

  // finally filter normals.

  framebuffer_.release();

  glFinish();

  frame.valid = true;

  // restore settings.
  glViewport(ov[0], ov[1], ov[2], ov[3]);
  if (depthTest) glEnable(GL_DEPTH_TEST);

  glDepthFunc(depthFunc);
  glClearColor(cc[0], cc[1], cc[2], cc[3]);
  glPointSize(psize);
  glBindFramebuffer(GL_FRAMEBUFFER, old_fbo);

  sampler_.release(0);

  CheckGlError();
}
