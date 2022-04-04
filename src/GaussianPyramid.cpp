#include "GaussianPyramid.h"

void GaussianPyramid::buildPyramid(Mat img) {
    pyramid.push_back(img);

    if(levels != 0) {
        for(int i{1}; i < levels; i++) {
            pyrDown( img, img, Size( img.cols/2, img.rows/2 ) );
            pyramid.push_back(img);
        }
    }

    std::reverse(pyramid.begin(), pyramid.end());
}

void GaussianPyramid::reconPyramid() {
    
}

void GaussianPyramid::printPyramid() {
    
    int counter = 1;

    for(const auto level : pyramid) {
        
        cout << "level " << counter << ": " << level.rows << "x" << level.cols << endl;
        counter++;
    }
}

void GaussianPyramid::showEachStage() {

    for(const auto level : pyramid) {
        cout << "rows: " << level.rows << " cols: " << level.cols << endl;
        imshow( window_name, level );
        char c = (char)waitKey(0);
        if( c == 27 )
        { continue; }
    }
}

void GaussianPyramid::showSingleStage(Mat level) {
    cout << "rows: " << level.rows << " cols: " << level.cols << endl;
    imshow( window_name, level );
    while((char)waitKey(0) != 27);
}