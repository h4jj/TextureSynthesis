#include <iostream>
#include <fstream>
#include <Eigen/Core>
#include <vector>
#include <chrono>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using std::ifstream;
using std::cout;
using std::endl;
using Eigen::Vector3f;

typedef Vector3f Color;

Color** readImage(std::string filepath, int& width, int& height, int& bpp) {

    uint8_t *data = stbi_load(filepath.c_str(), &width, &height, &bpp, 3);
    if(!data) {
        cout << "Couldn't open file, exiting." << endl;
        exit(0);
    }

    const int COLS = width;
    const int ROWS = height;

    Color** colorData = nullptr;
    colorData = new Color*[ROWS];
    int counter = 0;

    for(int m{0}; m<ROWS; m++) {
        colorData[m] = new Color[COLS];
        for(int n{0}; n<COLS; n++) {
            colorData[m][n].x() = (int)data[counter];
            colorData[m][n].y() = (int)data[counter+1];
            colorData[m][n].z() = (int)data[counter+2];
            
            counter += 3;
        }
    }

    return colorData;
}

uint8_t* readFile(std::string filepath, int& width, int& height, int& bpp) {
    uint8_t *data = stbi_load(filepath.c_str(), &width, &height, &bpp, 3);
    if(!data) {
        cout << "Couldn't open file, exiting." << endl;
        exit(0);
    }

    return data;
}


void printColorData(Color** data, int height, int width) {
    std::ofstream file("comparison2.txt");
    int counter = 0;

    for(int m{0}; m<height; m++) {
        for(int n{0}; n<width; n++) {
            counter++;
            file << data[m][n] << " ";
        }
    }

    cout << "Colors: " << counter << endl;
}

void printCharData(uint8_t* data, int height, int width) {

    std::ofstream file("comparison1.txt");
    int counter = 0;
    for(int i{0}; i<width*height*3; i += 3) {
        Color color = {};
        counter++;
        color.x() = data[i];
        color.y() = data[i+1];
        color.z() = data[i+2];

        file << color << " ";
    }

    cout << "Colors: " << counter << endl;
}

void constructNeighborhoodVector(Color** data,int ROWS, int COLS, std::vector<std::vector<Color>>& nbVect, int nbGrid) {

    for(int m{0}; m<ROWS; m++) {
        for(int n{0}; n<COLS; n++) {
            std::vector<Color> neighborhood = {};

            for(int i{0}; i<nbGrid; i++) {
                for(int j{0}; j<nbGrid; j++) {
                    Color color = {};
                    color = data[m-i < 0 ? ROWS-i : m-i][n-j < 0 ? COLS-j : n-j];
                    neighborhood.push_back(color);      
                }
            }
            nbVect.push_back(neighborhood);
        }
    }
}

float l2VectorNorm(std::vector<Color>& nb1, std::vector<Color> nb2) {
    float t = 0;
    Color color = {};
    for(int i{1}; i<nb1.size(); i++) {
        t += pow((nb1[i].x() - nb2[i].x()),2);
        t += pow((nb1[i].y() - nb2[i].y()),2);
        t += pow((nb1[i].z() - nb2[i].z()),2);
        // cout << "nb1[i]: " << nb1[i] << endl;
        // cout << "nb2[i]: " << nb2[i] << endl;
        // cout << "t: " << t << endl;
    }
    return t;
}

std::vector<Color> constructNeighborhood(Color** data, int m, int n, int rows, int cols, int nbGrid) {
    std::vector<Color> neighborhood;

    for(int i{0}; i<nbGrid; i++) {
        for(int j{0}; j<nbGrid; j++) {
            Color color = {};
            color = data[m-i < 0 ? rows-i : m-i][n-j < 0 ? cols-j : n-j];
            neighborhood.push_back(color);      
        }
    }

    return neighborhood;
}

void compareNeighborhoods(std::vector<std::vector<Color>> ipNeighborhoodVect, std::vector<Color>& opNeighborhood) {
    
    float minDifference = 10000000000000000;
    
    for(auto nb : ipNeighborhoodVect) {
        
        float t = l2VectorNorm(nb, opNeighborhood);

        if(t < minDifference) {
            // cout << "t: " << t << endl;
            minDifference = t;
            opNeighborhood[0] = nb[0];
        }
    }
}

int main(int argc, char** argv) {

    auto start = std::chrono::steady_clock::now();
    int width, height, bpp;
    int width2, height2, bpp2;

    Color** inputColorData = readImage("../imgs/textureSC.png", width, height, bpp);
    uint8_t* outputData = readFile("../imgs/noiseTexture100.png", width2, height2, bpp2);
    Color** outputColorData = readImage("../imgs/noiseTexture100.png", width2, height2, bpp2);

    std::vector<std::vector<Color>> nbVectInput = {};
    std::vector<Color> nbVectOutput = {};
    
    // comparison 2D array can be generated before-hand
    // output image needs to construct neighborhood at each iteration
    // with newly assigned pixels or else no causality will appear
    // possible issues to look over are:
    // 1- neighborhood construction 
    // 2- l2norm function ? could be modified maybe
    constructNeighborhoodVector(inputColorData, height, width, nbVectInput, 7);

    // for each pixel in output image
    // 1- construct neighborhood
    // 2- compare neighborhood with all neighborhoods in input texture
    // 3- assign op pixel color based off smallest difference between neighborhoods using l2 norm
    // 4- repeat

    for(int m{0}; m<height2; m++) {
        for(int n{0}; n<width2; n++) {
            std::vector<Color> opNeighborhood = {};
            opNeighborhood = constructNeighborhood(outputColorData, m , n, height2, width2, 7);
            compareNeighborhoods(nbVectInput, opNeighborhood);
            outputColorData[m][n] = opNeighborhood[0];
            nbVectOutput.push_back(opNeighborhood[0]);
        }
    }

    unsigned char outputFinalResult[nbVectOutput.size() * 3];
    int counter = 0;

    for(auto& nb1 : nbVectOutput) {
        outputFinalResult[counter] = (unsigned char)nb1.x();
        outputFinalResult[counter + 1] = (unsigned char)nb1.y();
        outputFinalResult[counter + 2] = (unsigned char)nb1.z();
        
        counter += 3;
    }

    stbi_write_png("outputTest.png", width2, height2, 3, outputFinalResult,3*width2);

    auto end = std::chrono::steady_clock::now();
    cout << "Elapsed time in minutes: " << std::chrono::duration_cast<std::chrono::seconds>((end - start) / 60).count() << " minutes";

    return 0;
}