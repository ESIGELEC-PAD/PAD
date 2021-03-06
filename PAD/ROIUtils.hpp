//
//  ROIUtils.hpp
//  PAD
//
//  Created by Aaron Vizzini & Wu Weibo on 23/03/2016.
//  Copyright © 2016 ESIGELEC-IS. All rights reserved.
//

#include <opencv/cv.h>
#include "PADSettings.hpp"

using namespace cv;
using namespace std;

#define FRAMES_TO_AVG 5
#define EMPTY -1
#define OCCUPIED 1

#ifndef ROI_hpp
#define ROI_hpp
class ROI {
    private:
        int occupationOverTime[FRAMES_TO_AVG] = { [0 ... FRAMES_TO_AVG-1] = EMPTY };//Keeps track of occupation over N number of frames.
        bool occupied = false; //whether or not this roi is considered occupied
    
    public:
        int id;
        int parking_lot_id;
        String description;
        Point a, b, c, d; //the four points that form this roi
        int whitePixelCount = 0; //number of white pixels in this roi
        int redPixelCount = 0; //number of red pixels in this roi
        double threshold = PADSettings::instance().getWhitePixelThreshold();
        vector<Point> contour;
    
    /*
     Set occupied or not
     This method does not set the occupied instance variable to isOccupied.
     It collects N number of isOccupied records then determines if more than half of them have a value of true, then it will set the occupied instance variable to true or false
     The purpose of this is to "smooth" out the occupied variable since noise can briefly make an ROI that is on the edge of being occupied/not occupied change statuses for a frame or two.
     */
    void setOccupied(bool isOccupied){
        if(occupationOverTime[FRAMES_TO_AVG-1] == EMPTY) {
            for(int i = 0; i<FRAMES_TO_AVG; i++){
                if(occupationOverTime[i] == -1){
                    occupationOverTime[i] = isOccupied;
                    break;
                }
            }
        } else {
            int occupiedCount = 0;
            
            for(int i = 0; i < FRAMES_TO_AVG; i++){
                if(occupationOverTime[i] == OCCUPIED) {
                    occupiedCount++;
                }
                occupationOverTime[i] = EMPTY;
            }
            
            occupied = occupiedCount > (FRAMES_TO_AVG/2) ? true : false;
        }
    }
    
    //Return whether or not the ROI is occupied
    bool getOccupied() {
        return occupied;
    }
    
};
#endif

#ifndef ROIUtils_hpp
#define ROIUtils_hpp
class ROIUtils {
public:
    //Draws ROIs as polygons on the image.
    static void drawROIsOnImage(vector<ROI> rois, Mat &image);
    //Get individual mats from the defined rois.
    static vector<Mat> getROIMats(vector<ROI> rois, Mat &image);
    //Set the white pixel counts of each rois, via the submats we are passing in for each parking spot.
    static void setWhitePixelCounts(vector<Mat> mats, vector<ROI> &rois);
    //Determine if a given roi is occupied based on its white pixel count.
    static void whitePixelOccupied(vector<ROI> &rois);
    //Red pixels means motion is occuring within the given mat, we need to set this count for later determinination of whether or not there is moving obstruction.
    static void setRedPixelCounts(vector<Mat> mats, vector<ROI> &rois);
private:
    //Add a label to display the coverage percentage for a given ROI
    static void addLabel(cv::Mat& image, const ROI &roi);
};
#endif

