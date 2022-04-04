#include "kdtree.h"

void kdtree::buildTree(std::vector<std::vector<cv::Vec3b>> trainingVectors) {

  // 1 - find median
  
  unsigned int r = 0, g = 0, b = 0;
  std::cout << "SIZE: " << trainingVectors.size() << std::endl;
  for(const auto vect : trainingVectors) {
    std::cout << vect[1] << std::endl;
    r += vect[5][0];
    g += vect[5][1];
    b += vect[5][2];
  }

  r /= trainingVectors.size();
  g /= trainingVectors.size();
  b /= trainingVectors.size();

  std::cout << std::endl << std::endl << std::endl;
  cv::Vec3b color(r,g,b);
  std::cout << "Color mean:" << color << std::endl;


}