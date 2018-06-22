#include <gtest/gtest.h>
#include <stdint.h>
#include <rv/XmlDocument.h>
#include <rv/XmlNode.h>

#include <sstream>
#include <rv/Parameter.h>
#include <rv/PrimitiveParameters.h>
#include <rv/ParameterList.h>
#include <rv/RangeParameter.h>

using namespace rv;

namespace
{
class ParameterTest: public ::testing::Test
{
  protected:
    void SetUp()
    {
      std::stringstream buffer;

      buffer << "<xml>" << std::endl;
      buffer << "  <param name = \"intValue\" type = \"integer\">3</param>" << std::endl;
      buffer << "  <param name = \"floatValue\" type = \"float\">4.5</param>" << std::endl;
      buffer << "  <param name = \"stringValue\" type = \"string\">test</param>" << std::endl;
      buffer << "</xml>" << std::endl;

      xmlDocument = buffer.str();
    }

    std::string xmlDocument;
};

TEST_F(ParameterTest, testConversion)
{
  /********** IntegerParameter ***********/
  IntegerParameter intParam("intValue", 3);
  int ivalue = intParam;
  ASSERT_EQ(3, ivalue);
  Parameter* param = &intParam;
  uint32_t ivalue2 = *param;
  ASSERT_EQ(3, ivalue2);

  /********** FloatParameter ***********/
  FloatParameter floatParam("floatValue", 4.5);
  float fvalue = floatParam;
  ASSERT_EQ(4.5f, fvalue);
  param = &floatParam;
  double fvalue2 = *param;
  ASSERT_EQ(4.5, fvalue2);

  /********** StringParameter ***********/
  StringParameter stringParam("stringValue", "test");
  std::string svalue = stringParam;
  ASSERT_EQ("test", svalue);
  param = &stringParam;
  std::string svalue2 = *param;
  ASSERT_EQ("test", svalue2);

  /********** MatrixParameter ***********/
//  legacy_fkie::Matrix exp = legacy_fkie::Matrix::ones(4);
//  MatrixParameter matrixParam("matrixValue", legacy_fkie::Matrix::ones(4));
//  param = &matrixParam;
//  legacy_fkie::Matrix m = matrixParam;
//  //	assertEqualMatrices(exp, m, __LINE__, __FILE__);
//  legacy_fkie::Matrix m2;
//  m2 = *param;
  //	assertEqualMatrices(exp, m2, __LINE__, __FILE__);
}

TEST_F(ParameterTest, testParseParameter)
{
  XmlDocument doc = XmlDocument::fromData(xmlDocument);
  XmlNode root = doc.root();

  XmlNode::List children = root.getChildren();

  ParameterList map;

  Parameter* param = Parameter::parseParameter(children[0]);
  ASSERT_TRUE((param != 0));
  ASSERT_EQ("intValue", param->name());
  IntegerParameter* intParam = dynamic_cast<IntegerParameter*>(param);
  ASSERT_TRUE((intParam != 0));
  ASSERT_EQ(3, intParam->value());
  map.insert(*param);
  ASSERT_EQ(1, map.size());
  delete param;

  param = Parameter::parseParameter(children[1]);
  ASSERT_TRUE((param != 0));
  ASSERT_EQ("floatValue", param->name());
  FloatParameter* floatParam = dynamic_cast<FloatParameter*>(param);
  ASSERT_TRUE((floatParam != 0));
  ASSERT_EQ(4.5, floatParam->value());
  map.insert(*param);
  ASSERT_EQ(2, map.size());
  delete param;

  param = Parameter::parseParameter(children[2]);
  ASSERT_TRUE((param != 0));
  ASSERT_EQ("stringValue", param->name());
  StringParameter* stringParam = dynamic_cast<StringParameter*>(param);
  ASSERT_TRUE((stringParam != 0));
  ASSERT_EQ("test", stringParam->value());
  map.insert(*param);
  ASSERT_EQ(3, map.size());
  delete param;

  /* check map */ASSERT_FALSE((!map.hasParam("intValue")));
  ASSERT_FALSE((!map.hasParam("floatValue")));
  ASSERT_FALSE((!map.hasParam("stringValue")));
  intParam = map.getParameter<IntegerParameter>("intValue");
  ASSERT_TRUE((intParam != 0));
  ASSERT_EQ(3, intParam->value());
  floatParam = map.getParameter<FloatParameter>("floatValue");
  ASSERT_TRUE((floatParam != 0));
  ASSERT_EQ(4.5, floatParam->value());
  stringParam = map.getParameter<StringParameter>("stringValue");
  ASSERT_EQ("test", stringParam->value());
  ASSERT_EQ("test", stringParam->value());

  // testing the serialization.

  std::stringstream ostream;
  ostream << "<config>" << std::endl;
  for (ParameterList::const_iterator pit = map.begin(); pit != map.end(); ++pit)
    ostream << "  " << *pit << std::endl;
  BooleanParameter bparam("boolValue", true);
  ostream << "  " << bparam << std::endl;
  ostream << "</config>" << std::endl;

//  std::cout << "ostream output: " << std::endl;
//  std::cout << ostream.str() << std::endl;

  ParameterList map2;
  XmlDocument doc2 = XmlDocument::fromData(ostream.str());
  XmlNode root2 = doc2.root();

  XmlNode::List children2 = root2.getChildren();
  for (uint32_t i = 0; i < children2.size(); ++i)
    map2.insert(Parameter::parseParameter(children2[i]));

  ASSERT_TRUE(map2.hasParam("intValue"));
  ASSERT_TRUE(map2.hasParam("floatValue"));
  ASSERT_TRUE(map2.hasParam("stringValue"));
  ASSERT_TRUE(map2.hasParam("boolValue"));

  ASSERT_EQ("integer", map2["intValue"].type());
  ASSERT_EQ("float", map2["floatValue"].type());
  ASSERT_EQ("string", map2["stringValue"].type());
  ASSERT_EQ("boolean", map2["boolValue"].type());

  int ivalue = map2["intValue"];
  float fvalue = map2["floatValue"];
  std::string svalue = map2.getValue<std::string>("stringValue");
  bool bvalue = map2["boolValue"];

  ASSERT_EQ(3, ivalue);
  ASSERT_NEAR(4.5, fvalue, 0.01);
  ASSERT_EQ("test", svalue);
  ASSERT_TRUE(bvalue);

}

TEST_F(ParameterTest, testRangeParameter)
{
  std::string nodeText = "<param name=\"rangeTest\" type=\"range\">"
      "<param name=\"begin\" type=\"integer\">0</param>"
      "<param name=\"end\" type=\"integer\">10</param>"
      "<param name=\"increment\" type=\"integer\">1</param>"
      "</param>";
  XmlDocument doc = XmlDocument::fromData(nodeText);
  XmlNode node = doc.root();

  RangeParameter param("rangeTest", node);
  const ParameterList& list = param.getParams();
  ASSERT_EQ(0, list.getValue<uint32_t>("begin"));
  ASSERT_EQ(10, list.getValue<uint32_t>("end"));
  ASSERT_EQ(1, list.getValue<uint32_t>("increment"));

  uint32_t i = 0;
  for (RangeParameter::const_iterator it = param.begin(); it != param.end(); ++it)
  {
    uint32_t val = *it;
    ASSERT_EQ(i++, val);
    ASSERT_EQ("rangeTest", it->name());
  }
  ASSERT_EQ(11, i);
}

}
