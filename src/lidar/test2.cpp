#include "LidarManager.h"

int main(void)
{
  LidarManager l;
  l.setUpLidar();
  l.start();
  l.run();
  l.stop();
}
