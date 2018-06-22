#include <gtest/gtest.h>

#include <core/ImagePyramidGenerator.h>
#include <algorithm>

using namespace glow;

namespace {

TEST(PyramidTest, testSquareImage) {
  uint32_t w = 40, h = 40;  // 20, 10.

  std::vector<vec2> img_coords;

  for (uint32_t i = 0; i < w; ++i) {
    for (uint32_t j = 0; j < h; ++j) {
      img_coords.push_back(vec2(i, j));
    }
  }

//  for (uint32_t i = 0; i < 5; ++i) {
//    for (uint32_t j = 0; j < 5; ++j) {
//      std::cout << "(" << img_coords[i * h + j].x << ", " << img_coords[i * h + j].y << "), ";
//    }
//    std::cout << " ..." << std::endl;
//  }

  std::vector<bool> contained(w * h);

  std::vector<vec2> dims;

  ImagePyramidGenerator::makePyramid(3, w, h, img_coords, dims);

  ASSERT_EQ(3, dims.size());
  ASSERT_EQ(10, dims[0].x);
  ASSERT_EQ(20, dims[1].x);
  ASSERT_EQ(40, dims[2].x);
  ASSERT_EQ(40, dims[0].y);
  ASSERT_EQ(40, dims[1].y);
  ASSERT_EQ(40, dims[2].y);

//  for (uint32_t i = 0; i < 5; ++i) {
//    for (uint32_t j = 0; j < 5; ++j) {
//      std::cout << "(" << img_coords[i * h + j].x << ", " << img_coords[i * h + j].y << "), ";
//    }
//    std::cout << " ..." << std::endl;
//  }

  // check that all values are in range && check for duplicates.

  for (uint32_t i = 0; i < dims.size(); ++i) {
    uint32_t size = dims[i].x * dims[i].y;
    std::fill(contained.begin(), contained.end(), false);
    for (uint32_t j = 0; j < size; ++j) {
      uint32_t idx = img_coords[j].x * h + img_coords[j].y;
      ASSERT_FALSE(contained[idx]) << img_coords[j].x << " " << img_coords[j].y;  // duplicates?
      contained[idx] = true;
    }

//    std::cout << "result of level " << i << ":" << std::endl;
//    for (uint32_t x = 0; x < w; ++x) {
//      for (uint32_t y = 0; y < h; ++y) {
//        std::cout << (contained[x * h + y] ? "X" : " ");
//      }
//      std::cout << std::endl;
//    }
  }

  // finally everything should be in the last level.
  for (uint32_t i = 0; i < contained.size(); ++i) {
    ASSERT_TRUE(contained[i]);
  }
}

TEST(PyramidTest, testRectangularImage) {
  uint32_t w = 40, h = 80;  // 20, 10.

  std::vector<vec2> img_coords;

  for (uint32_t i = 0; i < w; ++i) {
    for (uint32_t j = 0; j < h; ++j) {
      img_coords.push_back(vec2(i, j));
    }
  }

  std::vector<bool> contained(w * h);

  std::vector<vec2> dims;

  ImagePyramidGenerator::makePyramid(3, w, h, img_coords, dims);

  ASSERT_EQ(3, dims.size());
  ASSERT_EQ(10, dims[0].x);
  ASSERT_EQ(20, dims[1].x);
  ASSERT_EQ(40, dims[2].x);
  ASSERT_EQ(80, dims[0].y);
  ASSERT_EQ(80, dims[1].y);
  ASSERT_EQ(80, dims[2].y);

  // check that all values are in range && check for duplicates.

  for (uint32_t i = 0; i < dims.size(); ++i) {
    uint32_t size = dims[i].x * dims[i].y;
    std::fill(contained.begin(), contained.end(), false);
    for (uint32_t j = 0; j < size; ++j) {
      uint32_t idx = img_coords[j].x * h + img_coords[j].y;
      ASSERT_FALSE(contained[idx]) << img_coords[j].x << " " << img_coords[j].y;  // duplicates?
      contained[idx] = true;
    }
  }

  // finally everything should be in the last level.
  for (uint32_t i = 0; i < contained.size(); ++i) {
    ASSERT_TRUE(contained[i]);
  }
}

TEST(PyramidTest, testUnevenImage) {
  uint32_t w = 71, h = 32;  // 20, 10.

  std::vector<vec2> img_coords;

  for (uint32_t i = 0; i < w; ++i) {
    for (uint32_t j = 0; j < h; ++j) {
      img_coords.push_back(vec2(i, j));
    }
  }

  std::vector<bool> contained(w * h);

  std::vector<vec2> dims;

  ImagePyramidGenerator::makePyramid(3, w, h, img_coords, dims);

  ASSERT_EQ(3, dims.size());
  //  ASSERT_EQ(10, dims[0].x);
  //  ASSERT_EQ(20, dims[1].x);
  //  ASSERT_EQ(40, dims[2].x);
  //  ASSERT_EQ(20, dims[0].y);
  //  ASSERT_EQ(40, dims[1].y);
  //  ASSERT_EQ(80, dims[2].y);

  // check that all values are in range && check for duplicates.

  for (uint32_t i = 0; i < dims.size(); ++i) {
    uint32_t size = dims[i].x * dims[i].y;
    std::fill(contained.begin(), contained.end(), false);
    for (uint32_t j = 0; j < size; ++j) {
      uint32_t idx = img_coords[j].x * h + img_coords[j].y;
      ASSERT_FALSE(contained[idx]) << img_coords[j].x << " " << img_coords[j].y;  // duplicates?
      contained[idx] = true;
    }
  }

  // finally everything should be in the last level.
  for (uint32_t i = 0; i < contained.size(); ++i) {
    ASSERT_TRUE(contained[i]);
  }
}
}
