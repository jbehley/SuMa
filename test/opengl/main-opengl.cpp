#include <glow/glbase.h>
#include <gtest/gtest.h>

#include <glow/GlCapabilities.h>
#include <glow/GlState.h>
#include <glow/util/X11OffscreenContext.h>

int main(int argc, char **argv) {
  glow::X11OffscreenContext ctx;

  glow::inititializeGLEW();
  //  ASSERT_TRUE(ctx.create());
  if (argc > 1 && std::string(argv[1]) == "info") {
    std::cout << "Device's OpenGL Capabilities: \n" << std::endl;
    std::cout << glow::GlCapabilities::getInstance() << std::endl;

    std::cout << "Device's current OpenGL state: \n" << std::endl;
    std::cout << glow::GlState::queryAll() << std::endl;
  }
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
