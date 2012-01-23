package com.thomasriga.carbotvision;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PorterDuff;
import android.graphics.PorterDuffXfermode;
import android.graphics.Rect;
import android.graphics.RectF;
import android.hardware.Camera;
import android.hardware.Camera.PreviewCallback;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import java.lang.System;
import java.nio.ByteBuffer;
import java.nio.IntBuffer;

public class NativeCVisionView extends SurfaceView implements SurfaceHolder.Callback
{
	private Camera cam;
	private SurfaceHolder surfHolder;
	private byte[] frame;
	private IntBuffer frameDifference;
	private Camera.Size frameSize;
	private boolean isRunning;
	private CarBotVision carBotVision;

	private native void sobelFilter(byte[] frame, int width, int height, IntBuffer diff);
	private native void redcircles(int width, int height, byte yuv[], int[] rgba, int[] cmd);
	private native void features_extraction(int width, int height, byte yuv[], int[] rgba);

	static
	{
		System.loadLibrary("analysis");
	}

	public NativeCVisionView(Context context, AttributeSet attr)
	{	
		this((CarBotVision) context, attr);
	}
	
	public NativeCVisionView(CarBotVision carBotVision, AttributeSet attr)
	{
		super(carBotVision, attr);
		this.carBotVision = carBotVision;
		surfHolder = getHolder();
		surfHolder.addCallback(this);
	}

	public void setCam(Camera c)
	{
		cam = c;
		if(cam == null) return;
		cam.setPreviewCallback(new PreviewCallback()
		{
			public void onPreviewFrame(byte[] f, Camera c)
			{
				if(f.length != (frame.length*1.5))
				{
					Log.w(this.getClass().getName(), "skipping frame");
					return;
				}
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
				paint.setStyle(Paint.Style.STROKE);
				if(isRunning) canvas = surfHolder.lockCanvas(null);
				if(isRunning && canvas!=null)
				{
					try
					{
						System.arraycopy(f, 0, frame, 0, frame.length);
				        int[] rgba = new int[frameSize.width * frameSize.height];
				        int[] cmds = new int[3];
				        //features_extraction(frameSize.width, frameSize.height, f, rgba);
				        redcircles(frameSize.width, frameSize.height, f, rgba, cmds);
				        //carBotVision.sendCommand(cmds);
						//analysis(frame, frameSize.width, frameSize.height, frameDifference);
						frameDifference.position(0);
						//Bitmap bmp = Bitmap.createBitmap(frameSize.width>>0, frameSize.height>>0, Bitmap.Config.ARGB_8888);
						//bmp.copyPixelsFromBuffer(frameDifference);
				        Bitmap bmp = Bitmap.createBitmap(frameSize.width, frameSize.height, Bitmap.Config.ARGB_8888);
				        bmp.setPixels(rgba, 0, frameSize.width, 0, 0, frameSize.width, frameSize.height);
				        Rect src = new Rect(0, 0, (frameSize.width>>0)-1, (frameSize.height>>0)-1);
						RectF dst = new RectF(0, 0, canvas.getWidth()-1, canvas.getHeight()-1);
						canvas.drawBitmap(bmp, src, dst, paint);
						
						int cx = 0;
						if(cmds[0] > 0)
							cx = canvas.getWidth() / (frameSize.width / cmds[0]);
						int cy = 0;
						if(cmds[1] > 0)
							cy = canvas.getHeight() / (frameSize.height / cmds[1]);
						int cd = cmds[2];
						canvas.drawCircle(cx, cy, cd, paint);
						
						//canvas.drawCircle(cmds[0], cmds[1], cmds[2], paint);
					}
					finally
					{
						surfHolder.unlockCanvasAndPost(canvas);
					}	
				}
			}
		});
	}

	public void setRunning(boolean r)
	{
		isRunning = r;
	}

	public void setPreviewSize(Camera.Size s)
	{
		frameSize = s;
		frame = new byte[s.width*s.height];
		frameDifference = ByteBuffer.allocateDirect((s.width*s.height)<<2).asIntBuffer();
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
}
