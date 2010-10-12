#include "onyx/base/base.h"
#include "gtest/gtest.h"
#include "onyx/sound/wave.h"
#include "onyx/sound/async_player.h"
#include <QFile>
namespace
{
class ASPTest : public ::testing::Test {
};

TEST_F(ASPTest,instance)
{
    QString path = SAMPLE_ROOT;
    path += "/wav/short.wav";

    Sound sound;
    sound.setVolume(90);
    sound.setVolume(90);

    QFile file(path);
    EXPECT_TRUE(file.open(QIODevice::ReadOnly));
    QByteArray data = file.readAll();
    for(int i = 0; i < 100; ++i)
    {
        EXPECT_TRUE(AsyncPlayer::instance().play(sound, data));
    }
}
}
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

