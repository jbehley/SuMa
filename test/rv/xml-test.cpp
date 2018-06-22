#include <gtest/gtest.h>
#include <rv/XmlDocument.h>

TEST(XmlTest, parse)
{
  // parse stream is all we need to test.
  std::string xml_doc;
  xml_doc += "<root>\n";
  xml_doc += "  <element name=\"1337\" a=\"1234.5678\">\n";
  xml_doc += "    <inner_element name=\"inner node\" value=\"1337.1337\"/>\n";
  xml_doc += "  </element>\n";
  xml_doc += "  <!-- some comment -->\n";
  xml_doc += "  <a>\n";
  xml_doc += "   <nested>\n";
  xml_doc += "   </nested><element>the content!</element>\n";
  xml_doc += "  </a>\n";
  xml_doc += "  <element name=\"waldo\" />\n";
  xml_doc += "</root>\n <!-- comment after root? -->\n\n  ";

  //std::cout << xml_doc << "" << std::endl;

  //std::cout << "######################" << std::endl;

  std::stringstream stream(xml_doc);

  rv::XmlDocument doc = rv::XmlDocument::fromStream(stream);

  const rv::XmlNode* node;

  ASSERT_TRUE(doc.root().isValid());
  ASSERT_EQ("root", doc.root().getName());
  ASSERT_EQ(3, doc.root().getChildren().size());

  node = &doc.root().getChildren()[0];
  ASSERT_EQ("element", node->getName());
  ASSERT_TRUE(node->hasAttribute("name"));
  ASSERT_FALSE(node->hasAttribute("Foobar"));
  ASSERT_EQ("1337", node->getAttribute("name"));
  ASSERT_TRUE(node->hasAttribute("a"));
  ASSERT_EQ("1234.5678", node->getAttribute("a"));
  ASSERT_EQ(1, node->getChildren().size());
  node = &node->getChildren()[0];
  ASSERT_EQ("inner_element", node->getName());
  ASSERT_TRUE(node->hasAttribute("name"));
  ASSERT_EQ("inner node", node->getAttribute("name"));
  ASSERT_TRUE(node->hasAttribute("value"));
  ASSERT_EQ("1337.1337", node->getAttribute("value"));

  node = &doc.root().getChildren()[1];
  ASSERT_EQ("a", node->getName());
  ASSERT_EQ(2, node->getChildren().size());
  ASSERT_EQ("element", node->getChildren()[1].getName());
  ASSERT_FALSE(node->getChildren()[1].hasAttribute("missing"));
  ASSERT_EQ("the content!", node->getChildren()[1].getContent());
  ASSERT_TRUE(doc.root().getChildren()[0].hasAttribute("name"));

  // test accessors:
  ASSERT_TRUE(doc.root().getChild("element").isValid());
  ASSERT_EQ("1337", doc.root().getChild("element").getAttribute("name"));
  ASSERT_FALSE(doc.root().getChild("missing").isValid());

  ASSERT_TRUE(doc.root().getChildWith("name", "waldo").isValid());
  ASSERT_FALSE(doc.root().getChildWith("name", "blabla").isValid());
  ASSERT_EQ("element", doc.root().getChildWith("name", "waldo").getName());
}

TEST(XmlTest, partial_parse)
{

  std::string xml_doc;
  xml_doc += "<root>\n";
  xml_doc += "  <element name=\"1337\" a=\"1234.5678\">\n";
  xml_doc += "    <inner_element name=\"inner node\" value=\"1337.1337\"/>\n";

  ASSERT_THROW(rv::XmlDocument::fromData(xml_doc), rv::XmlError);
  rv::XmlDocument doc = rv::XmlDocument::fromData(xml_doc, true);
  ASSERT_TRUE(doc.root().isValid());
  ASSERT_EQ(1, doc.root().getChildren().size());

  const rv::XmlNode* node = &doc.root().getChildren()[0];
  ASSERT_EQ("element", node->getName());
  ASSERT_TRUE(node->hasAttribute("name"));
  ASSERT_FALSE(node->hasAttribute("Foobar"));
  ASSERT_EQ("1337", node->getAttribute("name"));
  ASSERT_TRUE(node->hasAttribute("a"));
  ASSERT_EQ("1234.5678", node->getAttribute("a"));

  ASSERT_EQ(1, node->getChildren().size());
  node = &node->getChildren()[0];
  ASSERT_EQ("inner_element", node->getName());
  ASSERT_TRUE(node->hasAttribute("name"));
  ASSERT_EQ("inner node", node->getAttribute("name"));
  ASSERT_TRUE(node->hasAttribute("value"));
  ASSERT_EQ("1337.1337", node->getAttribute("value"));

}
