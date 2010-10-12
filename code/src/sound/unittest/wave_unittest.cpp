#include "onyx/base/base.h"
#include "gtest/gtest.h"
#include "onyx/sound/wave.h"
#include "QApplication"

namespace onyx
{
class WTest : public ::testing::Test {
public:
    void SetUp() {
        static char* name;
        static int FAKE_ARGC = 1;
        static char** FAKE_ARGV = &name;
        app_ = new QApplication(FAKE_ARGC, FAKE_ARGV);
    }

protected:
    QApplication* app_;
};
TEST_F(WTest, open)
{
    QString path = SAMPLE_ROOT;
    path += "/wav/short.wav";

    Sound sound;
    EXPECT_TRUE(playWaveFile(path, sound));

}
}
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

