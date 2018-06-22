#include <list>
#include <vector>

#include <gtest/gtest.h>
#include <rv/FileUtil.h>
#include <rv/Math.h>
#include <rv/geometry.h>

using namespace rv;

namespace {

class MathTest : public ::testing::Test {
 protected:
  virtual void SetUp() { EPSILON = 0.0001; }

  Eigen::VectorXf generateVector() {
    Eigen::VectorXf vector(10, 1);

    vector[0] = 0.8147;
    vector[1] = 0.9058;
    vector[2] = 0.1270;
    vector[3] = 0.9134;
    vector[4] = 0.6324;
    vector[5] = 0.0975;
    vector[6] = 0.2785;
    vector[7] = 0.5469;
    vector[8] = 0.9575;
    vector[9] = 0.9649;

    //  0.8147
    //  0.9058
    //  0.1270
    //  0.9134
    //  0.6324
    //  0.0975
    //  0.2785
    //  0.5469
    //  0.9575
    //  0.9649

    // MatLab: V = [0.8147; 0.9058; 0.1270; 0.9134; 0.6324; 0.0975; 0.2785; 0.5469; 0.9575; 0.9649];

    return vector;
  }

  Eigen::MatrixXf generateMatrix() {
    Eigen::MatrixXf matrix(10, 3);

    matrix(0, 0) = 0.0211;
    matrix(0, 1) = 0.4993;
    matrix(0, 2) = 0.5529;

    matrix(1, 0) = 0.5123;
    matrix(1, 1) = 0.8135;
    matrix(1, 2) = 0.0185;

    matrix(2, 0) = 0.4076;
    matrix(2, 1) = 0.2348;
    matrix(2, 2) = 0.5078;

    matrix(3, 0) = 0.2198;
    matrix(3, 1) = 0.0223;
    matrix(3, 2) = 0.7136;

    matrix(4, 0) = 0.2305;
    matrix(4, 1) = 0.3011;
    matrix(4, 2) = 0.6977;

    matrix(5, 0) = 0.4385;
    matrix(5, 1) = 0.2981;
    matrix(5, 2) = 0.8935;

    matrix(6, 0) = 0.9199;
    matrix(6, 1) = 0.8259;
    matrix(6, 2) = 0.7012;

    matrix(7, 0) = 0.3190;
    matrix(7, 1) = 0.1364;
    matrix(7, 2) = 0.9373;

    matrix(8, 0) = 0.2843;
    matrix(8, 1) = 0.8146;
    matrix(8, 2) = 0.0109;

    matrix(9, 0) = 0.3638;
    matrix(9, 1) = 0.8611;
    matrix(9, 2) = 0.8197;

    //  0.0211 0.4993 0.5529
    //  0.5123 0.8135 0.0185
    //  0.4076 0.2348 0.5078
    //  0.2198 0.0223 0.7136
    //  0.2305 0.3011 0.6977
    //  0.4385 0.2981 0.8935
    //  0.9199 0.8259 0.7012
    //  0.3190 0.1364 0.9373
    //  0.2843 0.8146 0.0109
    //  0.3638 0.8611 0.8197
    // Matlab: M = [0.0211 0.4993 0.5529; 0.5123 0.8135 0.0185; 0.4076 0.2348 0.5078; 0.2198 0.0223 0.7136; 0.2305
    // 0.3011 0.6977;  0.4385 0.2981 0.8935; 0.9199 0.8259 0.7012;  0.3190 0.1364 0.9373; 0.2843 0.8146 0.0109; 0.3638
    // 0.8611 0.8197]

    return matrix;
  }

  double EPSILON;
};

TEST_F(MathTest, testUtilityFunctions) {
  /** testing normalize angle **/
  std::vector<float> angles;
  const float pipi = 2.0f * Math::PI;
  angles.push_back(1.3337f * pipi);
  angles.push_back(0.4f * pipi);
  angles.push_back(-0.54f * pipi);

  for (uint32_t i = 0; i < angles.size(); ++i) {
    float angle = Math::normalizeAngle(angles[i]);
    ASSERT_TRUE((angle > 0.0f));
    ASSERT_TRUE((angle < (pipi + 0.0001)));
  }
}

TEST_F(MathTest, testStatisticalOperations) {
  std::vector<double> vals1;
  vals1.push_back(3.0);
  vals1.push_back(1.0);
  vals1.push_back(-4.0);
  vals1.push_back(2.0);

  double vals2[4];
  vals2[0] = 3.0;
  vals2[1] = 1.0;
  vals2[2] = -4.0;
  vals2[3] = 2.0;

  /** mean of some values. **/
  double mean1 = Math::mean(vals1.begin(), vals1.end());
  ASSERT_NEAR(0.5, mean1, EPSILON);

  double mean2 = Math::mean(vals2, vals2 + 4);
  ASSERT_NEAR(0.5, mean2, EPSILON);

  /** sigma of some values. **/
  double sigma1 = Math::sigma(vals1.begin(), vals1.end());
  ASSERT_NEAR(3.1091, sigma1, EPSILON);
  double sigma2 = Math::sigma(vals2, vals2 + 4);
  ASSERT_NEAR(3.1091, sigma2, EPSILON);
  double sigma3 = Math::sigma(vals1.begin(), vals1.end(), 1);
  ASSERT_NEAR(2.6926, sigma3, EPSILON);
}

TEST_F(MathTest, testCovariance) {
  Eigen::MatrixXf T = generateMatrix();

  //
  //  ASSERT_NEAR(0.0558, result(0, 0), EPSILON);
  //  ASSERT_NEAR(0.0322, result(0, 1), EPSILON);
  //  ASSERT_NEAR(0.0008, result(0, 2), EPSILON);
  //  ASSERT_NEAR(0.0322, result(1, 0), EPSILON);
  //  ASSERT_NEAR(0.1046, result(1, 1), EPSILON);
  //  ASSERT_NEAR(-0.0544, result(1, 2), EPSILON);
  //  ASSERT_NEAR(0.0008, result(2, 0), EPSILON);
  //  ASSERT_NEAR(-0.0544, result(2, 1), EPSILON);
  //  ASSERT_NEAR(0.1083, result(2, 2), EPSILON);
  //
  //  /** covariance of points. **/
  std::vector<Point3f> pts;
  for (uint32_t i = 0; i < T.rows(); ++i) pts.push_back(Point3f(T(i, 0), T(i, 1), T(i, 2)));

  Eigen::Matrix3f result2 = Math::cov3(pts.begin(), pts.end());

  ASSERT_NEAR(0.05024525f, result2(0, 0), EPSILON);
  ASSERT_NEAR(0.02894307f, result2(0, 1), EPSILON);
  ASSERT_NEAR(0.00074500f, result2(0, 2), EPSILON);

  ASSERT_NEAR(0.02894307f, result2(1, 0), EPSILON);
  ASSERT_NEAR(0.09411970f, result2(1, 1), EPSILON);
  ASSERT_NEAR(-0.04892648f, result2(1, 2), EPSILON);

  ASSERT_NEAR(0.00074500f, result2(2, 0), EPSILON);
  ASSERT_NEAR(-0.04892648f, result2(2, 1), EPSILON);
  ASSERT_NEAR(0.09746157f, result2(2, 2), EPSILON);
}

TEST_F(MathTest, testMin) {
  //  legacy_fkie::Vector V = generateVector();
  Eigen::MatrixXf T = generateMatrix();
  //  // testing for vector
  //  double result1 = Math::min(V);
  //  ASSERT_NEAR(result1, 0.0975, EPSILON);
  //  // testing for matrix
  Eigen::MatrixXf result2 = Math::min(T);
  ASSERT_EQ(1, (int)result2.rows());

  ASSERT_EQ(T.cols(), result2.cols());
  ASSERT_NEAR(0.0211, result2(0, 0), EPSILON);
  ASSERT_NEAR(0.0223, result2(0, 1), EPSILON);
  ASSERT_NEAR(0.0109, result2(0, 2), EPSILON);
}

TEST_F(MathTest, testMax) {
  //  legacy_fkie::Vector V = generateVector();
  Eigen::MatrixXf T = generateMatrix();

  // testing for vector
  //  double result1 = Math::max(V);
  //  ASSERT_NEAR(0.9649, result1, EPSILON);
  // testing for matrix
  Eigen::MatrixXf result2 = Math::max(T);
  ASSERT_EQ(1, (int)result2.rows());
  ASSERT_EQ(T.cols(), result2.cols());
  ASSERT_NEAR(0.9199, result2(0, 0), EPSILON);
  ASSERT_NEAR(0.8611, result2(0, 1), EPSILON);
  ASSERT_NEAR(0.9373, result2(0, 2), EPSILON);

  std::vector<float> vec;
  vec.push_back(2.0f);
  vec.push_back(-23.0f);
  vec.push_back(12.32f);
  vec.push_back(5.03f);

  ASSERT_EQ(2, Math::argmax(vec));
}

// TEST_F(MathTest, testColumn)
//{
//  legacy_fkie::Matrix T = generateMatrix();
//
//  legacy_fkie::Vector c0 = T.column(0);
//  legacy_fkie::Vector c1 = T.column(1);
//  legacy_fkie::Vector c2 = T.column(2);
//
//  // first column
//  ASSERT_EQ(10, (int ) c0.size());
//
//  ASSERT_NEAR(c0[0], 0.0211, EPSILON);
//  ASSERT_NEAR(c0[1], 0.5123, EPSILON);
//  ASSERT_NEAR(c0[2], 0.4076, EPSILON);
//  ASSERT_NEAR(c0[3], 0.2198, EPSILON);
//  ASSERT_NEAR(c0[4], 0.2305, EPSILON);
//  ASSERT_NEAR(c0[5], 0.4385, EPSILON);
//  ASSERT_NEAR(c0[6], 0.9199, EPSILON);
//  ASSERT_NEAR(c0[7], 0.3190, EPSILON);
//  ASSERT_NEAR(c0[8], 0.2843, EPSILON);
//  ASSERT_NEAR(c0[9], 0.3638, EPSILON);
//
//  // second column
//  ASSERT_EQ(10, (int ) c1.size());
//  ASSERT_NEAR(c1[0], 0.4993, EPSILON);
//  ASSERT_NEAR(c1[1], 0.8135, EPSILON);
//  ASSERT_NEAR(c1[2], 0.2348, EPSILON);
//  ASSERT_NEAR(c1[3], 0.0223, EPSILON);
//  ASSERT_NEAR(c1[4], 0.3011, EPSILON);
//  ASSERT_NEAR(c1[5], 0.2981, EPSILON);
//  ASSERT_NEAR(c1[6], 0.8259, EPSILON);
//  ASSERT_NEAR(c1[7], 0.1364, EPSILON);
//  ASSERT_NEAR(c1[8], 0.8146, EPSILON);
//  ASSERT_NEAR(c1[9], 0.8611, EPSILON);
//
//  // third column
//  ASSERT_EQ(10, (int ) c2.size());
//  ASSERT_NEAR(c2[0], 0.5529, EPSILON);
//  ASSERT_NEAR(c2[1], 0.0185, EPSILON);
//  ASSERT_NEAR(c2[2], 0.5078, EPSILON);
//  ASSERT_NEAR(c2[3], 0.7136, EPSILON);
//  ASSERT_NEAR(c2[4], 0.6977, EPSILON);
//  ASSERT_NEAR(c2[5], 0.8935, EPSILON);
//  ASSERT_NEAR(c2[6], 0.7012, EPSILON);
//  ASSERT_NEAR(c2[7], 0.9373, EPSILON);
//  ASSERT_NEAR(c2[8], 0.0109, EPSILON);
//  ASSERT_NEAR(c2[9], 0.8197, EPSILON);
//
//  // now set an column
//  legacy_fkie::Vector v = generateVector();
//
//  T.setColumn(1, v);
//  c0 = T.column(0);
//  c1 = T.column(1);
//  c2 = T.column(2);
//
//  ASSERT_EQ(10, (int ) c0.size());
//  ASSERT_NEAR(c0[0], 0.0211, EPSILON);
//  ASSERT_NEAR(c0[1], 0.5123, EPSILON);
//  ASSERT_NEAR(c0[2], 0.4076, EPSILON);
//  ASSERT_NEAR(c0[3], 0.2198, EPSILON);
//  ASSERT_NEAR(c0[4], 0.2305, EPSILON);
//  ASSERT_NEAR(c0[5], 0.4385, EPSILON);
//  ASSERT_NEAR(c0[6], 0.9199, EPSILON);
//  ASSERT_NEAR(c0[7], 0.3190, EPSILON);
//  ASSERT_NEAR(c0[8], 0.2843, EPSILON);
//  ASSERT_NEAR(c0[9], 0.3638, EPSILON);
//
//  // second column
//  ASSERT_EQ(10, (int ) c1.size());
//
//  ASSERT_NEAR(c1[0], 0.8147, EPSILON);
//  ASSERT_NEAR(c1[1], 0.9058, EPSILON);
//  ASSERT_NEAR(c1[2], 0.1270, EPSILON);
//  ASSERT_NEAR(c1[3], 0.9134, EPSILON);
//  ASSERT_NEAR(c1[4], 0.6324, EPSILON);
//  ASSERT_NEAR(c1[5], 0.0975, EPSILON);
//  ASSERT_NEAR(c1[6], 0.2785, EPSILON);
//  ASSERT_NEAR(c1[7], 0.5469, EPSILON);
//  ASSERT_NEAR(c1[8], 0.9575, EPSILON);
//  ASSERT_NEAR(c1[9], 0.9649, EPSILON);
//
//  // third column
//  ASSERT_EQ(10, (int ) c1.size());
//
//  ASSERT_NEAR(c2[0], 0.5529, EPSILON);
//  ASSERT_NEAR(c2[1], 0.0185, EPSILON);
//  ASSERT_NEAR(c2[2], 0.5078, EPSILON);
//  ASSERT_NEAR(c2[3], 0.7136, EPSILON);
//  ASSERT_NEAR(c2[4], 0.6977, EPSILON);
//  ASSERT_NEAR(c2[5], 0.8935, EPSILON);
//  ASSERT_NEAR(c2[6], 0.7012, EPSILON);
//  ASSERT_NEAR(c2[7], 0.9373, EPSILON);
//  ASSERT_NEAR(c2[8], 0.0109, EPSILON);
//  ASSERT_NEAR(c2[9], 0.8197, EPSILON);
//}

TEST_F(MathTest, testSort) {
  Eigen::VectorXf v = generateVector();
  Eigen::MatrixXf m(v.size(), 1);
  m.col(0) = v;
  Eigen::MatrixXf B(v.size(), 1);
  Eigen::MatrixXf IX(v.size(), 1);

  Math::sort(m, B, IX);
  Eigen::VectorXf c1 = B.col(0);

  ASSERT_EQ(10, (int)c1.size());
  ASSERT_NEAR(c1[0], 0.0975, EPSILON);
  ASSERT_NEAR(c1[1], 0.1270, EPSILON);
  ASSERT_NEAR(c1[2], 0.2785, EPSILON);
  ASSERT_NEAR(c1[3], 0.5469, EPSILON);
  ASSERT_NEAR(c1[4], 0.6324, EPSILON);
  ASSERT_NEAR(c1[5], 0.8147, EPSILON);
  ASSERT_NEAR(c1[6], 0.9058, EPSILON);
  ASSERT_NEAR(c1[7], 0.9134, EPSILON);
  ASSERT_NEAR(c1[8], 0.9575, EPSILON);
  ASSERT_NEAR(c1[9], 0.9649, EPSILON);

  Eigen::VectorXf c2 = IX.col(0);

  ASSERT_EQ(10, (int)c2.size());
  ASSERT_NEAR(c2[0], 5, EPSILON);
  ASSERT_NEAR(c2[1], 2, EPSILON);
  ASSERT_NEAR(c2[2], 6, EPSILON);
  ASSERT_NEAR(c2[3], 7, EPSILON);
  ASSERT_NEAR(c2[4], 4, EPSILON);
  ASSERT_NEAR(c2[5], 0, EPSILON);
  ASSERT_NEAR(c2[6], 1, EPSILON);
  ASSERT_NEAR(c2[7], 3, EPSILON);
  ASSERT_NEAR(c2[8], 8, EPSILON);
  ASSERT_NEAR(c2[9], 9, EPSILON);
}

// TEST_F(MathTest, testProd)
//{
//  legacy_fkie::Matrix M = generateMatrix();
//  // scale it a little bit
//  M *= 2;
//
//  // prod along the first dimension (column product)
//  legacy_fkie::Vector result = Math::prod(M);
//  ASSERT_NEAR(result[0], 0.0030, EPSILON);
//  ASSERT_NEAR(result[1], 0.0154, EPSILON);
//  ASSERT_NEAR(result[2], 0.0139, EPSILON);
//  // prod along the second dimension (row product)
//  result = Math::prod(M, 1);
//  ASSERT_NEAR(result[0], 0.0466, EPSILON);
//  ASSERT_NEAR(result[1], 0.0617, EPSILON);
//  ASSERT_NEAR(result[2], 0.3888, EPSILON);
//  ASSERT_NEAR(result[3], 0.0280, EPSILON);
//  ASSERT_NEAR(result[4], 0.3874, EPSILON);
//  ASSERT_NEAR(result[5], 0.9344, EPSILON);
//  ASSERT_NEAR(result[6], 4.2619, EPSILON);
//  ASSERT_NEAR(result[7], 0.3263, EPSILON);
//  ASSERT_NEAR(result[8], 0.0202, EPSILON);
//}

// TEST_F(MathTest, testSqrt)
//{
//  legacy_fkie::Matrix T = generateMatrix();
//  legacy_fkie::Matrix result = Math::cov(T); // covariance is positve-definite and symmetric.
//
//  legacy_fkie::Matrix G = Math::sqrt(result);
//  ASSERT_NEAR(0.227602, G(0, 0), EPSILON);
//  ASSERT_NEAR(0.062363, G(0, 1), EPSILON);
//  ASSERT_NEAR(0.011679, G(0, 2), EPSILON);
//
//  ASSERT_NEAR(0.062363, G(1, 0), EPSILON);
//  ASSERT_NEAR(0.304676, G(1, 1), EPSILON);
//  ASSERT_NEAR(-0.088661, G(1, 2), EPSILON);
//
//  ASSERT_NEAR(0.011679, G(2, 0), EPSILON);
//  ASSERT_NEAR(-0.088661, G(2, 1), EPSILON);
//  ASSERT_NEAR(0.316691, G(2, 2), EPSILON);
//
//  //  assertEqualMatrices(result, G*G);
//}

// TEST_F(MathTest, testRandperm)
//{
//  double exp[10];
//  for (uint32_t i = 1; i <= 10; ++i)
//    exp[i - 1] = i;
//
//  legacy_fkie::Vector v = Math::randperm(10);
//  assertAlmostEqualLists(exp, exp + 10, v.ptr(), v.ptr() + 10);
//}

TEST_F(MathTest, testSum) {
  std::vector<float> vec;

  vec.push_back(0.46726);
  vec.push_back(-0.37542);
  vec.push_back(0.96938);
  vec.push_back(0.94173);
  vec.push_back(-0.71018);
  vec.push_back(-1.81132);
  vec.push_back(-0.36521);
  vec.push_back(-0.46832);
  vec.push_back(-0.20275);
  vec.push_back(0.21010);

  ASSERT_NEAR(-1.3447, Math::sum(vec.begin(), vec.end()), EPSILON);

  std::list<float> list;

  list.push_back(0.46726);
  list.push_back(-0.37542);
  list.push_back(0.96938);
  list.push_back(0.94173);
  list.push_back(-0.71018);
  list.push_back(-1.81132);
  list.push_back(-0.36521);
  list.push_back(-0.46832);

  ASSERT_NEAR(-1.3521, Math::sum(list.begin(), list.end()), EPSILON);
}

TEST_F(MathTest, testMatrixDump) {
  const uint32_t M = 2;
  const uint32_t N = 3;
  /** storing a matrix, and reading it again. **/
  Eigen::MatrixXf mat(M, N);
  for (uint32_t i = 0; i < mat.rows(); ++i) {
    for (uint32_t j = 0; j < mat.cols(); ++j) mat(i, j) = j + i * N;
  }

  //  std::string filename = "test.mat";
  //  Math::save(filename, "mat", mat);
  //  ASSERT_TRUE(legacy_fkie::FileUtil::exists(filename));
  //
  //  Eigen::MatrixXf mat2;
  //  ASSERT_TRUE(Math::load(filename, mat2));
  //  ASSERT_EQ(M, mat2.rows());
  //  ASSERT_EQ(N, mat2.cols());
  //  for (uint32_t i = 0; i < mat2.rows(); ++i)
  //  {
  //    for (uint32_t j = 0; j < mat2.cols(); ++j)
  //    {
  //      ASSERT_NEAR(j + i * N, mat2(i, j), EPSILON);
  //    }
  //  }
}
}
