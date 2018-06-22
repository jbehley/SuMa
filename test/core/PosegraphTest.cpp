#include <core/Posegraph.h>

#include <gtest/gtest.h>
#include <gtsam/geometry/Pose3.h>

namespace {

Eigen::Matrix4d pose(float yaw, float pitch, float roll, float x, float y, float z) {
  return gtsam::Pose3(gtsam::Rot3::RzRyRx(yaw, pitch, roll), gtsam::Point3(x, y, z)).matrix();
}

float normalize_angle(float angle) {
  float a = angle;
  while (a < 0) a += 2 * M_PI;
  while (a > 2 * M_PI) a -= 2 * M_PI;
  return a;
}

#define ASSERT_POSE_EQ(YAW, PITCH, ROLL, X, Y, Z, POSE)                                     \
  do {                                                                                      \
    ASSERT_NEAR(normalize_angle(YAW), normalize_angle(POSE.rotation().yaw()), 0.0001f);     \
    ASSERT_NEAR(normalize_angle(PITCH), normalize_angle(POSE.rotation().pitch()), 0.0001f); \
    ASSERT_NEAR(normalize_angle(ROLL), normalize_angle(POSE.rotation().roll()), 0.0001f);   \
    ASSERT_NEAR(X, POSE.translation().x(), 0.0001f);                                        \
    ASSERT_NEAR(Y, POSE.translation().y(), 0.0001f);                                        \
    ASSERT_NEAR(Z, POSE.translation().z(), 0.0001f);                                        \
  } while (0)

TEST(PosegraphTest, testOptimize) {
  Posegraph p_gtsam;

  Posegraph::Matrix6d info_ = 10 * Posegraph::Matrix6d::Identity();

  p_gtsam.addEdge(0, 1, pose(0, 0, 0, 2., 0, 0), info_);
  p_gtsam.addEdge(1, 2, pose(0, 0, 0, 2., 0, 0), info_);
  p_gtsam.addEdge(2, 3, pose(0, 0, M_PI_2, 2., 0, 0), info_);

  p_gtsam.addEdge(3, 4, pose(0, 0, 0, 2., 0, 0), info_);
  p_gtsam.addEdge(4, 5, pose(0, 0, 0, 2., 0, 0), info_);
  p_gtsam.addEdge(5, 6, pose(0, 0, M_PI_2, 2., 0, 0), info_);

  p_gtsam.addEdge(6, 7, pose(0, 0, 0, 2., 0, 0), info_);
  p_gtsam.addEdge(7, 8, pose(0, 0, 0, 2., 0, 0), info_);
  p_gtsam.addEdge(8, 9, pose(0, 0, M_PI_2, 2., 0, 0), info_);

  p_gtsam.addEdge(9, 10, pose(0, 0, 0, 2., 0, 0), info_);
  p_gtsam.addEdge(10, 11, pose(0, 0, 0, 2., 0, 0), info_);
  p_gtsam.addEdge(11, 12, pose(0, 0, 0, 2., 0, 0), info_);

  // loop closure edges.
  p_gtsam.addEdge(11, 0, pose(0, 0, M_PI_2, 2., 0, 0), info_);
  p_gtsam.addEdge(12, 0, pose(0, 0, M_PI_2, 0, 0, 0), info_);
  p_gtsam.addEdge(11, 1, pose(0, 0, M_PI_2, 2., 2, 0), info_);
  p_gtsam.addEdge(12, 1, pose(0, 0, M_PI_2, 0, 2, 0), info_);

  // initial values.
  p_gtsam.setInitial(0, pose(0, 0, 0, 0, 0, 0));

  p_gtsam.setInitial(1, pose(0, 0, 0, 2.1, 0.1, 0));
  p_gtsam.setInitial(2, pose(0, 0, 0, 3.9, 0.1, 0));
  p_gtsam.setInitial(3, pose(0, 0, M_PI_2, 6.1, -0.1, 0));

  p_gtsam.setInitial(4, pose(0, 0, M_PI_2, 6.1, 2.1, 0));
  p_gtsam.setInitial(5, pose(0, 0, M_PI_2, 6.1, 3.9, 0));
  p_gtsam.setInitial(6, pose(0, 0, M_PI, 6.1, 6.1, 0));

  p_gtsam.setInitial(7, pose(0, 0, M_PI, 4.0, 6.1, 0));
  p_gtsam.setInitial(8, pose(0, 0, M_PI, 2.0, 6.1, 0));
  p_gtsam.setInitial(9, pose(0, 0, 3 * M_PI_2, 0.0, 6.1, 0));

  p_gtsam.setInitial(10, pose(0, 0, 3 * M_PI_2, 0.0, 3.95, 0));
  p_gtsam.setInitial(11, pose(0, 0, 3 * M_PI_2, 0.0, 2.0, 0));
  p_gtsam.setInitial(12, pose(0, 0, 3 * M_PI_2, 0.0, 0.0, 0));

  // p_gtsam.graph().print("Factor graph");
  // p_gtsam.initial().print("Initial values:");

  p_gtsam.optimize(10);

  // p_gtsam.result().print("Result values:");

  const gtsam::Values& result = p_gtsam.result();

  ASSERT_POSE_EQ(0, 0, 0, 0, 0, 0, result.at<gtsam::Pose3>(0));

  ASSERT_POSE_EQ(0, 0, 0, 2, 0, 0, result.at<gtsam::Pose3>(1));
  ASSERT_POSE_EQ(0, 0, 0, 4, 0, 0, result.at<gtsam::Pose3>(2));
  ASSERT_POSE_EQ(M_PI_2, 0, 0, 6, 0, 0, result.at<gtsam::Pose3>(3));

  ASSERT_POSE_EQ(M_PI_2, 0, 0, 6, 2, 0, result.at<gtsam::Pose3>(4));
  ASSERT_POSE_EQ(M_PI_2, 0, 0, 6, 4, 0, result.at<gtsam::Pose3>(5));
  ASSERT_POSE_EQ(M_PI, 0, 0, 6, 6, 0, result.at<gtsam::Pose3>(6));

  ASSERT_POSE_EQ(M_PI, 0, 0, 4, 6, 0, result.at<gtsam::Pose3>(7));
  ASSERT_POSE_EQ(M_PI, 0, 0, 2, 6, 0, result.at<gtsam::Pose3>(8));
  ASSERT_POSE_EQ(3 * M_PI_2, 0, 0, 0, 6, 0, result.at<gtsam::Pose3>(9));

  ASSERT_POSE_EQ(3 * M_PI_2, 0, 0, 0, 4, 0, result.at<gtsam::Pose3>(10));
  ASSERT_POSE_EQ(3 * M_PI_2, 0, 0, 0, 2, 0, result.at<gtsam::Pose3>(11));
  ASSERT_POSE_EQ(3 * M_PI_2, 0, 0, 0, 0, 0, result.at<gtsam::Pose3>(12));
}
}
