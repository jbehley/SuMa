#include <gtest/gtest.h>

#include <util/kitti_utils.h>

// from original KITTI devkit.
#include "matrix.h"

using namespace std;

namespace devkit {

// static parameter
// float lengths[] = {5,10,50,100,150,200,250,300,350,400};
float lengths[] = {100, 200, 300, 400, 500, 600, 700, 800};
int32_t num_lengths = 8;

struct errors {
  int32_t first_frame;
  float r_err;
  float t_err;
  float len;
  float speed;
  errors(int32_t first_frame, float r_err, float t_err, float len, float speed)
      : first_frame(first_frame), r_err(r_err), t_err(t_err), len(len), speed(speed) {}
};

vector<Matrix> loadPoses(string file_name) {
  vector<Matrix> poses;
  FILE *fp = fopen(file_name.c_str(), "r");
  if (!fp) return poses;
  while (!feof(fp)) {
    Matrix P = Matrix::eye(4);
    if (fscanf(fp, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", &P.val[0][0], &P.val[0][1], &P.val[0][2],
               &P.val[0][3], &P.val[1][0], &P.val[1][1], &P.val[1][2], &P.val[1][3], &P.val[2][0], &P.val[2][1],
               &P.val[2][2], &P.val[2][3]) == 12) {
      poses.push_back(P);
    }
  }
  fclose(fp);
  return poses;
}

vector<float> trajectoryDistances(vector<Matrix> &poses) {
  vector<float> dist;
  dist.push_back(0);
  for (uint32_t i = 1; i < poses.size(); i++) {
    Matrix P1 = poses[i - 1];
    Matrix P2 = poses[i];
    float dx = P1.val[0][3] - P2.val[0][3];
    float dy = P1.val[1][3] - P2.val[1][3];
    float dz = P1.val[2][3] - P2.val[2][3];
    dist.push_back(dist[i - 1] + sqrt(dx * dx + dy * dy + dz * dz));
  }
  return dist;
}

int32_t lastFrameFromSegmentLength(vector<float> &dist, int32_t first_frame, float len) {
  for (uint32_t i = first_frame; i < dist.size(); i++)
    if (dist[i] > dist[first_frame] + len) return i;
  return -1;
}

inline float rotationError(Matrix &pose_error) {
  float a = pose_error.val[0][0];
  float b = pose_error.val[1][1];
  float c = pose_error.val[2][2];
  float d = 0.5 * (a + b + c - 1.0);
  return acos(max(min(d, 1.0f), -1.0f));
}

inline float translationError(Matrix &pose_error) {
  float dx = pose_error.val[0][3];
  float dy = pose_error.val[1][3];
  float dz = pose_error.val[2][3];
  return sqrt(dx * dx + dy * dy + dz * dz);
}

vector<errors> calcSequenceErrors(vector<Matrix> &poses_gt, vector<Matrix> &poses_result) {
  // error vector
  vector<errors> err;

  // parameters
  int32_t step_size = 10;  // every second

  // pre-compute distances (from ground truth as reference)
  vector<float> dist = trajectoryDistances(poses_gt);

  // for all start positions do
  for (uint32_t first_frame = 0; first_frame < poses_gt.size(); first_frame += step_size) {
    // for all segment lengths do
    for (int32_t i = 0; i < num_lengths; i++) {
      // current length
      float len = lengths[i];

      // compute last frame
      int32_t last_frame = lastFrameFromSegmentLength(dist, first_frame, len);

      // continue, if sequence not long enough
      if (last_frame == -1) continue;

      // compute rotational and translational errors
      Matrix pose_delta_gt = Matrix::inv(poses_gt[first_frame]) * poses_gt[last_frame];
      Matrix pose_delta_result = Matrix::inv(poses_result[first_frame]) * poses_result[last_frame];
      Matrix pose_error = Matrix::inv(pose_delta_result) * pose_delta_gt;
      float r_err = rotationError(pose_error);
      float t_err = translationError(pose_error);

      // compute speed
      float num_frames = (float)(last_frame - first_frame + 1);
      float speed = len / (0.1 * num_frames);

      // write to file
      err.push_back(errors(first_frame, r_err / len, t_err / len, len, speed));
    }
  }

  // return error vector
  return err;
}
}

namespace {

TEST(EvaluationTest, testSequencesError) {
  std::cout << "Checking sequence error." << std::endl;
  vector<Matrix> poses_gt_devkit = devkit::loadPoses("gt_poses.txt");
  vector<Matrix> poses_result_devkit = devkit::loadPoses("result_poses.txt");
  ASSERT_GT(poses_gt_devkit.size(), 0);
  ASSERT_GT(poses_result_devkit.size(), 0);

  vector<Eigen::Matrix4f> poses_gt = KITTI::Odometry::loadPoses("gt_poses.txt");
  vector<Eigen::Matrix4f> poses_result = KITTI::Odometry::loadPoses("result_poses.txt");

  ASSERT_GT(poses_gt.size(), 0);
  ASSERT_GT(poses_result.size(), 0);

  ASSERT_EQ(poses_gt_devkit.size(), poses_gt.size());
  ASSERT_EQ(poses_result_devkit.size(), poses_result.size());

  vector<devkit::errors> err_devkit = devkit::calcSequenceErrors(poses_gt_devkit, poses_result_devkit);
  vector<KITTI::Odometry::errors> err = KITTI::Odometry::calcSequenceErrors(poses_gt, poses_result);

  ASSERT_EQ(err_devkit.size(), err.size());

  for (uint32_t i = 0; i < err.size(); ++i) {
    ASSERT_EQ(err_devkit[i].first_frame, err[i].first_frame);
    ASSERT_NEAR(err_devkit[i].len, err[i].len, 0.1f);
    ASSERT_NEAR(err_devkit[i].r_err, err[i].r_err, 0.1f);
    ASSERT_NEAR(err_devkit[i].t_err, err[i].t_err, 0.1f);
    ASSERT_NEAR(err_devkit[i].speed, err[i].speed, 0.1f);
  }
}
}
