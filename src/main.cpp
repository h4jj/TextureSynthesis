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

            // Color col0 = {}, col1 = {}, col2 = {}, col3 = {}, col4 = {}, col5 = {}, col6 = {}, col7 = {}, col8 = {}, col9 = {}, col10 = {}, col11 = {}, col12 = {}, col13 = {}, col14 = {}, col15 = {}, col16 = {}, col17 = {};
            // col0 = data[m][n];
            // col1 = data[m][n-1 < 0 ? COLS-1 : n-1];
            // col2 = data[m][n-2 < 0 ? COLS-2 : n-2];
            // col3 = data[m][n-3 < 0 ? COLS-3 : n-3];

            // col4 = data[m-1 < 0 ? ROWS-1 : m-1][n+3 >= COLS ? 2 : n+3];
            // col5 = data[m-1 < 0 ? ROWS-1 : m-1][n+2 >= COLS ? 1 : n+2];
            // col6 = data[m-1 < 0 ? ROWS-1 : m-1][n+1 >= COLS ? 0 : n+1];
            // col7 = data[m-1 < 0 ? ROWS-1 : m-1][n];
            // col8 = data[m-1 < 0 ? ROWS-1 : m-1][n-1 < 0 ? COLS-1 : n-1];
            // col9 = data[m-1 < 0 ? ROWS-1 : m-1][n-2 < 0 ? COLS-2 : n-2];
            // col10 = data[m-1 < 0 ? ROWS-1 : m-1][n-3 < 0 ? COLS-3 : n-3];

            
            // col11 = data[m-2 < 0 ? ROWS-2 : m-2][n+3 >= COLS ? 2 : n+3];
            // col12 = data[m-2 < 0 ? ROWS-2 : m-2][n+2 >= COLS ? 1 : n+2];
            // col13 = data[m-2 < 0 ? ROWS-2 : m-2][n+1 >= COLS ? 0 : n+1];
            // col14 = data[m-2 < 0 ? ROWS-2 : m-2][n]; 
            // col15 = data[m-2 < 0 ? ROWS-2 : m-2][n-1 < 0 ? COLS-1 : n-1];
            // col16 = data[m-2 < 0 ? ROWS-2 : m-2][n-2 < 0 ? COLS-2 : n-2];
            // col17 = data[m-2 < 0 ? ROWS-2 : m-2][n-3 < 0 ? COLS-3 : n-3];

            // neighborhood.push_back(col0);
            // neighborhood.push_back(col1);
            // neighborhood.push_back(col2);
            // neighborhood.push_back(col3);
            // neighborhood.push_back(col4);
            // neighborhood.push_back(col5);
            // neighborhood.push_back(col6);
            // neighborhood.push_back(col7);
            // neighborhood.push_back(col8);
            // neighborhood.push_back(col9);
            // neighborhood.push_back(col10);
            // neighborhood.push_back(col11);
            // neighborhood.push_back(col12);
            // neighborhood.push_back(col13);
            // neighborhood.push_back(col14);
            // neighborhood.push_back(col15);
            // neighborhood.push_back(col16);
            // neighborhood.push_back(col17);

            int minVal = -floor(nbGrid/2);
            int maxVal = -minVal;

            for(int k{0}; k<=maxVal; k++) {
                Color color = {};
                color = data[m][n-k < 0 ? COLS-k : n-k];

                neighborhood.push_back(color);
            }

            for(int i{1}; i<=2; i++) {
                for(int j{minVal}; j<=maxVal; j++) {
                    Color color = {};
                    if(j < 0){
                        color = data[m-i < 0 ? ROWS-i : m-i][n+j < 0 ? COLS+j : n+j];   
                    }
                    else if(j > 0) {
                        color = data[m-i < 0 ? ROWS-i : m-i][n+j >= COLS ? j-1 : n+j];
                    }
                    else {
                        color = data[m-i < 0 ? ROWS-i : m-i][n];
                    }

                    neighborhood.push_back(color);
                }
            }

            nbVect.push_back(neighborhood);
        }
    }
}


float l2VectorNorm(std::vector<Color>& nb1, std::vector<Color>& nb2) {
    float t = 0;
    Color color = {};
    for(int i{1}; i<nb1.size(); i++) {
        t += pow(nb1[i].x() - nb2[i].x(), 2);
        t += pow(nb1[i].y() - nb2[i].y(), 2);
        t += pow(nb1[i].z() - nb2[i].z(), 2);
    }
    
    return t;
}

std::vector<Color> constructNeighborhood(Color** data, int m, int n, int ROWS, int COLS, int nbGrid) {
    std::vector<Color> neighborhood = {};

    int minVal = -floor(nbGrid/2);
    int maxVal = -minVal;

 
    for(int k{0}; k<=maxVal; k++) {
        Color color = {};
        color = data[m][n-k < 0 ? COLS-k : n-k];

        neighborhood.push_back(color);
    }

    for(int i{1}; i<=2; i++) {
        for(int j{minVal}; j<=maxVal; j++) {
            Color color = {};
            if(j < 0){
                color = data[m-i < 0 ? ROWS-i : m-i][n+j < 0 ? COLS+j : n+j];   
            }
            else if(j > 0) {
                color = data[m-i < 0 ? ROWS-i : m-i][n+j >= COLS ? j-1 : n+j];
            }
            else {
                color = data[m-i < 0 ? ROWS-i : m-i][n];
            }

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
            minDifference = t;
            opNeighborhood[0] = nb[0];
        }
    }
}

void singleResolutionSynthesis() {

    auto start = std::chrono::steady_clock::now();
    int width, height, bpp;
    int width2, height2, bpp2;

    Color** inputColorData = readImage("../imgs/textureSC.png", width, height, bpp);
    uint8_t* outputData = readFile("../imgs/noiseTexture.png", width2, height2, bpp2);
    Color** outputColorData = readImage("../imgs/noiseTexture.png", width2, height2, bpp2);

    std::vector<std::vector<Color>> nbVectInput = {};
    std::vector<Color> nbVectOutput = {};

    constructNeighborhoodVector(inputColorData, height, width, nbVectInput, 9);

    for(int m{0}; m<height2; m++) {
        for(int n{0}; n<width2; n++) {
            std::vector<Color> opNeighborhood = {};
            opNeighborhood = constructNeighborhood(outputColorData, m , n, height2, width2, 9);
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

}

int main(int argc, char** argv) {

    singleResolutionSynthesis();

    return 0;
}