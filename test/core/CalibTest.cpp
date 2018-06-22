#include <gtest/gtest.h>
#include "util/kitti_utils.h"

namespace {

TEST(CalibrationTest, testInitialization) {
  KITTICalibration calib("calib.txt");

  ASSERT_TRUE(calib.exists("Tr"));
  ASSERT_TRUE(calib.exists("P0"));
  ASSERT_TRUE(calib.exists("P1"));
  ASSERT_TRUE(calib.exists("P2"));
  ASSERT_TRUE(calib.exists("P3"));
  ASSERT_FALSE(calib.exists("invalid"));

  Eigen::Matrix4f Tr = calib["Tr"];

  ASSERT_FLOAT_EQ(0.000427680238558, Tr(0, 0));
  ASSERT_FLOAT_EQ(-0.999967248495, Tr(0, 1));
  ASSERT_FLOAT_EQ(-0.00808449168347, Tr(0, 2));
  ASSERT_FLOAT_EQ(-0.0119845992771, Tr(0, 3));

  ASSERT_FLOAT_EQ(-0.0072106265075, Tr(1, 0));
  ASSERT_FLOAT_EQ(0.00808119847165, Tr(1, 1));
  ASSERT_FLOAT_EQ(-0.99994131645, Tr(1, 2));
  ASSERT_FLOAT_EQ(-0.0540398472975, Tr(1, 3));

  ASSERT_FLOAT_EQ(0.99997386459, Tr(2, 0));
  ASSERT_FLOAT_EQ(0.000485948581039, Tr(2, 1));
  ASSERT_FLOAT_EQ(-0.00720693369242, Tr(2, 2));
  ASSERT_FLOAT_EQ(-0.292196864869, Tr(2, 3));

  ASSERT_FLOAT_EQ(0.0f, Tr(3, 0));
  ASSERT_FLOAT_EQ(0.0f, Tr(3, 1));
  ASSERT_FLOAT_EQ(0.0f, Tr(3, 2));
  ASSERT_FLOAT_EQ(1.0f, Tr(3, 3));
}
}
