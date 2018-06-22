#include <gtest/gtest.h>
#include <stdint.h>
#include <rv/geometry.h>

using namespace rv;

namespace
{

class GeometryTest: public ::testing::Test
{
  protected:
    void SetUp()
    {
      EPSILON = 0.00001;
    }

    double EPSILON;
};

TEST_F(GeometryTest, testInitializers)
{
  Point3f p(1, 2, 3);
  const Point3f q(-3, 2, -1); // for testing const access

  Normal3f n(4, 5, 6);
  const Normal3f m(-2, 1, 5); // for testing const access

  Vector3f v(-7, 8, -9);
  const Vector3f u(-2, 1, 4); // for testing const access

  ASSERT_NEAR(1.0f, p.x(), EPSILON);
  ASSERT_NEAR(2.0f, p.y(), EPSILON);
  ASSERT_NEAR(3.0f, p.z(), EPSILON);

  ASSERT_NEAR(1.0f, p[0], EPSILON);
  ASSERT_NEAR(2.0f, p[1], EPSILON);
  ASSERT_NEAR(3.0f, p[2], EPSILON);

  ASSERT_NEAR(1.0f, p.vec[0], EPSILON);
  ASSERT_NEAR(2.0f, p.vec[1], EPSILON);
  ASSERT_NEAR(3.0f, p.vec[2], EPSILON);
  ASSERT_NEAR(1.0f, p.vec[3], EPSILON);

  ASSERT_NEAR(-3.0f, q.x(), EPSILON);
  ASSERT_NEAR(2.0f, q.y(), EPSILON);
  ASSERT_NEAR(-1.0f, q.z(), EPSILON);

  ASSERT_NEAR(-3.0f, q[0], EPSILON);
  ASSERT_NEAR(2.0f, q[1], EPSILON);
  ASSERT_NEAR(-1.0f, q[2], EPSILON);

  ASSERT_NEAR(-3.0f, q.vec[0], EPSILON);
  ASSERT_NEAR(2.0f, q.vec[1], EPSILON);
  ASSERT_NEAR(-1.0f, q.vec[2], EPSILON);
  ASSERT_NEAR(1.0f, q.vec[3], EPSILON);

  ASSERT_NEAR(4.0f, n.x(), EPSILON);
  ASSERT_NEAR(5.0f, n.y(), EPSILON);
  ASSERT_NEAR(6.0f, n.z(), EPSILON);

  ASSERT_NEAR(4.0f, n[0], EPSILON);
  ASSERT_NEAR(5.0f, n[1], EPSILON);
  ASSERT_NEAR(6.0f, n[2], EPSILON);

  ASSERT_NEAR(4.0f, n.vec[0], EPSILON);
  ASSERT_NEAR(5.0f, n.vec[1], EPSILON);
  ASSERT_NEAR(6.0f, n.vec[2], EPSILON);
  ASSERT_NEAR(0.0f, n.vec[3], EPSILON);

  ASSERT_NEAR(-2.0f, m.x(), EPSILON);
  ASSERT_NEAR(1.0f, m.y(), EPSILON);
  ASSERT_NEAR(5.0f, m.z(), EPSILON);

  ASSERT_NEAR(-2.0f, m[0], EPSILON);
  ASSERT_NEAR(1.0f, m[1], EPSILON);
  ASSERT_NEAR(5.0f, m[2], EPSILON);

  ASSERT_NEAR(-2.0f, m.vec[0], EPSILON);
  ASSERT_NEAR(1.0f, m.vec[1], EPSILON);
  ASSERT_NEAR(5.0f, m.vec[2], EPSILON);
  ASSERT_NEAR(0.0f, m.vec[3], EPSILON);

  ASSERT_NEAR(-7.0f, v.x(), EPSILON);
  ASSERT_NEAR(8.0f, v.y(), EPSILON);
  ASSERT_NEAR(-9.0f, v.z(), EPSILON);

  ASSERT_NEAR(-7.0f, v[0], EPSILON);
  ASSERT_NEAR(8.0f, v[1], EPSILON);
  ASSERT_NEAR(-9.0f, v[2], EPSILON);

  ASSERT_NEAR(-7.0f, v.vec[0], EPSILON);
  ASSERT_NEAR(8.0f, v.vec[1], EPSILON);
  ASSERT_NEAR(-9.0f, v.vec[2], EPSILON);
  ASSERT_NEAR(0.0f, v.vec[3], EPSILON);

  ASSERT_NEAR(-2.0f, u.x(), EPSILON);
  ASSERT_NEAR(1.0f, u.y(), EPSILON);
  ASSERT_NEAR(4.0f, u.z(), EPSILON);

  ASSERT_NEAR(-2.0f, u[0], EPSILON);
  ASSERT_NEAR(1.0f, u[1], EPSILON);
  ASSERT_NEAR(4.0f, u[2], EPSILON);

  ASSERT_NEAR(-2.0f, u.vec[0], EPSILON);
  ASSERT_NEAR(1.0f, u.vec[1], EPSILON);
  ASSERT_NEAR(4.0f, u.vec[2], EPSILON);
  ASSERT_NEAR(0.0f, u.vec[3], EPSILON);
}

TEST_F(GeometryTest, testCopy)
{
  Point3f p(1, 2, 3);
  const Point3f q(-3, 2, -1); // for testing const access

  Normal3f n(4, 5, 6);
  const Normal3f m(-2, 1, 5); // for testing const access

  Vector3f v(-7, 8, -9);
  const Vector3f u(-2, 1, 4); // for testing const access

  Point3f pcopy(p);
  ASSERT_NEAR(1.0f, pcopy.x(), EPSILON);
  ASSERT_NEAR(2.0f, pcopy.y(), EPSILON);
  ASSERT_NEAR(3.0f, pcopy.z(), EPSILON);
  ASSERT_NEAR(1.0f, pcopy.vec[3], EPSILON);

  pcopy = q;
  ASSERT_NEAR(-3.0f, pcopy.x(), EPSILON);
  ASSERT_NEAR(2.0f, pcopy.y(), EPSILON);
  ASSERT_NEAR(-1.0f, pcopy.z(), EPSILON);
  ASSERT_NEAR(1.0f, pcopy.vec[3], EPSILON);

  Normal3f ncopy(n);
  ASSERT_NEAR(4.0f, ncopy.x(), EPSILON);
  ASSERT_NEAR(5.0f, ncopy.y(), EPSILON);
  ASSERT_NEAR(6.0f, ncopy.z(), EPSILON);
  ASSERT_NEAR(0.0f, ncopy.vec[3], EPSILON);

  ncopy = m;
  ASSERT_NEAR(-2.0f, ncopy.x(), EPSILON);
  ASSERT_NEAR(1.0f, ncopy.y(), EPSILON);
  ASSERT_NEAR(5.0f, ncopy.z(), EPSILON);
  ASSERT_NEAR(0.0f, ncopy.vec[3], EPSILON);

  Vector3f vcopy(v);

  ASSERT_NEAR(-7.0f, vcopy.x(), EPSILON);
  ASSERT_NEAR(8.0f, vcopy.y(), EPSILON);
  ASSERT_NEAR(-9.0f, vcopy.z(), EPSILON);
  ASSERT_NEAR(0.0f, vcopy.vec[3], EPSILON);

  vcopy = u;

  ASSERT_NEAR(-2.0f, vcopy.x(), EPSILON);
  ASSERT_NEAR(1.0f, vcopy.y(), EPSILON);
  ASSERT_NEAR(4.0f, vcopy.z(), EPSILON);
  ASSERT_NEAR(0.0f, vcopy.vec[3], EPSILON);

}

TEST_F(GeometryTest, testPointOperations)
{
  Point3f p(1, 2, 3);
  const Point3f q(-3, 2, -1); // for testing const access

  Normal3f n(4, 5, 6);
  const Normal3f m(-2, 1, 5); // for testing const access

  Vector3f v(-7, 8, -9);
  const Vector3f u(-2, 1, 4); // for testing const access

  Point3f pq = p + q;
  ASSERT_NEAR(-2.0f, pq.x(), EPSILON);
  ASSERT_NEAR(4.0f, pq.y(), EPSILON);
  ASSERT_NEAR(2.0f, pq.z(), EPSILON);
  ASSERT_NEAR(1.0f, pq.vec[3], EPSILON);

  Point3f pq2 = pq / 3.0f;
  ASSERT_NEAR(-2.0f / 3.0f, pq2.x(), EPSILON);
  ASSERT_NEAR(4.0f / 3.0f, pq2.y(), EPSILON);
  ASSERT_NEAR(2.0f / 3.0f, pq2.z(), EPSILON);
  ASSERT_NEAR(1.0f, pq2.vec[3], EPSILON);

  Point3f pcopy = q;
  pcopy += p;
  ASSERT_NEAR(-2.0f, pcopy.x(), EPSILON);
  ASSERT_NEAR(4.0f, pcopy.y(), EPSILON);
  ASSERT_NEAR(2.0f, pcopy.z(), EPSILON);
  ASSERT_NEAR(1.0f, pcopy.vec[3], EPSILON);

  pcopy /= 3.0f;
  ASSERT_NEAR(-2.0f / 3.0f, pq2.x(), EPSILON);
  ASSERT_NEAR(4.0f / 3.0f, pq2.y(), EPSILON);
  ASSERT_NEAR(2.0f / 3.0f, pq2.z(), EPSILON);
  ASSERT_NEAR(1.0f, pq2.vec[3], EPSILON);

  Point3f pq3 = p - v;
  ASSERT_NEAR(8.0f, pq3.x(), EPSILON);
  ASSERT_NEAR(-6.0f, pq3.y(), EPSILON);
  ASSERT_NEAR(12.0f, pq3.z(), EPSILON);
  ASSERT_NEAR(1.0f, pq3.vec[3], EPSILON);

  pcopy = p;
  pcopy -= v;
  ASSERT_NEAR(8.0f, pcopy.x(), EPSILON);
  ASSERT_NEAR(-6.0f, pcopy.y(), EPSILON);
  ASSERT_NEAR(12.0f, pcopy.z(), EPSILON);
  ASSERT_NEAR(1.0f, pcopy.vec[3], EPSILON);

  Point3f pv = p + v;
  ASSERT_NEAR(-6.0f, pv.x(), EPSILON);
  ASSERT_NEAR(10.0f, pv.y(), EPSILON);
  ASSERT_NEAR(-6.0f, pv.z(), EPSILON);
  ASSERT_NEAR(1.0f, pv.vec[3], EPSILON);

  pcopy = p;
  pcopy += v;
  ASSERT_NEAR(-6.0f, pcopy.x(), EPSILON);
  ASSERT_NEAR(10.0f, pcopy.y(), EPSILON);
  ASSERT_NEAR(-6.0f, pcopy.z(), EPSILON);
  ASSERT_NEAR(1.0f, pcopy.vec[3], EPSILON);

  Vector3f pmq = p - q;
  ASSERT_NEAR(4.0f, pmq.x(), EPSILON);
  ASSERT_NEAR(0.0f, pmq.y(), EPSILON);
  ASSERT_NEAR(4.0f, pmq.z(), EPSILON);
  ASSERT_NEAR(0.0f, pmq.vec[3], EPSILON);

  Point3f p12 = p * 12.0f;
  ASSERT_NEAR(12.0f, p12.x(), EPSILON);
  ASSERT_NEAR(24.0f, p12.y(), EPSILON);
  ASSERT_NEAR(36.0f, p12.z(), EPSILON);
  ASSERT_NEAR(1.0f, p12.vec[3], EPSILON);

  pcopy = p;
  pcopy *= -10.0f;
  ASSERT_NEAR(-10.0f, pcopy.x(), EPSILON);
  ASSERT_NEAR(-20.0f, pcopy.y(), EPSILON);
  ASSERT_NEAR(-30.0f, pcopy.z(), EPSILON);
  ASSERT_NEAR(1.0f, pcopy.vec[3], EPSILON);

  Point3f p113 = -113.0f * p;
  ASSERT_NEAR(-113.0f, p113.x(), EPSILON);
  ASSERT_NEAR(-226.0f, p113.y(), EPSILON);
  ASSERT_NEAR(-339.0f, p113.z(), EPSILON);
  ASSERT_NEAR(1.0f, p113.vec[3], EPSILON);

  Point3f p113clone(-113.0f, -226.0f, -339.0f);
  ASSERT_TRUE((p113clone == p113));
  ASSERT_FALSE((p113 == p));
  ASSERT_TRUE((p113 != p));
  ASSERT_FALSE((p113 != p113clone));

  ASSERT_NEAR(5.65685f, Distance(p, q), EPSILON);
  ASSERT_NEAR(32.0f, DistanceSquared(p, q), EPSILON);

}

TEST_F(GeometryTest, testVectorOperations)
{
  Point3f p(1, 2, 3);
  const Point3f q(-3, 2, -1); // for testing const access

  Normal3f n(4, 5, 6);
  const Normal3f m(-2, 1, 5); // for testing const access

  Vector3f v(-7, 8, -9);
  const Vector3f u(-2, 1, 4); // for testing const access

  Vector3f vp(p);
  ASSERT_NEAR(1., vp.x(), EPSILON);
  ASSERT_NEAR(2., vp.y(), EPSILON);
  ASSERT_NEAR(3., vp.z(), EPSILON);
  ASSERT_NEAR(0., vp.vec[3], EPSILON);

  Vector3f vu = v + u;
  ASSERT_NEAR(-9.0f, vu.x(), EPSILON);
  ASSERT_NEAR(9.0f, vu.y(), EPSILON);
  ASSERT_NEAR(-5.0f, vu.z(), EPSILON);
  ASSERT_NEAR(0.0f, vu.vec[3], EPSILON);

  Vector3f vu2 = vu / 3.0f;
  ASSERT_NEAR(-9.0f / 3.0f, vu2.x(), EPSILON);
  ASSERT_NEAR(9.0f / 3.0f, vu2.y(), EPSILON);
  ASSERT_NEAR(-5.0f / 3.0f, vu2.z(), EPSILON);
  ASSERT_NEAR(0.0f, vu2.vec[3], EPSILON);

  Vector3f vcopy = v;
  vcopy += u;
  ASSERT_NEAR(-9.0f, vcopy.x(), EPSILON);
  ASSERT_NEAR(9.0f, vcopy.y(), EPSILON);
  ASSERT_NEAR(-5.0f, vcopy.z(), EPSILON);
  ASSERT_NEAR(0.0f, vcopy.vec[3], EPSILON);

  vcopy /= 3.0f;
  ASSERT_NEAR(-9.0f / 3.0f, vcopy.x(), EPSILON);
  ASSERT_NEAR(9.0f / 3.0f, vcopy.y(), EPSILON);
  ASSERT_NEAR(-5.0f / 3.0f, vcopy.z(), EPSILON);
  ASSERT_NEAR(0.0f, vcopy.vec[3], EPSILON);

  Vector3f vmu = v - u;
  ASSERT_NEAR(-5.0f, vmu.x(), EPSILON);
  ASSERT_NEAR(7.0f, vmu.y(), EPSILON);
  ASSERT_NEAR(-13.0f, vmu.z(), EPSILON);
  ASSERT_NEAR(0.0f, vmu.vec[3], EPSILON);

  vcopy = v;
  vcopy -= u;
  ASSERT_NEAR(-5.0f, vcopy.x(), EPSILON);
  ASSERT_NEAR(7.0f, vcopy.y(), EPSILON);
  ASSERT_NEAR(-13.0f, vcopy.z(), EPSILON);
  ASSERT_NEAR(0.0f, vcopy.vec[3], EPSILON);

  Vector3f mv = -v;
  ASSERT_NEAR(7.0f, mv.x(), EPSILON);
  ASSERT_NEAR(-8.0f, mv.y(), EPSILON);
  ASSERT_NEAR(9.0f, mv.z(), EPSILON);
  ASSERT_NEAR(0.0f, mv.vec[3], EPSILON);

  Vector3f v5 = v * 5.0f;
  ASSERT_NEAR(-35.0f, v5.x(), EPSILON);
  ASSERT_NEAR(40.0f, v5.y(), EPSILON);
  ASSERT_NEAR(-45.0f, v5.z(), EPSILON);
  ASSERT_NEAR(0.0f, v5.vec[3], EPSILON);

  vcopy = v;
  vcopy *= -3.0f;
  ASSERT_NEAR(21.0f, vcopy.x(), EPSILON);
  ASSERT_NEAR(-24.0f, vcopy.y(), EPSILON);
  ASSERT_NEAR(27.0f, vcopy.z(), EPSILON);
  ASSERT_NEAR(0.0f, vcopy.vec[3], EPSILON);

  Vector3f v12 = 12.0f * v;
  ASSERT_NEAR(-84.0f, v12.x(), EPSILON);
  ASSERT_NEAR(96.0f, v12.y(), EPSILON);
  ASSERT_NEAR(-108.0f, v12.z(), EPSILON);
  ASSERT_NEAR(0.0f, v12.vec[3], EPSILON);

  Vector3f v12clone(-84.0f, 96.0f, -108.0f);
  ASSERT_TRUE((v12clone == v12));
  ASSERT_FALSE((v12 == v));
  ASSERT_TRUE((v12 != v));
  ASSERT_FALSE((v12clone != v12));

  ASSERT_NEAR(13.92838827718412f, v.Length(), EPSILON);
  ASSERT_NEAR(194.0f, v.LengthSquared(), EPSILON);

  Vector3f vconv(n);
  ASSERT_NEAR(4., vconv.x(), EPSILON);
  ASSERT_NEAR(5., vconv.y(), EPSILON);
  ASSERT_NEAR(6., vconv.z(), EPSILON);
  ASSERT_NEAR(0., vconv.vec[3], EPSILON);

  Vector3f normalizedv = Normalize(v);
  ASSERT_NEAR(-0.5025707110324167, normalizedv.x(), EPSILON);
  ASSERT_NEAR( 0.5743665268941905, normalizedv.y(), EPSILON);
  ASSERT_NEAR(-0.6461623427559643, normalizedv.z(), EPSILON);
  ASSERT_NEAR(0., normalizedv.vec[3], EPSILON);
  ASSERT_NEAR(1., normalizedv.Length(), EPSILON);

}

TEST_F(GeometryTest, testNormalOperations)
{
  Point3f p(1, 2, 3);
  const Point3f q(-3, 2, -1); // for testing const access

  Normal3f n(4, 5, 6);
  const Normal3f m(-2, 1, 5); // for testing const access

  Vector3f v(-7, 8, -9);
  const Vector3f u(-2, 1, 4); // for testing const access

  Normal3f nconv(v);
  ASSERT_NEAR(-7., nconv.x(), EPSILON);
  ASSERT_NEAR(8., nconv.y(), EPSILON);
  ASSERT_NEAR(-9., nconv.z(), EPSILON);

  Normal3f nm = n + m;
  ASSERT_NEAR(2.0f, nm.x(), EPSILON);
  ASSERT_NEAR(6.0f, nm.y(), EPSILON);
  ASSERT_NEAR(11.0f, nm.z(), EPSILON);
  ASSERT_NEAR(0.0f, nm.vec[3], EPSILON);

  Normal3f nm2 = nm / 4.0f;
  ASSERT_NEAR(0.5f, nm2.x(), EPSILON);
  ASSERT_NEAR(1.5f, nm2.y(), EPSILON);
  ASSERT_NEAR(2.75f, nm2.z(), EPSILON);
  ASSERT_NEAR(0.0f, nm2.vec[3], EPSILON);

  Normal3f ncopy = n;
  ncopy += m;
  ASSERT_NEAR(2.0f, ncopy.x(), EPSILON);
  ASSERT_NEAR(6.0f, ncopy.y(), EPSILON);
  ASSERT_NEAR(11.0f, ncopy.z(), EPSILON);
  ASSERT_NEAR(0.0f, ncopy.vec[3], EPSILON);

  ncopy /= 4.0f;
  ASSERT_NEAR(0.5f, ncopy.x(), EPSILON);
  ASSERT_NEAR(1.5f, ncopy.y(), EPSILON);
  ASSERT_NEAR(2.75f, ncopy.z(), EPSILON);
  ASSERT_NEAR(0.0f, ncopy.vec[3], EPSILON);

  Normal3f nmm = n - m;
  ASSERT_NEAR(6.0f, nmm.x(), EPSILON);
  ASSERT_NEAR(4.0f, nmm.y(), EPSILON);
  ASSERT_NEAR(1.0f, nmm.z(), EPSILON);
  ASSERT_NEAR(0.0f, nmm.vec[3], EPSILON);

  ncopy = n;
  ncopy -= m;
  ASSERT_NEAR(6.0f, ncopy.x(), EPSILON);
  ASSERT_NEAR(4.0f, ncopy.y(), EPSILON);
  ASSERT_NEAR(1.0f, ncopy.z(), EPSILON);
  ASSERT_NEAR(0.0f, ncopy.vec[3], EPSILON);

  Normal3f nminus = -n;
  ASSERT_NEAR(-4.0f, nminus.x(), EPSILON);
  ASSERT_NEAR(-5.0f, nminus.y(), EPSILON);
  ASSERT_NEAR(-6.0f, nminus.z(), EPSILON);
  ASSERT_NEAR(0.0f, nminus.vec[3], EPSILON);

  Normal3f n2 = n * -2.0f;
  ASSERT_NEAR(-8.0f, n2.x(), EPSILON);
  ASSERT_NEAR(-10.0f, n2.y(), EPSILON);
  ASSERT_NEAR(-12.0f, n2.z(), EPSILON);
  ASSERT_NEAR(0.0f, n2.vec[3], EPSILON);

  ncopy = n;
  ncopy *= -2.0f;
  ASSERT_NEAR(-8.0f, ncopy.x(), EPSILON);
  ASSERT_NEAR(-10.0f, ncopy.y(), EPSILON);
  ASSERT_NEAR(-12.0f, ncopy.z(), EPSILON);
  ASSERT_NEAR(0.0f, ncopy.vec[3], EPSILON);

  Normal3f n45 = -4.5f * n;
  ASSERT_NEAR(-18.0f, n45.x(), EPSILON);
  ASSERT_NEAR(-22.5f, n45.y(), EPSILON);
  ASSERT_NEAR(-27.0f, n45.z(), EPSILON);
  ASSERT_NEAR(0.0f, n45.vec[3], EPSILON);

  Normal3f n45clone(-18.0f, -22.5f, -27.0f);
  ASSERT_TRUE((n45clone == n45));
  ASSERT_FALSE((n45 == n));
  ASSERT_TRUE((n45clone != n));
  ASSERT_FALSE((n45clone != n45));

  ASSERT_NEAR(8.77496f, n.Length(), EPSILON);
  ASSERT_NEAR(77.0f, n.LengthSquared(), EPSILON);

  Normal3f normalizedn = Normalize(n);

  ASSERT_NEAR(0.4558423058385518, normalizedn.x(), EPSILON);
  ASSERT_NEAR(0.5698028822981898, normalizedn.y(), EPSILON);
  ASSERT_NEAR(0.6837634587578276, normalizedn.z(), EPSILON);
  ASSERT_NEAR(0.0f, normalizedn.vec[3], EPSILON);
  ASSERT_NEAR(1.0f, normalizedn.Length(), EPSILON);
}

TEST_F(GeometryTest, testDotCross)
{
  Point3f p(1, 2, 3);
  const Point3f q(-3, 2, -1); // for testing const access

  Normal3f n(4, 5, 6);
  const Normal3f m(-2, 1, 5); // for testing const access

  Vector3f v(-7, 8, -9);
  const Vector3f u(-2, 1, 4); // for testing const access

  /** dot products. **/
  Vector3f v2(-4, -1, 3);
  Normal3f nv(v.x(), v.y(), v.z());
  Normal3f nv2(v2.x(), v2.y(), v2.z());

  ASSERT_NEAR(-7, Dot(v, v2), EPSILON);
  ASSERT_NEAR(-7, Dot(nv, v2), EPSILON);
  ASSERT_NEAR(-7, Dot(v, nv2), EPSILON);
  ASSERT_NEAR(-7, Dot(nv, nv2), EPSILON);

  /** cross products. **/
  Vector3f c = Cross(v, v2);
  ASSERT_NEAR(15, c.x(), EPSILON);
  ASSERT_NEAR(57, c.y(), EPSILON);
  ASSERT_NEAR(39, c.z(), EPSILON);

  Vector3f c2 = Cross(nv, v2);
  ASSERT_NEAR(15, c2.x(), EPSILON);
  ASSERT_NEAR(57, c2.y(), EPSILON);
  ASSERT_NEAR(39, c2.z(), EPSILON);

  Vector3f c3 = Cross(v, nv2);
  ASSERT_NEAR(15.0f, c3.x(), EPSILON);
  ASSERT_NEAR(57.0f, c3.y(), EPSILON);
  ASSERT_NEAR(39.0f, c3.z(), EPSILON);
}

}
