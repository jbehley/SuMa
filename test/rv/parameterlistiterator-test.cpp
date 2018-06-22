#include <gtest/gtest.h>
#include <rv/ParameterListIterator.h>
#include <rv/PrimitiveParameters.h>
#include <rv/CompositeParameter.h>
#include <rv/RangeParameter.h>

using namespace rv;

namespace
{

}
class ParameterListIteratorTest: public ::testing::Test
{

};

//void ParameterListIteratorTestCase::testCopy()
//{
//  ParameterList params1;
//  params1.insert(IntegerParameter("foo", 1337));
//  params1.insert(StringParameter("bar", "bla"));
//  ParameterList rlist;
//  rlist.insert(IntegerParameter("value0", 1));
//  rlist.insert(IntegerParameter("value1", 3));
//  rlist.insert(IntegerParameter("value2", 3));
//  rlist.insert(IntegerParameter("value3", 7));
//  RangeParameter rparam("range", rlist);
//  params1.insert(rparam);
//
//  ParameterListIterator it(params1);
//  it.next();
//  it.next();
//
//  /** now testing the copy actions. **/
//  ParameterListIterator it2(it);
//  ParameterListIterator it3(params1);
//  it3 = it;
//
//  ASSERT_TRUE(it.hasNext());
//  ASSERT_EQ(it.hasNext(), it2.hasNext());
//  ASSERT_EQ(it.hasNext(), it3.hasNext());
//
//  const ParameterList& goldparams = it.next();
//  const ParameterList& testparams1 = it2.next();
//  const ParameterList& testparams2 = it3.next();
//
//  ASSERT_TRUE((goldparams == testparams1));
//  ASSERT_TRUE((goldparams == testparams2));
//
//}

TEST_F(ParameterListIteratorTest, testIterate)
{
  {
    ParameterList params1;
    params1.insert(IntegerParameter("foo", 1337));
    params1.insert(StringParameter("bar", "bla"));

    ParameterListIterator it(params1);
    ASSERT_TRUE(it.hasNext());
    const ParameterList& params = it.next();
    ASSERT_FALSE(it.hasNext());
    ASSERT_TRUE(params.hasParam("foo"));
    ASSERT_TRUE(params.hasParam("bar"));
    ASSERT_EQ(1337, params.getValue<int>("foo"));
    ASSERT_EQ("bla", params.getValue<std::string>("bar"));
  }

  {
    ParameterList params1;
    params1.insert(IntegerParameter("foo", 1337));
    params1.insert(StringParameter("bar", "bla"));
    ParameterList rlist;
    rlist.insert(IntegerParameter("value0", 1));
    rlist.insert(IntegerParameter("value1", 3));
    rlist.insert(IntegerParameter("value2", 3));
    rlist.insert(IntegerParameter("value3", 7));
    RangeParameter rparam("range", rlist);
    params1.insert(rparam);

    ParameterListIterator it(params1);

    double values[] =
    { 1, 3, 3, 7 };

    for (uint32_t i = 0; i < 4; ++i)
    {
      ASSERT_TRUE(it.hasNext());
      const ParameterList& params = it.next();
      ASSERT_TRUE(params.hasParam("foo"));
      ASSERT_TRUE(params.hasParam("bar"));
      ASSERT_TRUE(params.hasParam("range"));
      ASSERT_EQ(1337, params.getValue<int>("foo"));
      ASSERT_EQ("bla", params.getValue<std::string>("bar"));
      ASSERT_EQ(values[i], params.getValue<int>("range"));
    }
    ASSERT_FALSE(it.hasNext());
  }

  {
    ParameterList params1;
    params1.insert(IntegerParameter("foo", 1337));
    params1.insert(StringParameter("bar", "bla"));

    ParameterList range;
    range.insert(IntegerParameter("value0", 1));
    range.insert(IntegerParameter("value1", 3));
    RangeParameter rparam("range1", range);
    params1.insert(rparam);

    range.insert(IntegerParameter("value0", 4));
    range.insert(IntegerParameter("value1", 7));
    RangeParameter rparam2("range2", range);

    ParameterList cparam_list;
    cparam_list.insert(FloatParameter("foo", 1.337));
    cparam_list.insert(rparam2);

    CompositeParameter cparam("comp", cparam_list);
    params1.insert(cparam);

//    std::cout << "----------------------------" << std::endl;

//    for (ParameterList::const_iterator it = params1.begin(); it != params1.end(); ++it)
//      std::cout << *it << std::endl;

    ParameterListIterator it(params1);

    double values1[] =
    { 1, 1, 3, 3 };
    double values2[] =
    { 4, 7, 4, 7 };

    for (uint32_t i = 0; i < 4; ++i)
    {
      ASSERT_TRUE(it.hasNext());

      const ParameterList& params = it.next();
//      std::cout << "number of parameters = " << params.size() << std::endl;
//      for (ParameterList::const_iterator it = params.begin(); it != params.end(); ++it)
//        std::cout << *it << std::endl;

      ASSERT_TRUE(params.hasParam("foo"));
      ASSERT_TRUE(params.hasParam("bar"));
      ASSERT_TRUE(params.hasParam("range1"));
      ASSERT_TRUE(params.hasParam("comp"));
      ASSERT_EQ(1337, params.getValue<int>("foo"));
      ASSERT_EQ("bla", params.getValue<std::string>("bar"));
      ASSERT_EQ(values1[i], params.getValue<int>("range1"));

      const CompositeParameter* cparam1 = params.getParameter<CompositeParameter>("comp");
      const ParameterList& clist = cparam1->getParams();
      ASSERT_TRUE(clist.hasParam("foo"));
      ASSERT_TRUE(clist.hasParam("range2"));
      ASSERT_NEAR(1.337, clist.getValue<double>("foo"), 0.0001);
      ASSERT_EQ(values2[i], clist.getValue<int>("range2"));
    }
    ASSERT_FALSE(it.hasNext());
  }

  /** now testing with some filter. **/
  {
    ParameterList params1;
    params1.insert(IntegerParameter("foo", 1337));
    params1.insert(StringParameter("bar", "bla"));
    ParameterList rlist;
    rlist.insert(IntegerParameter("value0", 1));
    rlist.insert(IntegerParameter("value1", 3));
    rlist.insert(IntegerParameter("value2", 3));
    rlist.insert(IntegerParameter("value3", 7));
    RangeParameter rparam("range", rlist);
    params1.insert(rparam);

    ParameterList rlist2;
    rlist2.insert(IntegerParameter("value0", 10));
    rlist2.insert(IntegerParameter("value1", 11));
    rlist2.insert(IntegerParameter("value2", 12));
    RangeParameter rparam2("another-range", rlist2);
    params1.insert(rparam2);

    std::vector<std::string> filtered_names;
    filtered_names.push_back("range");

    ParameterListIterator it(params1, filtered_names);
    double values[] =
    { 1, 3, 3, 7 };

    for (uint32_t i = 0; i < 4; ++i)
    {
      ASSERT_TRUE(it.hasNext());
      const ParameterList& params = it.next();

//      std::cout << " ===== current params ==== " << std::endl;
//      for (ParameterList::const_iterator pit = params.begin(); pit != params.end(); ++pit)
//        std::cout << *pit << std::endl;

      ASSERT_TRUE(params.hasParam("foo"));
      ASSERT_TRUE(params.hasParam("bar"));
      ASSERT_TRUE(params.hasParam("range"));
      ASSERT_TRUE(params.hasParam("another-range"));
      ASSERT_EQ(1337, params.getValue<int>("foo"));
      ASSERT_EQ("bla", params.getValue<std::string>("bar"));
      ASSERT_EQ(values[i], params.getValue<int>("range"));
      ASSERT_EQ("range", params["another-range"].type());

      ParameterListIterator it2(params);
      int values2[] =
      { 10, 11, 12 };
      for (uint32_t j = 0; j < 3; ++j)
      {
        ASSERT_TRUE(it2.hasNext());
        const ParameterList& params2 = it2.next();

        ASSERT_TRUE(params2.hasParam("foo"));
        ASSERT_TRUE(params2.hasParam("bar"));
        ASSERT_TRUE(params2.hasParam("range"));
        ASSERT_TRUE(params2.hasParam("another-range"));
        ASSERT_EQ(1337, params2.getValue<int>("foo"));
        ASSERT_EQ("bla", params2.getValue<std::string>("bar"));
        ASSERT_EQ(values[i], params2.getValue<int>("range"));
        ASSERT_EQ(values2[j], params2.getValue<int>("another-range"));
      }
      ASSERT_FALSE(it2.hasNext());
    }
    ASSERT_FALSE(it.hasNext());

  }

}
