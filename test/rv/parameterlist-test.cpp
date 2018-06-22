#include <gtest/gtest.h>
#include <stdint.h>
#include <rv/ParameterList.h>
#include <rv/PrimitiveParameters.h>
#include "../test_helpers.h"

using namespace rv;

namespace
{

class ParameterListTest: public ::testing::Test
{
  protected:
    ParameterList params;
};

TEST_F(ParameterListTest, testOperations)
{
  /*** inserting different parameters ***/
  params.insert(BooleanParameter("bool", true));
  ASSERT_EQ(1, params.size());
  ASSERT_TRUE(params.hasParam("bool"));
  params.insert(FloatParameter("float", 3.0));
  ASSERT_EQ(2, params.size());
  ASSERT_TRUE(params.hasParam("float"));
  params.insert(StringParameter("string", "test"));
  ASSERT_EQ(3, params.size());
  ASSERT_TRUE(params.hasParam("string"));
  params.insert(StringParameter("string2", "remove"));
  ASSERT_EQ(4, params.size());
  ASSERT_TRUE(params.hasParam("string2"));
  ASSERT_FALSE(params.hasParam("bloat"));
  ASSERT_FALSE(params.hasParam("STRING"));

  /*** erasing parameters ***/
  params.erase("string2");
  ASSERT_EQ(3, params.size());
  ASSERT_FALSE(params.hasParam("string2"));

  /*** replacing a parameter ***/
  ASSERT_EQ("test", params.getValue<std::string>("string"));
  params.insert(StringParameter("string", "another test"));
  ASSERT_EQ("another test", params.getValue<std::string>("string"));

  /*** testing the exceptions ***/
//  ASSERT_THROW(params.getParameter<FloatParameter>("string"), legacy_fkie::Exception);
  ASSERT_THROW(params.getValue<double>("string"), rv::Exception);
  ASSERT_THROW(params.getValue<double>("not existing value"), rv::Exception);
  ASSERT_THROW(params["not existing value"], rv::Exception);

  uint32_t count = 0;
  std::vector<std::string> names;
  /*** testing the iterator ***/
  for (ParameterList::const_iterator it = params.begin(); it != params.end(); ++it)
  {
    names.push_back(it->name());
    ++count;
  }

  std::vector<std::string> expected_names;
  expected_names.push_back("bool");
  expected_names.push_back("float");
  expected_names.push_back("string");

  ASSERT_TRUE(similarVectors(expected_names, names));
  ASSERT_EQ(3, count);

  /** test copy **/
  ParameterList other = params;
  ASSERT_EQ(params.size(), other.size());
  for (ParameterList::const_iterator it = params.begin(); it != params.end(); ++it)
  {
    bool found = false;
    for (ParameterList::const_iterator it2 = other.begin(); it2 != other.end(); ++it2)
    {
      if (*it == *it2)
      {
        found = true;
        break;
      }
    }

    ASSERT_TRUE(found);
  }
}

}
