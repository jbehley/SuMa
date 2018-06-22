#ifndef TEST_HELPERS_H_
#define TEST_HELPERS_H_

#include <vector>
#include <stdint.h>
#include <boost/random.hpp>

#include <rv/point_traits.h>

// some utils to ease the generation of tests.

template<typename T>
bool similarVectors(std::vector<T>& vec1, std::vector<T>& vec2)
{
  if (vec1.size() != vec2.size())
  {
    std::cout << "expected size = " << vec1.size() << ", but got size = " << vec2.size() << std::endl;
    return false;
  }

  for (uint32_t i = 0; i < vec1.size(); ++i)
  {
    bool found = false;
    for (uint32_t j = 0; j < vec2.size(); ++j)
    {
      if (vec1[i] == vec2[j])
      {
        found = true;
        break;
      }
    }
    if (!found)
    {
      std::cout << i << "-th element (" << vec1[i] << ") not found." << std::endl;
      return false;
    }
  }

  return true;
}

template<typename T>
bool equalVectors(std::vector<T>& vec1, std::vector<T>& vec2)
{
  double eps = 0.00001;
  if (vec1.size() != vec2.size())
  {
    std::cout << "expected size = " << vec1.size() << ", but got size = " << vec2.size() << std::endl;
    return false;
  }

  for (uint32_t i = 0; i < vec1.size(); ++i)
  {
    if (std::abs(vec1[i] - vec2[i]) > eps)
    {
      std::cout << i << "-th element not equal: " << vec1[i] << " vs. " << vec2[i] << std::endl;
      return false;
    }
  }

  return true;
}

// check if vec1 is subset of vec2
template<typename T>
bool subsetOf(std::vector<T>& vec1, std::vector<T>& vec2)
{
  for (uint32_t i = 0; i < vec1.size(); ++i)
  {
    bool found = false;
    for (uint32_t j = 0; j < vec2.size(); ++j)
    {
      if (vec1[i] == vec2[j])
      {
        found = true;
        break;
      }
    }

    if (!found)
    {
      std::cout << i << "-th element (" << vec1[i] << ") is not in superset." << std::endl;
      return false;
    }
  }

  return true;
}

/**
 * \brief generate a uniform randomly samples set of points in [a, b]^3
 * @param pts
 * @param N
 * @param seed
 */
template<typename PointT, typename Allocator>
void generateRandomPoints(std::vector<PointT, Allocator>& pts, uint32_t N, uint32_t seed = 0, float a = -5, float b = 5)
{
  boost::mt11213b mtwister(seed);
  boost::uniform_01<> gen;
  pts.clear();
  pts.reserve(N);
  // generate N random points in [a,b] x [a,b] x [a,b]...
  float range = b - a;
  for (uint32_t i = 0; i < N; ++i)
  {
    PointT pt;
    rv::set<0>(pt, range * gen(mtwister) + a);
    rv::set<1>(pt, range * gen(mtwister) + a);
    rv::set<2>(pt, range * gen(mtwister) + a);

    pts.push_back(pt);
  }
}

#endif /* TEST_HELPERS_H_ */
