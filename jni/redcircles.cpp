#include <jni.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <cv.h>
#include <cvaux.h>

using namespace std;
using namespace cv;

extern "C" {
JNIEXPORT void JNICALL Java_com_thomasriga_carbotvision_NativeCVisionView_redcircles(JNIEnv* env, jobject thiz, jint width, jint height, jbyteArray yuv, jintArray bgra, jintArray cmds)
{
		jbyte* _yuv  = env->GetByteArrayElements(yuv, 0);
		jint*  _bgra = env->GetIntArrayElements(bgra, 0);
		jint* _cmds = env->GetIntArrayElements(cmds, 0);
		Mat myuv(height + height/2, width, CV_8UC1, (unsigned char *)_yuv);
		Mat mbgra(height, width, CV_8UC4, (unsigned char *)_bgra);
		Mat mgray(height, width, CV_8UC1, (unsigned char *)_yuv);
		jbyte DIRECTION_HINT = 0x2, LEFT = 0x1, STRAIGHT = 0x2, RIGHT = 0x3, FORWARD = 0xff, STOP = 0x2, BACKWARD = 0x3;
		int x;
		
		cvtColor(myuv, mbgra, CV_YUV420sp2BGR, 4);
		
		IplImage stub = mbgra;
		IplImage gstub = mgray;
		IplImage* hsv_frame = cvCreateImage(cvSize(width,height), IPL_DEPTH_8U, 3);
		IplImage* bgr_frame = cvCreateImage(cvSize(width,height), IPL_DEPTH_8U, 3);
		IplImage* thresholded = cvCreateImage(cvSize(width,height), IPL_DEPTH_8U, 1);
		IplImage* thresholded2 = cvCreateImage(cvSize(width,height), IPL_DEPTH_8U, 1);
		
		// red
		/*
		CvScalar hsv_min = cvScalar(0, 30, 60, 0); // H, S, V
		CvScalar hsv_max = cvScalar(15, 180, 180, 0); // H, S, V
		CvScalar hsv_min2 = cvScalar(160, 30, 60, 0); // H, S, V
		CvScalar hsv_max2 = cvScalar(180, 180, 180, 0); // H, S, V
		cvCvtColor(&stub, hsv_frame, CV_BGR2HSV);
		cvInRangeS(hsv_frame, hsv_min, hsv_max, thresholded);
		cvInRangeS(hsv_frame, hsv_min2, hsv_max2, thresholded2);
		cvOr(thresholded, thresholded2, thresholded);
		*/		
		cvSmooth(&gstub, thresholded, CV_GAUSSIAN, 9, 9);
		//cvCanny(thresholded, thresholded2, 1.0, 1.0, 3);    
		CvMemStorage* storage = cvCreateMemStorage(0);
	
		Mat mFinal = thresholded;     
		cvtColor(mFinal, mbgra, CV_GRAY2BGR, 4);
		stub = mbgra;
		
		 CvSeq* circles = cvHoughCircles(thresholded, storage, CV_HOUGH_GRADIENT, 2, thresholded->height/4, 200, 100);
		 if(circles->total > 0)
		 {
		 	float* p = (float*)cvGetSeqElem( circles, 0);
		 	//cvCircle( &stub, cvPoint(cvRound(p[0]),cvRound(p[1])), cvRound(p[2]), CV_RGB(255,255,255), 6);	 
		 	for(x = 0; x < 3; x++)
		 	{
		 		_cmds[x] = p[x];
		 	}
			 /*
			 if(p[0] < (int) ((double) width * 0.33)) 
			 {
				 _cmds[0] = DIRECTION_HINT;
				 _cmds[1] = RIGHT;
				 _cmds[2] = FORWARD;
			 }
			 else if(p[0] < (int) ((double) width * 0.66))
			 {
				 _cmds[0] = DIRECTION_HINT;
				 _cmds[1] = STRAIGHT;
				 _cmds[2] = FORWARD;
			 }
			 else
			 {
				 _cmds[0] = DIRECTION_HINT;
				 _cmds[1] = LEFT;
				 _cmds[2] = FORWARD;
			 } 
		 	*/	 
		 } 
		 else
		 {
			_cmds[0] = 0;
			_cmds[1] = 0;
			_cmds[2] = 0;
		}
		
		cvReleaseMemStorage(&storage);
		cvReleaseImage(&hsv_frame);
		cvReleaseImage(&bgr_frame);
		cvReleaseImage(&thresholded);    
		cvReleaseImage(&thresholded2);    
		env->ReleaseIntArrayElements(bgra, _bgra, 0);
		env->ReleaseIntArrayElements(cmds, _cmds, 0);
		env->ReleaseByteArrayElements(yuv, _yuv, 0);
}
}

