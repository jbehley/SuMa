#include <gtest/gtest.h>
#include <rv/FileUtil.h>

#include <cstdio>
#include <fstream>

using namespace rv;

TEST(FileutilTestcase, testExtists)
{
  std::ofstream out("test.txt");
  out << "test123";
  out.close();

  ASSERT_EQ(true, FileUtil::exists("./test.txt"));
  ASSERT_EQ(false, FileUtil::exists("/does/not/exists"));

  std::remove("test.txt");

  ASSERT_EQ(false, FileUtil::exists("./test.txt"));

}

TEST(FileutilTestcase, testBaseName)
{
  ASSERT_EQ("test.txt", FileUtil::baseName("./test.txt"));
  ASSERT_EQ("exists", FileUtil::baseName("/does/not/exists"));
  ASSERT_EQ(".", FileUtil::baseName("/does/not/exists/"));
  ASSERT_EQ("complicated.example.tar.gz", FileUtil::baseName("/very/very/very/complicated.example.tar.gz"));
}

TEST(FileutilTestcase, testDirName)
{
  ASSERT_EQ(".", FileUtil::dirName("./test.txt"));
  ASSERT_EQ("/does/not", FileUtil::dirName("/does/not/exists"));
  ASSERT_EQ("/does/not/exists", FileUtil::dirName("/does/not/exists/"));
  ASSERT_EQ("/very/very/very", FileUtil::dirName("/very/very/very/complicated.example.tar.gz"));
}

TEST(FileutilTestcase, testStripExtension)
{
  ASSERT_EQ("/test/directory/.", FileUtil::stripExtension("/test/directory/."));
  ASSERT_EQ("/test/directory/..", FileUtil::stripExtension("/test/directory/.."));

  ASSERT_EQ("./test", FileUtil::stripExtension("./test.txt"));
  ASSERT_EQ("/does/not/exists", FileUtil::stripExtension("/does/not/exists"));
  ASSERT_EQ("/very/very/very/complicated.example.tar", FileUtil::stripExtension("/very/very/very/complicated.example.tar.gz"));

  ASSERT_EQ("./test.txt", FileUtil::stripExtension("./test.txt", 0));
  ASSERT_EQ("/does/not/exists", FileUtil::stripExtension("/does/not/exists", 0));
  ASSERT_EQ("/very/very/very.directory/complicated.example.tar.gz", FileUtil::stripExtension("/very/very/very.directory/complicated.example.tar.gz", 0));


  ASSERT_EQ("./test", FileUtil::stripExtension("./test.txt", 2));
  ASSERT_EQ("/does/not/exists", FileUtil::stripExtension("/does/not/exists", 2));
  ASSERT_EQ("/very/very/very.directory/complicated.example", FileUtil::stripExtension("/very/very/very.directory/complicated.example.tar.gz", 2));
  ASSERT_EQ("/very/very/very.directory/complicated", FileUtil::stripExtension("/very/very/very.directory/complicated.example.tar.gz", 3));
  ASSERT_EQ("/very/very/very.directory/complicated", FileUtil::stripExtension("/very/very/very.directory/complicated.example.tar.gz", 4));
  ASSERT_EQ("/very/very/very.directory/complicated", FileUtil::stripExtension("/very/very/very.directory/complicated.example.tar.gz", 120));
}

TEST(FileutilTestcase, testExtension)
{
  ASSERT_EQ("", FileUtil::extension("/test/directory/."));
  ASSERT_EQ("", FileUtil::extension("/test/directory/.."));

  ASSERT_EQ(".txt", FileUtil::extension("./test.txt"));
  ASSERT_EQ("", FileUtil::extension("/does/not/exists"));
  ASSERT_EQ(".gz", FileUtil::extension("/very/very/very/complicated.example.tar.gz"));

  ASSERT_EQ("", FileUtil::extension("./test.txt", 0));
  ASSERT_EQ("", FileUtil::extension("/does/not/exists", 0));
  ASSERT_EQ("", FileUtil::extension("/very/very/very/complicated.example.tar.gz", 0));


  ASSERT_EQ(".txt", FileUtil::extension("./test.txt", 2));
  ASSERT_EQ("", FileUtil::extension("/does/not/exists", 2));
  ASSERT_EQ(".tar.gz", FileUtil::extension("/very/very/very/complicated.example.tar.gz", 2));
  ASSERT_EQ(".example.tar.gz", FileUtil::extension("/very/very/very/complicated.example.tar.gz", 3));
  ASSERT_EQ(".example.tar.gz", FileUtil::extension("/very/very/very/complicated.example.tar.gz", 4));
  ASSERT_EQ(".example.tar.gz", FileUtil::extension("/very/very/very/complicated.example.tar.gz", 120));
}
