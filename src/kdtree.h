#pragma once
#include "GaussianPyramid.h"

struct kdnode {
    int median;
    std::vector<cv::Vec3b> inputVect;
    kdnode *leftChild = nullptr, *rightChild = nullptr;
    
    kdnode(int med, std::vector<cv::Vec3b> ipVect, kdnode *left, kdnode *right) : median(med), inputVect(ipVect), leftChild(left), rightChild(right) {}
};

class kdtree {
    
public:
    //training vectors as input
    void buildTree(std::vector<std::vector<cv::Vec3b>>);
    
    // best-first traversal obtaining nearest color
    std::vector<cv::Vec3b> query(std::vector<cv::Vec3b>);

    kdnode* root = nullptr;
};
