#include <fstream>
#include <Eigen/Core>
#include <vector>
#include <thread>
#include <chrono>
#include "GaussianPyramid.h"
#include "KDTree.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using std::ifstream;
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

void showLiveUpdate(unsigned char* tempFinalResult, Color** outputColorData, int width2, int height2 ) {
        int counter = 0;

        for(int i{0}; i<height2; i++) {
            for(int j{0}; j<width2; j++){
                
                tempFinalResult[counter] = (unsigned char)outputColorData[i][j].x();
                tempFinalResult[counter + 1] = (unsigned char)outputColorData[i][j].y();
                tempFinalResult[counter + 2] = (unsigned char)outputColorData[i][j].z();
            
                counter += 3;
            }
        }

        stbi_write_png("outputTest.png", width2, height2, 3, tempFinalResult,3*width2); 
}

void singleResolutionSynthesis() {

    auto start = std::chrono::steady_clock::now();
    int width, height, bpp;
    int width2, height2, bpp2;
    const int nbGrid = 9;

    Color** inputColorData = readImage("../imgs/texture1.png", width, height, bpp);
    uint8_t* outputData = readFile("../imgs/noiseTexture.png", width2, height2, bpp2);
    Color** outputColorData = readImage("../imgs/noiseTexture.png", width2, height2, bpp2);

    std::vector<std::vector<Color>> nbVectInput = {};
    std::vector<Color> nbVectOutput = {};

    constructNeighborhoodVector(inputColorData, height, width, nbVectInput, nbGrid);

    for(int m{0}; m<height2; m++) {
        for(int n{0}; n<width2; n++) {
            std::vector<Color> opNeighborhood = {};
            opNeighborhood = constructNeighborhood(outputColorData, m , n, height2, width2, nbGrid);
            compareNeighborhoods(nbVectInput, opNeighborhood);
            outputColorData[m][n] = opNeighborhood[0];
            nbVectOutput.push_back(opNeighborhood[0]);
        }

        // unsigned char tempFinalResult[width2 * height2 * 3] = {};
        // showLiveUpdate(tempFinalResult, outputColorData, width2, height2);       
    }

    unsigned char outputFinalResult[nbVectOutput.size() * 3] = {};
    int counter = 0;

    for(auto& nb1 : nbVectOutput) {
        outputFinalResult[counter] = (unsigned char)nb1.x();
        outputFinalResult[counter + 1] = (unsigned char)nb1.y();
        outputFinalResult[counter + 2] = (unsigned char)nb1.z();
        
        counter += 3;
    }

    auto end = std::chrono::steady_clock::now();
    cout << "Elapsed time in seconds: " << std::chrono::duration_cast<std::chrono::seconds>((end - start)).count() << " seconds" << std::endl;

    stbi_write_png("../out/outputTest1.png", width2, height2, 3, outputFinalResult,3*width2);

}

enum class Mode {PARENT, CHILD};

std::vector<cv::Vec3b> getSingleLevelNeighborhood(std::vector<Mat>& pyramid, int level, int x, int y, Mode mode, int nbGrid) {

    int minVal = -floor(nbGrid/2);
    int maxVal = -minVal;

    std::vector<cv::Vec3b> neighborhood = {};
    Mat currentLvl = pyramid[level];

    if(mode == Mode::PARENT) {

        x = floor(x/2);
        y = floor(y/2);

        for(int i{minVal+1}; i<=maxVal-1; i++) {
            for(int j{minVal+1}; j<=maxVal-1; j++) {
                if(y+i < 0) {
                    y = currentLvl.cols + i;
                }   
                else if(y+i >= currentLvl.rows) {
                    y = -1 + i;
                }
                
                if(x+j < 0) {
                    x = currentLvl.cols + j;
                }
                else if(x+j >= currentLvl.cols) {
                    x = -1 + j;
                }
                    
                neighborhood.push_back(currentLvl.at<cv::Vec3b>(y+i,x+j));
                    
            }
        }
    }
    else {

        for(int k{0}; k<=maxVal; k++) {
            if(x-k < 0) {
                x = currentLvl.cols - k;
            }
            
            neighborhood.push_back(currentLvl.at<cv::Vec3b>(y,x-k));
        }

        for(int i{-2}; i<=-1; i++) {
            for(int j{minVal}; j<=maxVal; j++) {
                if(y+i < 0) {
                    y = currentLvl.rows + i;
                }   
                
                if(x+j < 0) {
                    x = currentLvl.cols + j;
                }
                else if(x+j >= currentLvl.cols) {
                    x = -1 + j;
                }
                
                neighborhood.push_back(currentLvl.at<cv::Vec3b>(y+i,x+j));
                
            }
        }
    }
    return neighborhood;       
}


std::vector<cv::Vec3b> getNeighborhood(std::vector<Mat>& pyramid, int level, int x, int y, int nbGrid) {
    auto nChild = getSingleLevelNeighborhood(pyramid, level, x, y, Mode::CHILD, nbGrid);

    if(level-1 >= 0) {
        auto nParent = getSingleLevelNeighborhood(pyramid,level-1,x,y, Mode::PARENT, nbGrid);
        for(auto& item : nParent) {
            nChild.push_back(item);
        }
    }
    else {
        for(int i{0}; i<9; i++) {
            nChild.push_back(cv::Vec3b(0,0,0)); 
        }
        
    }

    return nChild;
}

float l2VectorNormMultiRes(std::vector<cv::Vec3b>& nb1, std::vector<cv::Vec3b>& nb2) {
    float t = 0;
    Color color = {};
    for(int i{1}; i<nb1.size(); i++) {
        t += pow(nb1[i][0] - nb2[i][0], 2);
        t += pow(nb1[i][1] - nb2[i][1], 2);
        t += pow(nb1[i][2] - nb2[i][2], 2);
    }
    
    return t;
}

float l2VectorNormMultiResTSVQ(std::vector<double>& nb1, std::vector<double>& nb2) {
    float t = 0;
    Color color = {};
    for(int i{3}; i<nb1.size(); i++) {
        t += pow(nb1[i] - nb2[i], 2);
    }
    
    return t;
}

cv::Vec3b compareWithSearchBook(std::vector<cv::Vec3b> neighborhood, std::vector<std::vector<cv::Vec3b>> SearchBook) {
    float minDifference = 10000000000000000;
    cv::Vec3b color = {};
    
    for(auto inputVect : SearchBook) {
        
        float t = l2VectorNormMultiRes(inputVect, neighborhood);

        if(t < minDifference) {
            minDifference = t;
            color = inputVect[0];
        }
    }

    return color;
}

cv::Vec3b compareWithSearchBookTSVQ(std::vector<double> neighborhood, KDTree tree) {
    cv::Vec3b color = {};
    
    std::vector<double> colorVect = tree.nearest_point(neighborhood);

    color[0] = (unsigned char)colorVect[0];
    color[1] = (unsigned char)colorVect[1];
    color[2] = (unsigned char)colorVect[2];

    return color;
}

cv::Vec3b FindBestMatch(GaussianPyramid& Ga, GaussianPyramid& Gs, int level, int x, int y, std::vector<std::vector<cv::Vec3b>> SearchBook, int nbGrid) {
    cv::Vec3b bestMatch = {};
    auto neighborhood = getNeighborhood(Gs.pyramid, level, x, y, nbGrid);

    bestMatch = compareWithSearchBook(neighborhood, SearchBook);

    return bestMatch;
}

std::ostream& operator<<(std::ostream& out,const std::vector<double> vect) {
    return out << "[ " << vect[0] << ", " << vect[1] << " ," << vect[2] << " ]" << std::endl;
}

cv::Vec3b FindBestMatchTSVQ(GaussianPyramid& Ga, GaussianPyramid& Gs, int level, KDTree tree, int x, int y, int nbGrid) {
    cv::Vec3b bestMatch = {};
    auto neighborhood = getNeighborhood(Gs.pyramid, level, x, y, nbGrid);
    
    std::vector<double> tempNeighborhood = {};

    for(const auto color : neighborhood) {
        tempNeighborhood.push_back((double)color[0]);
        tempNeighborhood.push_back((double)color[1]);
        tempNeighborhood.push_back((double)color[2]);
        
    }

    // std::cout << "color: " << tempNeighborhood << std::endl;

    bestMatch = compareWithSearchBookTSVQ(tempNeighborhood, tree);
    

    return bestMatch;
}

std::vector<cv::Vec3b> getSingleLevelNeighborhoodSB(std::vector<Mat>& pyramid, int level, int x, int y, Mode mode, int nbGrid) {

    int minVal = -floor(nbGrid/2);
    int maxVal = -minVal;

    std::vector<cv::Vec3b> neighborhood = {};
    Mat currentLvl = pyramid[level];

    if(mode == Mode::PARENT) {

        x = floor(x/2);
        y = floor(y/2);

        for(int i{minVal+1}; i<=maxVal-1; i++) {
            for(int j{minVal+1}; j<=maxVal-1; j++) {
                if(y+i < 0 || y+i >= currentLvl.rows) {
                    neighborhood.push_back(cv::Vec3b(0,0,0));
                }   
                else {
                    if(x+j < 0 || x+j >= currentLvl.cols) {
                        neighborhood.push_back(cv::Vec3b(0,0,0));
                    }
                    else {
                        neighborhood.push_back(currentLvl.at<cv::Vec3b>(y+i,x+j));
                    }
                }
            }
        }
    }
    else {

        for(int k{0}; k<=maxVal; k++) {
            if(x-k < 0) {
                neighborhood.push_back(cv::Vec3b(0,0,0));
            }
            else {
                neighborhood.push_back(currentLvl.at<cv::Vec3b>(y,x-k));
            }
        }

        for(int i{-2}; i<=-1; i++) {
            for(int j{minVal}; j<=maxVal; j++) {
                if(y+i < 0) {
                    neighborhood.push_back(cv::Vec3b(0,0,0));
                }   
                else {
                    if(x+j < 0 || x+j >= currentLvl.cols) {
                        neighborhood.push_back(cv::Vec3b(0,0,0));
                    }
                    else {
                        neighborhood.push_back(currentLvl.at<cv::Vec3b>(y+i,x+j));
                    }
                }
            }
        }
        
    }

    return neighborhood;
}

std::vector<cv::Vec3b> getNeighborhoodSearchBook(std::vector<Mat>& pyramid, int level, int x, int y, int nbGrid) {
    auto nChild = getSingleLevelNeighborhoodSB(pyramid, level, x, y, Mode::CHILD, nbGrid);

    if(level-1 >= 0) {
        auto nParent = getSingleLevelNeighborhoodSB(pyramid,level-1,x,y, Mode::PARENT, nbGrid);
        for(auto& item : nParent) {
            nChild.push_back(item);
        }
    }
    else {
        for(int i{0}; i<9; i++) {
            nChild.push_back(cv::Vec3b(0,0,0)); 
        }
        
    }

    return nChild;
}

void buildSearchBook(std::vector<std::vector<cv::Vec3b>>& searchbook, std::vector<Mat> pyramid, int pyramidLvl, int nbGrid) {
    
    Mat currentLevel = pyramid[pyramidLvl];
    

    for(int j{0}; j<currentLevel.rows; j++) {
        for(int i{0}; i<currentLevel.cols; i++) {
            std::vector<cv::Vec3b> neighborhood = {};
            neighborhood = getNeighborhoodSearchBook(pyramid, pyramidLvl, i, j, nbGrid);
            searchbook.push_back(neighborhood);
        }
    }

    return;
}

void buildSearchBookFlattened(std::vector<std::vector<double>>& searchbook, std::vector<Mat> pyramid, int pyramidLvl, int nbGrid) {
    
    Mat currentLevel = pyramid[pyramidLvl];
    

    for(int j{0}; j<currentLevel.rows; j++) {
        for(int i{0}; i<currentLevel.cols; i++) {
            std::vector<cv::Vec3b> neighborhood = {};
            std::vector<double> tempNeighborhood = {};
            
            neighborhood = getNeighborhoodSearchBook(pyramid, pyramidLvl, i, j, nbGrid);
            for(const auto color : neighborhood) {
                tempNeighborhood.push_back((double)color[0]);
                tempNeighborhood.push_back((double)color[1]);
                tempNeighborhood.push_back((double)color[2]);
            }
            searchbook.push_back(tempNeighborhood);
        }
    }

    return;
}

void multiResolutionSynthesis() {
    double searching_time = 0.0f;

    const char* filename1 = "../imgs/texture1.png";
    const char* filename2 = "../imgs/noiseTexture.png";
    const int nbGrid = 5;
    
    Mat finalImage;
    // Loads an image
    Mat src = imread( cv::samples::findFile( filename1 ) );
    Mat target = imread( cv::samples::findFile( filename2 ) );
    // Check if image is loaded fine
    if(src.empty()){
        printf(" Error opening image\n");
        printf(" Program Arguments: [image_name -- default textureSC.png] \n");
        return;
    }
    if(target.empty()){
        printf(" Error opening image\n");
        printf(" Program Arguments: [image_name -- default noiseTexture.png] \n");
        return;
    }

    GaussianPyramid Ga = GaussianPyramid(src, 4);
    GaussianPyramid Gs = GaussianPyramid(target, 4);

    Ga.buildPyramid(Ga.getOriginalImg());
    Gs.buildPyramid(Gs.getOriginalImg());

    int pyramidLvl = 0;

    for(auto& level : Gs.pyramid) {
        
        std::vector<std::vector<cv::Vec3b>> SearchBook;
        buildSearchBook(SearchBook, Ga.pyramid, pyramidLvl, nbGrid);

        auto start2 = std::chrono::steady_clock::now();
        
        cout << "Pyramid Level: " << pyramidLvl << endl;
        for(int j{0}; j<level.rows; j++) {
            for(int i{0}; i<level.cols; i++) {
                cv::Vec3b C = FindBestMatch(Ga,Gs,pyramidLvl, i, j, SearchBook, nbGrid);
                cv::Vec3b& color = level.at<cv::Vec3b>(j,i);
                // cout << "before" << color << ' ';
                color[0] = C[0];
                color[1] = C[1];
                color[2] = C[2];

                level.at<cv::Vec3b>(j,i) = color;
            }
            
        }        

        finalImage = Gs.pyramid[pyramidLvl];
        pyramidLvl++;

        auto end2 = std::chrono::steady_clock::now();

        searching_time += std::chrono::duration_cast<std::chrono::seconds>((end2 - start2)).count();
    }

    cout << "Elapsed searching time in seconds: " << searching_time << " seconds" << std::endl;
    imwrite("../out/outputTest2.png", finalImage);
}

void multiResolutionSynthesisTSVQ() {
    double synthesis_time = 0.0f, searching_time = 0.0f;
    // auto start = std::chrono::steady_clock::now();
    const char* filename1 = "../imgs/texture1.png";
    const char* filename2 = "../imgs/noiseTexture.png";
    const int nbGrid = 5;
    // Loads an image
    Mat src = imread( cv::samples::findFile( filename1 ) );
    Mat target = imread( cv::samples::findFile( filename2 ) );
    Mat finalImage;
    // Check if image is loaded fine
    if(src.empty()){
        printf(" Error opening image\n");
        printf(" Program Arguments: [image_name -- default textureSC.png] \n");
        return;
    }
    if(target.empty()){
        printf(" Error opening image\n");
        printf(" Program Arguments: [image_name -- default noiseTexture.png] \n");
        return;
    }

    GaussianPyramid Ga = GaussianPyramid(src, 4);
    GaussianPyramid Gs = GaussianPyramid(target, 4);

    Ga.buildPyramid(Ga.getOriginalImg());
    Gs.buildPyramid(Gs.getOriginalImg());

    int pyramidLvl = 0;

    for(auto& level : Gs.pyramid) {
        
        std::vector<std::vector<double>> SearchBook;
        buildSearchBookFlattened(SearchBook, Ga.pyramid, pyramidLvl, nbGrid);
        auto start1 = std::chrono::steady_clock::now();
        KDTree tree(SearchBook);        
        auto end1 = std::chrono::steady_clock::now();
        synthesis_time += std::chrono::duration_cast<std::chrono::seconds>((end1 - start1)).count();

        auto start2 = std::chrono::steady_clock::now();

        cout << "Pyramid Level: " << pyramidLvl << endl;
        for(int j{0}; j<level.rows; j++) {
            for(int i{0}; i<level.cols; i++) {
                cv::Vec3b C = FindBestMatchTSVQ(Ga,Gs,pyramidLvl, tree, i, j, nbGrid);
                cv::Vec3b& color = level.at<cv::Vec3b>(j,i);
                // cout << "before" << color << ' ';
                color[0] = C[0];
                color[1] = C[1];
                color[2] = C[2];

                level.at<cv::Vec3b>(j,i) = color;
            }
            // if(pyramidLvl != Gs.pyramid.size() -1) {
            //     std::this_thread::sleep_for(std::chrono::milliseconds(50));
            // }

            // Mat finalImage = Gs.pyramid[pyramidLvl];
            // imwrite("finalImageTSVQ2.png", finalImage);

        }
        finalImage = Gs.pyramid[pyramidLvl];        

        pyramidLvl++;

        auto end2 = std::chrono::steady_clock::now();

        searching_time += std::chrono::duration_cast<std::chrono::seconds>((end2 - start2)).count();
    }

    cout << "Elapsed training time in seconds: " << synthesis_time << " seconds" << std::endl;
    cout << "Elapsed searching time in seconds: " << searching_time << " seconds" << std::endl;
    imwrite("../out/outputTest3.png", finalImage);
}


int main(int argc, char** argv) {

    singleResolutionSynthesis(); // 9x9
    multiResolutionSynthesis(); // 5x5
    multiResolutionSynthesisTSVQ(); // 5x5

    return EXIT_SUCCESS;
}

