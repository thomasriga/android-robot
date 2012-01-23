#include <jni.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <vector>
#include <list>
#include <opencv/cv.h>

using namespace std;
using namespace cv;

//params
#define FAST_UPDATE 0.8
#define SLOW_UPDATE 0.999999
#define TH 25
#define DISTANCE 2.0
#define STILL_FRAME 50
#define MERGED_BLOB_DISTANCE 50.0
#define MIN_CONTOUR_SIZE 100

//#define VERBOSE
//#define VERBOSE_MERGE

#define MINB 0
#define MAXB 0
#define MING 180
#define MAXG 230
#define MINR 180
#define MAXR 230

//macros
#define MAX_VAL(a,b) a>b ? a : b;
#define MIN_VAL(a,b) a<b ? a : b;

//blob class
class blob
{
public:
	blob(){
		min_x=0;
		min_y=0;
		max_x=0;
		max_y=0;
		id=0;
		bar_x=0;
		bar_y=0;
		live_frame=0;
		still_frame=0;
		area=0;
		};

public:
	blob operator=(blob b){
		blob a;
		a.min_x=b.min_x;
		a.min_y=b.min_y;
		a.max_x=b.max_x;
		a.max_y=b.max_y;
		a.id=b.id;
		a.hst=b.hst;
		a.bar_x=b.bar_x;
		a.bar_y=b.bar_y;
		a.live_frame=b.live_frame;
		a.still_frame=b.still_frame;
		a.area=b.area;
		return a;
	}
	bool operator==(blob a)
	{
		if(a.id==id)
			return true;
		else
			return false;
	}
public:
	int min_x;
	int min_y;
	int max_x;
	int max_y;
	int id;
	int bar_x;
	int bar_y;
	int live_frame;
	int still_frame;
	CvHistogram *hst;
	int area;
};
class BlobTracker
{
	public:
		BlobTracker(){
			counter_id=0;
		};

	public:
		void initialize();
		void merge_overlapped_blobs();
		void merge_blobs();
		void add_lost_blobs();
		void delete_lost_blobs();
		void draw_blobs(IplImage *img,list<blob> bl);
		void draw_mask();
		void update_backgroud();
		void image_difference(IplImage *img);
		void color_track(IplImage *img,IplImage *gray);
		void find_blob(IplImage *img);
		void update_blob_list();
		void initialize(IplImage *img);
		list<blob> get_blob_list();
		IplImage *get_background();
		IplImage *get_mask();
		IplImage *get_foreground();
		int blob_dup(list<blob> ls,blob bl);
		void release();

	private:
		list<blob> blob_list;
		list<blob> tmp_blob_list;
		list<blob> lost_blob_list;

		cv::Ptr<IplImage> grayImg;
		cv::Ptr<IplImage> grayDiff;
		cv::Ptr<IplImage> removeBackgrnd;
		cv::Ptr<IplImage> grayFrame;
		cv::Ptr<IplImage> cpyImg;
		cv::Ptr<IplImage> img_red;
		cv::Ptr<IplImage> img_green;
		cv::Ptr<IplImage> img_blue;
		cv::Ptr<IplImage> addedImg;

		CvMemStorage *storage;
		CvSeq *contours;
		int counter_id;
};
//check for blob duplication
int BlobTracker::blob_dup(list<blob> a,blob bl)
{
	list<blob>::iterator it=a.begin();

	unsigned int counter_tmp=0;
	int find_blob=0;

	while(counter_tmp<a.size())
	{
		blob tb=*it;
		if(tb.id==bl.id)
		{
			find_blob=1;
			break;
		}
		it++;
		counter_tmp++;
	}

	return find_blob;
}
//merge overlapped blobs
void BlobTracker::merge_overlapped_blobs()
{
	if(tmp_blob_list.size()>1)//at least 2 elements
	{

#ifdef VERBOSE_MERGE
		printf("Merging OB\n");
#endif

		unsigned int counter_loop=0;
		unsigned int blob_list_size=tmp_blob_list.size();

		while(counter_loop<blob_list_size)
		{
			int overlapped=0;
			unsigned int counter_blob=0;
			unsigned int overlap_blob=0;

			list<blob>::iterator iter_blob=tmp_blob_list.begin();
			list<blob>::iterator ib=tmp_blob_list.begin();

			list<blob> a;
			a.clear();

			blob tmp_blob=*iter_blob;
			ib++;

			while(counter_blob<(tmp_blob_list.size()-1))
			{
				blob bl=*ib;

				blob nb=bl;

				overlapped=1;

				if(bl.min_x>=tmp_blob.max_x)
					overlapped=0;

				if(tmp_blob.min_x>=bl.max_x)
					overlapped=0;

				if(bl.min_y>=tmp_blob.max_y)
					overlapped=0;

				if(tmp_blob.min_y>=bl.max_y)
					overlapped=0;
#ifdef VERBOSE_MERGE
				printf("((%d,%d,%d)-->",tmp_blob.id,bl.id,overlapped);
#endif
				if(overlapped==1)
				{
					nb.min_x=MIN_VAL(bl.min_x,tmp_blob.min_x);
					nb.min_y=MIN_VAL(bl.min_y,tmp_blob.min_y);
					nb.max_x=MAX_VAL(bl.max_x,tmp_blob.max_x);
					nb.max_y=MAX_VAL(bl.max_y,tmp_blob.max_y);

					//nb.live_frame=MAX_VAL(bl.live_frame,tmp_blob.live_frame);
					//nb.still_frame=MAX_VAL(bl.still_frame,tmp_blob.still_frame);

					//new id
					if(bl.area>tmp_blob.area)
					{
						nb.id=bl.id;
						nb.live_frame=bl.live_frame;
						nb.still_frame=0;
						nb.hst=bl.hst;
					}
					else
					{
						nb.id=tmp_blob.id;
						nb.live_frame=tmp_blob.live_frame;
						nb.still_frame=0;
						nb.hst=tmp_blob.hst;
					}

					nb.area=(nb.max_x-nb.min_x)*(nb.max_y-nb.min_y);

					//new gowed blob in list
#ifdef VERBOSE_MERGE
					printf("M %d)",nb.id);
#endif
					if(blob_dup(a,nb)==0)
						a.push_back(nb);

					overlap_blob=1;
				}
				else
				{
					//not overlapped blob in list
#ifdef VERBOSE_MERGE
					printf("%d)",bl.id);
#endif
					if(blob_dup(a,bl)==0)
						a.push_back(bl);
				}

				ib++;

				counter_blob++;
			}

			if(overlap_blob==0)
			{
				if(blob_dup(a,tmp_blob)==0)
					a.push_back(tmp_blob);

				tmp_blob_list.clear();
				tmp_blob_list=a;
			}
			else
			{
				//add not merged to lost blobs
				if(blob_dup(a,tmp_blob)==0)
					lost_blob_list.push_back(tmp_blob);
			}

			tmp_blob_list.clear();

			tmp_blob_list=a;

			counter_loop++;
#ifdef VERBOSE_MERGE
			printf("\n");
#endif
		}
	}
}
//merge overlapped blobs
void BlobTracker::merge_blobs()
{
	if(tmp_blob_list.size()>1)//at least 2 elements
	{
#ifdef VERBOSE_MERGE
		printf("Merging DB\n");
#endif
		unsigned int counter_loop=0;
		unsigned int blob_list_size=tmp_blob_list.size();

		while(counter_loop<blob_list_size)
		{
			unsigned int counter_blob=0;
			unsigned int overlap_blob=0;

			list<blob>::iterator iter_blob=tmp_blob_list.begin();
			list<blob>::iterator ib=tmp_blob_list.begin();

			list<blob> a;
			a.clear();

			blob tmp_blob=*iter_blob;
			ib++;

			while(counter_blob<(tmp_blob_list.size()-1))
			{
				blob bl=*ib;

				blob nb=bl;

				int bl_bx=(bl.min_x+tmp_blob.max_x)/2;
				int bl_by=(bl.min_y+tmp_blob.max_y)/2;

				int tmp_blob_bx=(tmp_blob.min_x+tmp_blob.max_x)/2;
				int tmp_blob_by=(tmp_blob.min_y+tmp_blob.max_y)/2;

				double dist=sqrt((double)((bl_bx-tmp_blob_bx)*(bl_bx-tmp_blob_bx))+
				(double)((bl_by-tmp_blob_by)*(bl_by-tmp_blob_by)));

#ifdef VERBOSE_MERGE
				printf("((%d,%d,%f)-->",tmp_blob.id,bl.id,dist);
#endif

				if(dist<MERGED_BLOB_DISTANCE)
				{
					nb.min_x=MIN_VAL(bl.min_x,tmp_blob.min_x);
					nb.min_y=MIN_VAL(bl.min_y,tmp_blob.min_y);
					nb.max_x=MAX_VAL(bl.max_x,tmp_blob.max_x);
					nb.max_y=MAX_VAL(bl.max_y,tmp_blob.max_y);

					//nb.live_frame=MAX_VAL(bl.live_frame,tmp_blob.live_frame);
					//nb.still_frame=MAX_VAL(bl.still_frame,tmp_blob.still_frame);

					//new id
					if(bl.area>tmp_blob.area)
					{
						nb.id=bl.id;
						nb.live_frame=bl.live_frame;
						nb.still_frame=0;
						nb.hst=bl.hst;
					}
					else
					{
						nb.id=tmp_blob.id;
						nb.live_frame=tmp_blob.live_frame;
						nb.still_frame=0;
						nb.hst=tmp_blob.hst;
					}

					nb.area=(nb.max_x-nb.min_x)*(nb.max_y-nb.min_y);

					//new gowed blob in list
#ifdef VERBOSE_MERGE
					printf("M %d)",nb.id);
#endif
					if(blob_dup(a,nb)==0)
						a.push_back(nb);

					overlap_blob=1;
				}
				else
				{
					//not distance merged blob in list
#ifdef VERBOSE_MERGE
					printf("%d)",bl.id);
#endif

					if(blob_dup(a,bl)==0)
						a.push_back(bl);
				}

				ib++;

				counter_blob++;
			}

			if(overlap_blob==0)
			{
				if(blob_dup(a,tmp_blob)==0)
					a.push_back(tmp_blob);

				tmp_blob_list.clear();
				tmp_blob_list=a;
			}
			else
			{
				//add not merged to lost blobs
				if(blob_dup(a,tmp_blob)==0)
					lost_blob_list.push_back(tmp_blob);
			}

			tmp_blob_list.clear();

			tmp_blob_list=a;

			counter_loop++;
#ifdef VERBOSE_MERGE
			printf("\n");
#endif
		}
	}
}
//add lost blob
void BlobTracker::add_lost_blobs()
{
	unsigned int counter_iter=0;
	list<blob>::iterator iter_blob;

	if(blob_list.size()!=0)
	{
		list<blob> a;
		a.clear();

		//insert old lost blob
		iter_blob=lost_blob_list.begin();
		counter_iter=0;

		while(counter_iter<lost_blob_list.size())
		{
			blob tmp_blob=*iter_blob;

			if(blob_dup(a,tmp_blob)==0)
			{
				a.push_back(tmp_blob);
			}

			counter_iter++;
			iter_blob++;
		}

		//insert new lost blob
		iter_blob=blob_list.begin();
		counter_iter=0;

		while(counter_iter<blob_list.size())
		{
			blob tmp_blob=*iter_blob;

			if(blob_dup(a,tmp_blob)==0)
			{
				a.push_back(tmp_blob);
			}

			counter_iter++;
			iter_blob++;
		}

		lost_blob_list.clear();

		lost_blob_list=a;
	}
}
//delete lost blob
void BlobTracker::delete_lost_blobs()
{
	unsigned int counter_iter=0;

	list<blob> a;
	a.clear();

	list<blob>::iterator iter_blob=lost_blob_list.begin();

#ifdef VERBOSE
	if(lost_blob_list.size()!=0)
	{
		printf("Removing: ");
	}
#endif

	//if old lost blob, remove it
	while(counter_iter<lost_blob_list.size())
	{
		blob tmp_blob=*iter_blob;

		if(tmp_blob.live_frame>0)
		{

#ifdef VERBOSE
			printf("LB(%d,%d) ",tmp_blob.id,tmp_blob.live_frame);
#endif

			tmp_blob.live_frame--;

			if(blob_dup(a,tmp_blob)==0)
			{
				a.push_back(tmp_blob);
			}

		}

		counter_iter++;
		iter_blob++;
	}

#ifdef VERBOSE
	if(lost_blob_list.size()!=0)
		printf("\n");
#endif

	lost_blob_list.clear();

	lost_blob_list=a;
}
//draw blob
void BlobTracker::draw_blobs(IplImage *img,list<blob> bl)
{
	//show blob list
	list<blob>::iterator iter;
	iter = bl.begin();
	unsigned int counter_iter=0;

	while(counter_iter<bl.size())
	{
		blob tmp_blob=*iter;
		CvFont font;

		cvDrawRect(img,cvPoint(tmp_blob.min_x,tmp_blob.min_y),
			cvPoint(tmp_blob.max_x,tmp_blob.max_y),cvScalar(0,255,0,255),9,CV_AA,0);

		cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC,1.0,1.0,0,1);

		char id_val[256];
		sprintf(id_val,"%d,%d,%d",tmp_blob.id,tmp_blob.live_frame,tmp_blob.still_frame);

		cvPutText(img,id_val,cvPoint(tmp_blob.bar_x,tmp_blob.bar_y),
			&font,cvScalar(255,255,0,255));

		iter++;
		counter_iter++;
	}

#ifdef VERBOSE
	if(counter_iter!=0)
		printf("\n");
#endif

}
//draw mask
void BlobTracker::draw_mask()
{
	list<blob>::iterator iter;
	iter = blob_list.begin();

	unsigned int counter_elem=0;
	while(counter_elem<blob_list.size())
	{
		int counter=0;

		blob tmp_blob = *iter;

		int min_x=tmp_blob.min_x;
		int min_y=tmp_blob.min_y;
		int max_x=tmp_blob.max_x;
		int max_y=tmp_blob.max_y;

		for(int i=min_y-25;i<max_y+25;i++)
		{
			for(int j=min_x-25;j<max_x+25;j++)
			{
				counter=i*removeBackgrnd->width+j;

				if((counter>0 && counter<(removeBackgrnd->width*removeBackgrnd->height)))
				{
					if(tmp_blob.still_frame<STILL_FRAME)//frame timeout
					{
						removeBackgrnd->imageData[counter]=255;//slow marker
					}
					else
					{
						removeBackgrnd->imageData[counter]=128;//fast marker (no object, release it)
					}
				}
			}
		}

		counter_elem++;
		iter++;
	}
}
//update background
void BlobTracker::update_backgroud()
{
	double a=FAST_UPDATE;
	double b=SLOW_UPDATE;

	unsigned char *bk=new unsigned char [grayImg->height*grayImg->width];
	unsigned char *act=new unsigned char[grayFrame->height*grayFrame->width];

	memcpy(bk,grayImg->imageData,grayImg->height*grayImg->width);
	memcpy(act,grayFrame->imageData,grayFrame->height*grayImg->width);

	int counter=0;
	for(int i=0;i<grayImg->height;i++)
	{
		for(int j=0;j<grayImg->width;j++)
		{
			if(removeBackgrnd->imageData[counter]==-1)
			{
				//slow update
				bk[counter]=(unsigned char)(((double)act[counter] -
					b*(double)(act[counter]-bk[counter])+0.5));

			}
			else
			{
				//fast update
				bk[counter]=(unsigned char)(((double)act[counter] -
					a*(double)(act[counter]-bk[counter])+0.5));
			}

			counter++;
		}
	}

	memcpy(grayImg->imageData,bk,grayFrame->width*grayFrame->height);

	delete []act;
	delete []bk;
}
//image difference
void BlobTracker::image_difference(IplImage *img)
{
	//Color to gray
	//cvCvtColor( img , grayFrame , CV_BGR2GRAY );

	grayFrame=cvCloneImage(img);
	
	//Frame difference (actual and dynamic backgroud)
	cvAbsDiff( grayFrame , grayImg , grayDiff );

	//Thresholding
	cvThreshold(grayDiff,grayDiff,TH,255,CV_THRESH_BINARY);
}
//color track (sperimental)
void BlobTracker::color_track(IplImage *img,IplImage *gray)
{
	cvZero(img_red);
	cvZero(img_green);
	cvZero(img_blue);
	cvZero(grayImg);
	cvZero(grayDiff);
	
	cvSplit(img,img_blue,img_green,img_red,grayFrame);

	//cvCvtColor(img,grayFrame,CV_BGR2GRAY);
	
	grayFrame=cvCloneImage(gray);
	
	//grayFrame=gray;

	cvAbsDiff(img_blue,grayFrame,img_blue);
	cvAbsDiff(img_green,grayFrame,img_green);
	cvAbsDiff(img_red,grayFrame,img_red);

	cvNot(img_blue,img_blue);
	cvNot(img_green,img_green);
	cvNot(img_red,img_red);

	cvInRangeS(img_blue,cvScalar(MINB),cvScalar(MAXB),img_blue);
	cvMorphologyEx(img_blue,img_blue,NULL,NULL,CV_MOP_CLOSE);

	cvInRangeS(img_green,cvScalar(MING),cvScalar(MAXG),img_green);
	cvMorphologyEx(img_green,img_green,NULL,NULL,CV_MOP_CLOSE);

	cvInRangeS(img_red,cvScalar(MINR),cvScalar(MAXR),img_red);
	cvMorphologyEx(img_red,img_red,NULL,NULL,CV_MOP_CLOSE);

	cvAdd(img_blue,img_green,grayDiff,NULL);
	cvAdd(grayDiff,img_red,grayDiff,NULL);

	//cvShowImage("green",img_green);
	//cvShowImage("blue",img_blue);
	//cvShowImage("red",img_red);
}
//find blob
void BlobTracker::find_blob(IplImage *img)
{
	tmp_blob_list.clear();

	//Segmentation
	cvFindContours(grayDiff, storage, &contours, sizeof(CvContour),
		    CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );

	//reset mask image
	cvZero(removeBackgrnd);

	CvRect max_rect=cvRect(0,0,0,0);

	 if(contours)
	{
		contours = cvApproxPoly( contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, 3, 1);

		do
		{
			if(contours)
			{
				double area=fabs(cvContourArea(contours,CV_WHOLE_SEQ));

				//use big objects
				if(area>MIN_CONTOUR_SIZE)
				{
					CvRect rect = cvBoundingRect(contours);

					unsigned char *tmp_image = new unsigned char[(rect.width+20)*(rect.height+20)];
					unsigned char *clr_image = new unsigned char[img->width*img->height];

					memcpy(clr_image,grayFrame->imageData,img->width*img->height);

					//get blob roi
					int counter=0;
					int counter_subimg=0;

					for(int i=rect.y-10;i<rect.y+rect.height+10;i++)
					{
						for(int j=rect.x-10;j<rect.x+rect.width+10;j++)
						{
							counter=i*img->width+j;

							if((counter>0 && counter<(img->width*img->height)))
							{
								tmp_image[counter_subimg]=clr_image[counter];

								counter_subimg++;
							}
						}
					}

					//create blob
					blob bl;

					bl.min_x=rect.x-5;
					bl.min_y=rect.y-5;
					bl.max_x=rect.x+(rect.width+5);
					bl.max_y=rect.y+(rect.height+5);

					bl.bar_x=(bl.min_x+bl.max_x)/2;
					bl.bar_y=(bl.min_y+bl.max_y)/2;

					bl.area=(rect.width+10)*(rect.height+10);

					bl.id=0;

					//histogram
					CvSize sz_si = cvSize(rect.width+10,rect.height+10);

					cv::Ptr<IplImage> histFrame = cvCreateImage( sz_si , IPL_DEPTH_8U ,1 );

					memcpy(histFrame->imageData,tmp_image,sz_si.width*sz_si.height);

					int size_hist=32;
					bl.hst=cvCreateHist(1,&size_hist,CV_HIST_ARRAY,0,1);

					cvCalcHist((IplImage**)&histFrame,bl.hst,0,0);

					//cvNormalizeHist(bl.hst,1.0);

					unsigned int counter_size=0;

					list<blob>::iterator it = blob_list.begin();
					list<blob>::iterator it_max;

					double distance_max=-1.0;

					if(blob_list.size()>0)//if there are old blobs
					{
#ifdef VERBOSE
						printf("Compare: ");
#endif
						int lost_blob=0;
						//check for similar histogram
						while(counter_size<blob_list.size())
						{
							blob old_blob = *it;
							double dist=cvCompareHist(bl.hst,old_blob.hst,CV_COMP_CORREL);
#ifdef VERBOSE
							printf("(%f,%d) ",dist,old_blob.id);
#endif
							if(distance_max<dist)
							{
								distance_max=dist;
								bl.id=old_blob.id;
								it_max=it;
								bl.live_frame=old_blob.live_frame;
								bl.still_frame=old_blob.still_frame;
							}

							it++;
							counter_size++;
						}

						counter_size=0;
						it=lost_blob_list.begin();
						while(counter_size<lost_blob_list.size())
						{
							blob old_blob = *it;
							double dist=cvCompareHist(bl.hst,old_blob.hst,CV_COMP_CORREL);
#ifdef VERBOSE
							printf("LB (%f,%d) ",dist,old_blob.id);
#endif
							if(distance_max<dist)
							{
								distance_max=dist;
								bl.id=old_blob.id;
								it_max=it;
								bl.live_frame=old_blob.live_frame;
								bl.still_frame=old_blob.still_frame;
								lost_blob=1;
							}

							counter_size++;
							it++;
						}
#ifdef VERBOSE
						printf("--> Blob: %f,%d,%d\n",distance_max,bl.id,bl.live_frame);
#endif
						if(distance_max<-0.8)//new blob
						{
							bl.id=counter_id++;
							bl.live_frame=0;
							bl.still_frame=0;
						}
						else// blob reassign
						{
							blob old_blob=*it_max;

							if(lost_blob==0)
							{
								blob_list.erase(it_max);//erase old blob
							}
							else
							{
								lost_blob_list.erase(it_max);//erase lost blob
							}

							bl.live_frame++;

							if(sqrt((double)((bl.bar_x-old_blob.bar_x)*(bl.bar_x-old_blob.bar_x)) +
								(double)((bl.bar_y-old_blob.bar_y)*(bl.bar_y-old_blob.bar_y)))<DISTANCE)
							{
								bl.still_frame++;
							}
						}
					}
					else//no old blobs, then new blob
					{
						bl.id=counter_id++;
						bl.live_frame=0;
						bl.still_frame=0;
					}

#ifdef VERBOSE
					printf("\n");
#endif

					list<blob>::iterator iter_blob=tmp_blob_list.begin();

					unsigned int counter_iter=0;
					int find_item=0;

					while(counter_iter<tmp_blob_list.size())
					{
						blob tmp_blob=*iter_blob;

						if(tmp_blob.id==bl.id)
						{
							find_item=1;
							break;
						}

						iter_blob++;
						counter_iter++;
					}

					if(find_item==0)
					{
						tmp_blob_list.push_back(bl);
					}

					cvReleaseImage((IplImage**)&histFrame);

					delete []tmp_image;
					delete []clr_image;
				}
			}

			contours=contours->h_next;

		} while(contours!=NULL);
	}
}
//update blob list
void BlobTracker::update_blob_list()
{
	blob_list.clear();
	blob_list=tmp_blob_list;
	tmp_blob_list.clear();
}
//initialize blob tracker
void BlobTracker::initialize(IplImage *img)
{
	CvSize sz = cvGetSize(img);

	grayImg = cvCreateImage( sz , IPL_DEPTH_8U , 1 );
	grayDiff = cvCreateImage( sz , IPL_DEPTH_8U ,1 );
	removeBackgrnd = cvCreateImage( sz , IPL_DEPTH_8U ,1 );
	grayFrame = cvCreateImage( sz , IPL_DEPTH_8U ,1 );
	img_red = cvCreateImage( sz, 8 , 1 );
	img_green = cvCreateImage( sz, 8 , 1 );
	img_blue = cvCreateImage( sz, 8 , 1 );
	addedImg = cvCreateImage( sz, 8 , 1  );

	//cvCvtColor( img, grayImg , CV_BGR2GRAY );

	grayImg=cvCloneImage(img);
	
	storage = cvCreateMemStorage(0);
	contours=0;
	blob_list.clear();
	tmp_blob_list.clear();
	lost_blob_list.clear();
}
//get blob list
list<blob> BlobTracker::get_blob_list()
{
	return blob_list;
}
//get background
IplImage *BlobTracker::get_background()
{
	return grayImg;
}
//get mask
IplImage *BlobTracker::get_mask()
{
	return removeBackgrnd;
}
//get foreground
IplImage *BlobTracker::get_foreground()
{
	return grayDiff;
}
//release blob tracker
void BlobTracker::release()
{
	cvReleaseImage((IplImage **)&removeBackgrnd);
	cvReleaseImage((IplImage **)&grayImg);
	cvReleaseImage((IplImage **)&grayFrame);
	cvReleaseImage((IplImage **)&img_red);
	cvReleaseImage((IplImage **)&img_green);
	cvReleaseImage((IplImage **)&img_blue);
	cvReleaseImage((IplImage **)&addedImg);
}

int init_lib=0;
BlobTracker blob_tracker;

#define COLOR_TRACKER
//#define DIFF_TRACKER

extern "C" JNIEXPORT void JNICALL Java_com_thomasriga_carbotvision_NativeCVisionView_features_extraction(JNIEnv* env, jobject thiz, jint width, jint height, jbyteArray yuv, jintArray bgra)
{
    jbyte* _yuv  = env->GetByteArrayElements(yuv, 0);
    jint*  _bgra = env->GetIntArrayElements(bgra, 0);

    Mat myuv(height + height/2, width, CV_8UC1, (unsigned char *)_yuv);
    Mat mbgra(height, width, CV_8UC4, (unsigned char *)_bgra);
    Mat mgray(height, width, CV_8UC1, (unsigned char *)_yuv);
	
    IplImage img=mgray;
    IplImage img_color=mbgra;

    if(init_lib<9)
    {
    	blob_tracker.initialize(&img);
    	init_lib++;
    }

    //Please make attention about BGRA byte order
    //ARGB stored in java as int array becomes BGRA at native level
    cvtColor(myuv, mbgra, CV_YUV420sp2BGR, 4);
    
    img_color=mbgra;
    
    if(init_lib>=9)
    {

#ifdef DIFF_TRACKER
    	blob_tracker.image_difference(&img);
#endif
	    
#ifdef COLOR_TRACKER
    	blob_tracker.color_track(&img_color,&img);
#endif
	    
    	blob_tracker.find_blob(&img);

    	//merging overlapped blobs in actual blob list
    	blob_tracker.merge_overlapped_blobs();

    	//merging near blob in actual blob list
    	blob_tracker.merge_blobs();

    	//add lost blobs
    	blob_tracker.add_lost_blobs();

    	//delete lost blobs
    	blob_tracker.delete_lost_blobs();

    	//update blob list
    	blob_tracker.update_blob_list();

    	//show blob list
    	list<blob> bl = blob_tracker.get_blob_list();
    	blob_tracker.draw_blobs(&img_color,bl);

#ifdef DIFF_TRACKER
    	//update mask background fast/slow
    	blob_tracker.draw_mask();

    	//update background
    	blob_tracker.update_backgroud();
#endif

    }
    
    env->ReleaseIntArrayElements(bgra, _bgra, 0);
    env->ReleaseByteArrayElements(yuv, _yuv, 0);

}
