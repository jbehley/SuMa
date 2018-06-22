#include <gtest/gtest.h>

#include <core/Frame2Model.h>
#include <core/Preprocessing.h>
#include <rv/PrimitiveParameters.h>
#include "io/KITTIReader.h"

#include <core/LieGaussNewton.h>

using namespace rv;
using namespace glow;

void printMat(const Eigen::MatrixXd& mat) {
  std::cout << std::fixed;
  std::cout << std::setprecision(5);
  for (uint32_t i = 0; i < mat.rows(); ++i) {
    for (uint32_t j = 0; j < mat.cols(); ++j) {
      std::cout << ((i == 0 && j == 0) ? "" : ",") << mat(i, j);
    }
  }
  std::cout << std::endl;
}

void compareMatrices(const Eigen::MatrixXd& expected, const Eigen::MatrixXd& given) {
  ASSERT_EQ(expected.rows(), given.rows());
  ASSERT_EQ(expected.cols(), given.cols());
  for (uint32_t i = 0; i < expected.rows(); ++i) {
    for (uint32_t j = 0; j < expected.cols(); ++j) {
      ASSERT_NEAR(expected(i, j), given(i, j), 0.001f)
          << "Mismatch at (" << i << ", " << j << "): Expected: " << expected(i, j) << ",but got: " << given(i, j);
    }
  }
}

TEST(JacobianTest, testjacobian) {
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
  objective.setData(frame1, frame2);

  Eigen::MatrixXd JtJ(6, 6);
  Eigen::MatrixXd Jtf(6, 1);
  objective.initialize(Eigen::Matrix4d::Identity());
  objective.jacobianProducts(JtJ, Jtf);

  //  printMat(JtJ);
  //  printMat(Jtf);

  Eigen::MatrixXd JtJ_gold(6, 6);
  Eigen::MatrixXd Jtf_gold(6, 1);
  JtJ_gold << 993.91180, -51.22279, -643.58301, -350.30380, 9306.28320, 7171.18750, -51.22279, 821.79529, -543.81512,
      2145.35474, 3262.42944, -2469.49561, -643.58301, -543.81512, 21746.54688, -51255.28906, -27316.54297, -2912.13965,
      -350.30380, 2145.35474, -51255.28906, 2884729.25000, -147322.00000, 71476.58594, 9306.28320, 3262.42944,
      -27316.54297, -147322.00000, 2454717.25000, 107829.93750, 7171.18750, -2469.49561, -2912.13965, 71476.58594,
      107829.93750, 560050.56250;
  Jtf_gold << 137.19766, -50.23804, -117.34109, -1546.24805, -3293.39111, -2373.92749;

  uint32_t inlier = 23562, outlier = 6443, valid = 30005, invalid = 16075;

  compareMatrices(JtJ_gold, JtJ);
  compareMatrices(Jtf_gold, Jtf);

  ASSERT_EQ(inlier, objective.inlier());
  ASSERT_EQ(outlier, objective.outlier());
  ASSERT_EQ(valid, objective.valid());
  ASSERT_EQ(invalid, objective.invalid());
}
