#include <gtest/gtest.h>

#include <core/lie_algebra.h>

namespace {

TEST(LieTest, testLogExpSE3) {
  // simple sanity test, x = exp(log(x)) and x = log(exp(x))

  Eigen::VectorXd x(6);
  x << -0.7, 0.1, 0.1, 0.1, -0.5, 0.33;

  Eigen::VectorXd x_logexp = SE3::log(SE3::exp(x));

  for (uint32_t i = 0; i < 6; ++i) {
    ASSERT_NEAR(x[i], x_logexp[i], 0.0001);
  }
}
}
