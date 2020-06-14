
#include "sc.h"

using namespace cv;
using namespace std;




bool seam_carving(Mat& in_image, int new_width, int new_height, Mat& out_image){

    // some sanity checks
    // Check 1 -> new_width <= in_image.cols
    if(new_width>in_image.cols){
        cout<<"Invalid request!!! new_width has to be smaller than the current size!"<<endl;
        return false;
    }
    if(new_height>in_image.rows){
        cout<<"Invalid request!!! ne_height has to be smaller than the current size!"<<endl;
        return false;
    }
    
    if(new_width<=0){
        cout<<"Invalid request!!! new_width has to be positive!"<<endl;
        return false;

    }
    
    if(new_height<=0){
        cout<<"Invalid request!!! new_height has to be positive!"<<endl;
        return false;
        
    }

    
//   out_image = gradientMagnitude(in_image);
//   return true;
    return seam_carving_trivial(in_image, new_width, new_height, out_image);
}


// seam carves by removing trivial seams
bool seam_carving_trivial(Mat& in_image, int new_width, int new_height, Mat& out_image){

    Mat iimage = in_image.clone();
    Mat oimage = in_image.clone();

    while(iimage.rows!=new_height || iimage.cols!=new_width){
    	int numIterations = (-new_height + iimage.rows) + (-new_width + iimage.cols);
    	cout << "Iterations left: " << numIterations-- << endl;
        // horizontal seam if needed

        if(iimage.rows>new_height){
        	reduceSeam(iimage, oimage, false);
            iimage = oimage.clone();
        }
        
        if(iimage.cols>new_width){
        	reduceSeam(iimage, oimage, true);
            iimage = oimage.clone();
        }
    }
    
    out_image = oimage.clone();
    return true;
}

// horizontl trivial seam is a seam through the center of the image
bool reduce_horizontal_seam_trivial(Mat& in_image, Mat& out_image){

    // retrieve the dimensions of the new image
    int rows = in_image.rows-1;
    int cols = in_image.cols;
    
    // create an image slighly smaller
    out_image = Mat(rows, cols, CV_8UC3);
    
    //populate the image
    int middle = in_image.rows / 2;
    
    for(int i=0;i<=middle;++i)
        for(int j=0;j<cols;++j){
            Vec3b pixel = in_image.at<Vec3b>(i, j);
            
            /* at operator is r/w
            pixel[0] = 255;
            pixel[1] =255;
            pixel[2]=255;
            */
            
            
            
            out_image.at<Vec3b>(i,j) = pixel;
        }
    
    for(int i=middle+1;i<rows;++i)
        for(int j=0;j<cols;++j){
            Vec3b pixel = in_image.at<Vec3b>(i+1, j);
            
            /* at operator is r/w
             pixel[0] --> red
             pixel[1] --> green
             pixel[2] --> blue
             */
            
            
            out_image.at<Vec3b>(i,j) = pixel;
        }

    return true;
}

// vertical trivial seam is a seam through the center of the image
bool reduce_vertical_seam_trivial(Mat& in_image, Mat& out_image){
    // retrieve the dimensions of the new image
    int rows = in_image.rows;
    int cols = in_image.cols-1;
    
    // create an image slighly smaller
    out_image = Mat(rows, cols, CV_8UC3);
    
    //populate the image
    int middle = in_image.cols / 2;
    
    for(int i=0;i<rows;++i)
        for(int j=0;j<=middle;++j){
            Vec3b pixel = in_image.at<Vec3b>(i, j);
            
            /* at operator is r/w
             pixel[0] --> red
             pixel[1] --> green
             pixel[2] --> blue
             */
            
            
            out_image.at<Vec3b>(i,j) = pixel;
        }
    
    for(int i=0;i<rows;++i)
        for(int j=middle+1;j<cols;++j){
            Vec3b pixel = in_image.at<Vec3b>(i, j+1);
            
            /* at operator is r/w
             pixel[0] --> red
             pixel[1] --> green
             pixel[2] --> blue
             */
            
            
            out_image.at<Vec3b>(i,j) = pixel;
        }
    
    return true;
}

void reduceSeam(Mat& in_image, Mat& out_image, bool vertical) {
	 vector<int> seam = getSeam(in_image, vertical);
	 int rows = in_image.rows - (vertical ? 0 : 1);
	 int cols = in_image.cols - (vertical ? 1 : 0);

	// create an image slighly smaller
	 out_image = Mat(rows, cols, CV_8UC3);
	 for (int i = 0; i < in_image.rows; i++) {
		 for (int j = 0; j < in_image.cols; j++) {
			 if (vertical) {
				 if (j < seam[i]) {
					 out_image.at<Vec3b>(i, j) = in_image.at<Vec3b>(i, j);
				 } else if (j > seam[i]) {
					 out_image.at<Vec3b>(i, j - 1) = in_image.at<Vec3b>(i, j);
				 }
			 } else {
				 if (i < seam[j]) {
					 out_image.at<Vec3b>(i, j) = in_image.at<Vec3b>(i, j);
				 } else if (i > seam[j]) {
					 out_image.at<Vec3b>(i - 1, j) = in_image.at<Vec3b>(i, j);
				 }
			 }
		 }
	 }
}

vector<int> getSeam(const Mat& image, bool vertical) {
	int rows = image.rows;
	int cols = image.cols;

	Mat1f _energy = calculateEnergy(image);
	Mat1f energy = _energy;
	if (!vertical) {
		energy = _energy.t();
	}

	vector<int> seam(energy.rows);

	for (int i = 1; i < energy.rows; i++) {
		for (int j = 0; j < energy.cols; j++) {
			float extra = energy.at<float>(i-1, j);
			if (j > 0) {
				extra = min(extra, energy.at<float>(i-1, j-1));
			}
			if (j < cols - 1) {
				extra = min(extra, energy.at<float>(i-1, j+1));
			}
			energy.at<float>(i-1, j) += extra;
		}
	}

	int seamEndIndex = 1;
	// We want to ignore the first and the last columns because
	// with gradient magnitude as the energy function
	// the first and the last will be the smallest always
	for (int k = 1; k < energy.cols - 1; k++) {
		if (energy.at<float>(energy.rows - 1, k) <=
				energy.at<float>(energy.rows - 1, seamEndIndex)) {
			seamEndIndex = k;
		}
	}

	seam.back() = seamEndIndex;
	for (int k = seam.size() - 1; k >= 1; k--) {
		int prevIndex = seam[k];
		int nextIndex = prevIndex;
		if (prevIndex > 0) {
			if (energy.at<float>(k-1, nextIndex) > energy.at<float>(k-1, prevIndex - 1)) {
				nextIndex = prevIndex - 1;
			}
		}
		if (prevIndex < energy.cols - 1) {
			if (energy.at<float>(k-1, nextIndex) > energy.at<float>(k-1, prevIndex + 1)) {
				nextIndex = prevIndex + 1;
			}
		}
		seam[k-1] = nextIndex;
	}

	return seam;
}

Mat1f calculateEnergy(const Mat& image) {
	return gradientMagnitude(image);
}

Mat1f gradientMagnitude(const Mat& img) {
	Mat1b gray;
	cvtColor(img, gray, COLOR_BGR2GRAY);

	//Compute dx and dy derivatives
	Mat1f dx, dy;
	Sobel(gray, dx, CV_32F, 1, 0);
	Sobel(gray, dy, CV_32F, 0, 1);

	//Compute gradient
	Mat1f magn;
	magnitude(dx, dy, magn);
	return magn;
}

