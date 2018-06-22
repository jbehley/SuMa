#include <gtest/gtest.h>
#include <rv/transform.h>
#include <eigen3/Eigen/Dense>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
using namespace rv;

namespace
{

#define ASSERT_EQUAL_MATRICIES(a, b) \
  do \
  { \
    for (uint32_t i = 0; i < 4; ++i) \
      for (uint32_t j = 0; j < 4; ++j) \
        ASSERT_NEAR(a(i,j), b(i,j), 0.0001); \
  } while(0)

template<class T>
std::vector<T> str2vec(const std::string& str)
{
  std::vector<T> vec;
  boost::tokenizer<boost::escaped_list_separator<char> > tok(str.substr(1, str.size() - 2));
  for (boost::tokenizer<boost::escaped_list_separator<char> >::const_iterator it = tok.begin(); it != tok.end(); ++it)
  {
    vec.push_back(boost::lexical_cast<float>(*it));
  }
  return vec;
}

TEST(TransformTest, testInitialization)
{
  Eigen::Matrix4f Id = Eigen::Matrix4f::Identity();
  Eigen::Matrix4f m;
  m << 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16;
  Eigen::Matrix4f minv;
  minv << -1, -2, -3, -4, -5, -6, -7, -8, -9, -10, -11, -12, -13, -14, -15, -16;

  Transform T;
  ASSERT_EQUAL_MATRICIES(Id, T.GetMatrix());
  ASSERT_EQUAL_MATRICIES(Id, T.GetInverseMatrix());

  Transform T1(m, minv);
  ASSERT_EQUAL_MATRICIES(m, T1.GetMatrix());
  ASSERT_EQUAL_MATRICIES(minv, T1.GetInverseMatrix());

  // column-major output:
  std::vector<float> values = str2vec<float>("[1,5,9,13,2,6,10,14,3,7,11,15,4,8,12,16]");
  const float* arr = T1;
  for (uint32_t i = 0; i < 16; ++i)
    ASSERT_NEAR(values[i], arr[i], 0.0001);

  Eigen::Matrix4f gold;
  Eigen::Matrix4f result;

  Transform rx = RotateX(45);
  gold << 1, 0, 0, 0, 0, 0.7071, -0.7071, 0, 0, 0.7071, 0.7071, 0, 0, 0, 0, 1;
  ASSERT_EQUAL_MATRICIES(gold, rx.GetMatrix());
  result = rx.GetMatrix() * rx.GetInverseMatrix();
  ASSERT_EQUAL_MATRICIES(Id, result);

  Transform ry = RotateY(45);
  gold << 0.7071, 0, 0.7071, 0, 0, 1, 0, 0, -0.7071, 0, 0.7071, 0, 0, 0, 0, 1;
  ASSERT_EQUAL_MATRICIES(gold, ry.GetMatrix());
  result = ry.GetMatrix() * ry.GetInverseMatrix();
  ASSERT_EQUAL_MATRICIES(Id, result);

  Transform rz = RotateZ(45);
  gold << 0.7071, -0.7071, 0, 0, 0.7071, 0.7071, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1;
  ASSERT_EQUAL_MATRICIES(gold, rz.GetMatrix());
  result = rz.GetMatrix() * rz.GetInverseMatrix();
  ASSERT_EQUAL_MATRICIES(Id, result);

  // Rotate(45, 1.,1.,0.)
  // 0.8536,0.1464,0.5,0,0.1464,0.8536,-0.5,0,-0.5,0.5,0.7071,0,0,0,0,1
  Transform r = Rotate(45, Vector3f(1., 1., 0.));
  gold << 0.8536, 0.1464, 0.5, 0, 0.1464, 0.8536, -0.5, 0, -0.5, 0.5, 0.7071, 0, 0, 0, 0, 1;
  ASSERT_EQUAL_MATRICIES(gold, r.GetMatrix());
  result = r.GetMatrix() * r.GetInverseMatrix();
  ASSERT_EQUAL_MATRICIES(Id, result);

  // Translate(-12,-3.2,1.44)
  // 1,0,0,-12., 0,1,0,-3.2, 0,0,1,1.44, 0,0,0,1
  Transform translate = Translate(Vector3f(-12, -3.2, 1.44));
  gold << 1, 0, 0, -12., 0, 1, 0, -3.2, 0, 0, 1, 1.44, 0, 0, 0, 1;
  ASSERT_EQUAL_MATRICIES(gold, translate.GetMatrix());
  result = translate.GetMatrix() * translate.GetInverseMatrix();
  ASSERT_EQUAL_MATRICIES(Id, result);

  // Scale(1.,2.,3.)
  // 1,0,0,0, 0,2,0,0, 0,0,3,0, 0,0,0,1

  Transform T2(0.1, 0.04, 0.03, Vector3f(10., 20., 30.), 0);
  gold << 0.9942, -0.09859, 0.04277, 10, 0.09975, 0.9947, -0.02586, 20, -0.03999, 0.02997, 0.9988, 30, 0, 0, 0, 1;
  ASSERT_EQUAL_MATRICIES(gold, T2.GetMatrix());
  result = translate.GetMatrix() * translate.GetInverseMatrix();
  ASSERT_EQUAL_MATRICIES(Id, result);

  Transform Rdummy = Rotate(0.123f, Vector3f(1., 1., 0.));
  Eigen::Matrix3f R;
  for (uint32_t i = 0; i < 3; ++i)
    for (uint32_t j = 0; j < 3; ++j)
      R(i, j) = Rdummy.GetMatrix()(i, j);

  Vector3f t(-1., -2., 3.);
  Transform T15(R, t);

  ASSERT_NEAR(-1., T15.GetMatrix()(0,3), 0.0001);
  ASSERT_NEAR(-2., T15.GetMatrix()(1,3), 0.0001);
  ASSERT_NEAR(3., T15.GetMatrix()(2,3), 0.0001);

  result = T15.GetMatrix() * T15.GetInverseMatrix();
  ASSERT_EQUAL_MATRICIES(Id, result);
}

TEST(TransformTest, testOperations)
{
  Point3f p(1, 2, 3);
  Normal3f n(4, 5, 6);
  Vector3f v(-7, 8, -9);

  Eigen::Matrix4f Id = Eigen::Matrix4f::Identity();
  Eigen::Matrix4f m;
  m << 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16;
  Eigen::Matrix4f minv;
  minv << -1, -2, -3, -4, -5, -6, -7, -8, -9, -10, -11, -12, -13, -14, -15, -16;
  Eigen::Matrix4f m2;
  m2 << 0, 3, 2, 2, 1, 2, 3, 1, 2, 3, 2, 4, 1, 0, 0, 1;
  Eigen::Matrix4f m2inv;
  m2inv << -1, 2, 2, 2, 1, 2, 3, 1, 2, 3, 2, 4, 1, 0, 0, 1;

  Transform T;
  Transform T1(m, minv);

  Point3f tp = T1(p);  // [ 0.17647059, 0.45098039, 0.7254902, 1.0 ]
  ASSERT_NEAR( 0.17647059, tp.x(), 0.0001);
  ASSERT_NEAR( 0.4509803, tp.y(), 0.0001);
  ASSERT_NEAR( 0.7254902, tp.z(), 0.0001);
  ASSERT_NEAR( 1., tp.vec[3], 0.0001);

  Normal3f tn = T1(n);  // [ -83., -98., -113.]
  ASSERT_NEAR( -83., tn.x(), 0.0001);
  ASSERT_NEAR( -98., tn.y(), 0.0001);
  ASSERT_NEAR( -113, tn.z(), 0.0001);
  ASSERT_NEAR( 0.0, tn.vec[3], 0.0001);

  Vector3f tv = T1(v); //  [ -18. -50. -82. ]
  ASSERT_NEAR( -18., tv.x(), 0.0001);
  ASSERT_NEAR( -50., tv.y(), 0.0001);
  ASSERT_NEAR( -82., tv.z(), 0.0001);
  ASSERT_NEAR( 0.0, tv.vec[3], 0.0001);

  Transform T2(m2, m2inv);

  Transform T3 = T2 * T1;
  Eigen::Matrix4f res;
  res << 59, 66, 73, 80, 51, 58, 65, 72, 87, 98, 109, 120, 14, 16, 18, 20;
  ASSERT_EQUAL_MATRICIES(res, T3.GetMatrix());
//  res = T3.GetMatrix() * T3.GetInverseMatrix();
//  ASSERT_EQUAL_MATRICIES(Id, res);

  Transform T4 = RotateX(19) * RotateY(20);
  res = T4.GetMatrix() * T4.GetInverseMatrix();
  ASSERT_EQUAL_MATRICIES(Id, res);
}

}

