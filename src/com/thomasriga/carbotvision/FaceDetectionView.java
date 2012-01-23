package com.thomasriga.carbotvision;

import java.util.Vector;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PixelFormat;
import android.graphics.PorterDuff;
import android.graphics.PorterDuffXfermode;
import android.hardware.Camera;
import android.hardware.Camera.Face;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class FaceDetectionView extends SurfaceView implements SurfaceHolder.Callback, Camera.FaceDetectionListener
{
	private Camera cam;
	private SurfaceHolder surfHolder;
	private Camera.Size frameSize;
	private boolean isRunning;
	private CarBotVision carBotVision;
	private Speech speech;
	private Vector<Integer> ids;
	private int lastMessage = 0;
	private int lastTiltMessage = 0;
	public int NO_FACE = 0, FACE_RIGHT = 1, FACE_LEFT = 2, FACE_CENTER = 3, FACE_UP = 4, FACE_DOWN = 5; 
	
	public FaceDetectionView(Context context, AttributeSet attr)
	{	
		this((CarBotVision) context, attr);
	}
	
	public FaceDetectionView(CarBotVision carBotVision, AttributeSet attr)
	{
		super(carBotVision, attr);
		this.carBotVision = carBotVision;
		surfHolder = getHolder();
		surfHolder.setFormat(PixelFormat.TRANSPARENT);
		surfHolder.addCallback(this);
		speech = new Speech(carBotVision);
		ids = new Vector<Integer>();
	}

	public void shutdown() {
		speech.shutdown();
	}
	
	public void setCam(Camera c)
	{
		cam = c;
		if(cam == null) return;
		Log.w(this.getClass().getName(), "getMaxNumDetectedFaces: " + cam.getParameters().getMaxNumDetectedFaces());
	}

	public void setRunning(boolean r)
	{
		isRunning = r;
	}

	public void setPreviewSize(Camera.Size s)
	{
		frameSize = s;
	}

	public void surfaceCreated(SurfaceHolder sh)
	{
	}

	public void surfaceDestroyed(SurfaceHolder sh)
	{
	}

	public void surfaceChanged(SurfaceHolder sh, int format, int w, int h)
	{
	}
	
	public void onFaceDetection(Face[] faces, Camera camera) {
		Log.d(this.getClass().getName(), "onFaceDetection: faces: " + faces.length);
		Camera.Parameters p = cam.getParameters();
		Camera.Size fs = p.getPreviewSize();
		
		if(fs.width != frameSize.width || fs.height != frameSize.height)
		{
			setPreviewSize(fs);
		}
		
		Canvas canvas = null;
		Paint paint = new Paint();
		paint.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.SRC));
		paint.setColor(Color.BLUE);
		paint.setAlpha(0xFF);
		if(isRunning) canvas = surfHolder.lockCanvas(null);
		if(isRunning && canvas!=null)
		{
			try
			{
				canvas.drawColor( 0, PorterDuff.Mode.CLEAR); 
				float [] vals = new float[16];	
				// clean up face ID storage
				for(int x = 0; x < ids.size(); x++) {
					boolean found = false;
					for(int y = 0; y < faces.length; y++) {
						if(ids.get(x).intValue() == faces[y].id) {
							found = true;
						}
					}
					if(!found)
						ids.remove(x);
				}
				//int [] cmds = new int[faces.length * 5];
				int [] cmds = new int[3];
				// look for new face IDs 
				if(faces.length == 0) { 
					if(this.lastMessage != NO_FACE) {
						this.lastMessage = NO_FACE;
						Log.i(this.getClass().getName(), "sent 0: NO FACE");
						canvas.drawText("sent 0: NO FACE", 30, 30, paint);
						speech.say("nessuno");
						cmds[0] = NO_FACE;
						cmds[1] = NO_FACE;
						cmds[2] = NO_FACE;
						carBotVision.getComChannel().send(cmds);
					}
				}
				for(int x = 0; x < faces.length; x++) {
					Integer i = new Integer(faces[x].id);
					if(!ids.contains(i)) {
						ids.add(i);
						speech.say("ciao");
					}
					float left = ((float) canvas.getWidth() /
							(2000.0f / (float) (faces[x].rect.left + 1000)));
					float right = ((float) canvas.getWidth() /
							(2000.0f / (float) (faces[x].rect.right + 1000)));
					float top = ((float) canvas.getHeight() /
							(2000.0f / (float) (faces[x].rect.top + 1000)));
					float bottom = ((float) canvas.getHeight() /
							(2000.0f / (float) (faces[x].rect.bottom + 1000)));
					if(x == 0) {
						// pan
						if((faces[x].rect.left + ((faces[x].rect.right - faces[x].rect.left) / 2)) < -100) {
							cmds[0] = FACE_LEFT;
						}
						else if((faces[x].rect.left + ((faces[x].rect.right - faces[x].rect.left) / 2)) > 100) {
							cmds[0] = FACE_RIGHT;
						}
						else {
							cmds[0] = FACE_CENTER;
						}
						// tilt
						if((faces[x].rect.top + ((faces[x].rect.bottom - faces[x].rect.top) / 2)) < -100) {
							cmds[1] = FACE_DOWN;
						}
						else if((faces[x].rect.top + ((faces[x].rect.bottom - faces[x].rect.top) / 2)) > 100) {
							cmds[1] = FACE_UP;
						}
						else {
							cmds[1] = FACE_CENTER;
						}
						// unused
						cmds[2] = 0;
						if((this.lastTiltMessage != cmds[1]) || (this.lastMessage != cmds[0])) {
							this.lastTiltMessage = cmds[1];
							this.lastMessage = cmds[0];
							Log.i(this.getClass().getName(), "sent message: " + cmds);
							carBotVision.getComChannel().send(cmds);
						}
					}
					vals[0] = left;
					vals[1] = top;
					vals[2] = right;
					vals[3] = top;
					
					vals[4] = right;
					vals[5] = top;
					vals[6] = right;
					vals[7] = bottom;
					
					vals[8] = right;
					vals[9] = bottom;
					vals[10] = left;
					vals[11] = bottom;
					
					vals[12] = left;
					vals[13] = bottom;
					vals[14] = left;
					vals[15] = top;
					canvas.drawLines(vals, paint);
					// bounding box & score mapped on scale from 0 to 100
					/*
					cmds[c] = (faces[x].rect.left + 1000) / 20;
					c++;
					cmds[c] = (faces[x].rect.right + 1000) / 20;
					c++;
					cmds[c] = (faces[x].rect.top + 1000) / 20;
					c++;
					cmds[c] = (faces[x].rect.bottom + 1000) / 20;
					c++;
					cmds[c] = (faces[x].score + 1000) / 20;
					c++;
					*/
					
				}
			}
			finally
			{
				surfHolder.unlockCanvasAndPost(canvas);
			}	
		}
	}
}
