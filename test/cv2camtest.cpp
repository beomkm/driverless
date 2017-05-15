#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;


int main()
{
    VideoCapture vc(0);
    if(!vc.isOpened()) {
        printf("Cannot open VideoCapture!");
        return 1;
    }

    Mat img;

    //cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, 640);
    //cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 480);

    //cvNamedWindow("OpenCvCamtest", 0);
    //cvResizeWindow("OpenCvCamtest", 640, 480);

    while(1) {
        vc >> img;
        if(img.empty()) continue;

        imshow("cam", img);

        if(cvWaitKey(30) == 'q')
            break;
    }

    destroyAllWindows();

    return 0;
}
