package com.thomasriga.carbotvision;

import android.content.Context;
import android.graphics.PixelFormat;
import android.hardware.Camera;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.PowerManager;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.OrientationEventListener;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.Window;
import android.view.WindowManager;

public class CarBotVision extends CarBotUsb implements SurfaceHolder.Callback
{
	private static final int EXIT_MENU_OPTION = 0xCC882201;

	private Camera cam = null;
	private SurfaceView camSurfView;
	private PowerManager.WakeLock wakeLock;
	private FaceDetectionView faceDetectionView;
	private NativeCVisionView cVisionView;
	private SurfaceHolder camSurfHolder;
	private OrientationEventListener orientationEL;
	private Camera.Size previewSize = null;
	private boolean previewRunning = false;
	private int orientation = 0;
	public static int FACE_DETECTION = 1, CV_DETECTION = 2;
	private int detectionType = FACE_DETECTION;
    //private int detectionType = CV_DETECTION;
	CommChannel commChannel;
	BotSensors botSensors; 
	
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		if (mAccessory != null) {
			showControls();
		} else {
			hideControls();
		}
		// create sensor manager
		//this.botSensors = new BotSensors(this);
		
		// create communication channel
		//commChannel = new BTChannel(this);
		commChannel = this;
		// create UI
		getWindow().setFormat(PixelFormat.TRANSLUCENT);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
		PowerManager pm = (PowerManager)getSystemService(Context.POWER_SERVICE);
		wakeLock = pm.newWakeLock(PowerManager.SCREEN_DIM_WAKE_LOCK, "thomasriga");
		wakeLock.acquire();
		setContentView(R.layout.main);
	}

	protected void enableControls(boolean enable) {
		if (enable) {
			showControls();
		} else {
			hideControls();
		}
	}
	
	protected void hideControls() {
		setContentView(R.layout.no_device);
	}

	protected void showControls() {
		setContentView(R.layout.main);
	}

	
	public CommChannel getComChannel() {
		return commChannel;
	}
	
	public boolean onCreateOptionsMenu(Menu m)
	{
		m.add(0, EXIT_MENU_OPTION, 0, "Exit");
		return true;
	}

	public boolean onOptionsItemSelected(MenuItem i)
	{
		switch(i.getItemId())
		{
		case EXIT_MENU_OPTION:
			this.finish();
			return true;
		default:
			return false;
		}
	}

	@Override
	public void onDestroy()
	{
		stopCam();
		wakeLock.release();
		super.onDestroy();
	}

	@Override
	public void onStart()
	{
		initCam();
		super.onStart();
	}

	@Override
	public void onResume()
	{
		initCam();
		super.onResume();
	}

	@Override
	public void onPause()
	{
		stopCam();
		camSurfHolder.removeCallback(this);
		this.finish();
		super.onPause();
	}

	public void surfaceCreated(SurfaceHolder sh)
	{
	}

	public void surfaceChanged(SurfaceHolder sh, int format, int w, int h)
	{
		if(cam != null)
		{
			if(previewRunning) cam.stopPreview();
			Camera.Parameters p = cam.getParameters();
			p.setRotation(orientation);
			for(Camera.Size s : p.getSupportedPreviewSizes())
			{
				p.setPreviewSize(s.width, s.height);
				if(detectionType == FACE_DETECTION)
					faceDetectionView.setPreviewSize(s);
				else if(detectionType == CV_DETECTION)
					cVisionView.setPreviewSize(s);
				previewSize = s;
				break;
			}
			cam.setParameters(p);
			try
			{
				cam.setPreviewDisplay(sh);
			}
			catch(Exception e)
			{
				e.printStackTrace();
			}
			cam.startPreview();
			if(detectionType == FACE_DETECTION) {
				cam.setFaceDetectionListener(faceDetectionView);
				cam.startFaceDetection();
			}
			previewRunning = true;
		}
	}

	public void surfaceDestroyed(SurfaceHolder sh)
	{
	}

	private void initCam()
	{
		commChannel.connect();
        camSurfView = (SurfaceView)findViewById(R.id.surface_camera);
		camSurfHolder = camSurfView.getHolder();
		camSurfHolder.addCallback(this);
		camSurfHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
		if(detectionType == FACE_DETECTION) {
			faceDetectionView = (FaceDetectionView)findViewById(R.id.facedetection_surface_overlay);
			faceDetectionView.getHolder().setFormat(PixelFormat.TRANSLUCENT);
		}
		else if(detectionType == CV_DETECTION) {
			cVisionView = (NativeCVisionView)findViewById(R.id.surface_overlay);
			cVisionView.getHolder().setFormat(PixelFormat.TRANSLUCENT);
		}
		if(cam == null)
		{
			cam = Camera.open();
		}
		if(previewSize != null && previewSize.width > 0 && previewSize.height > 0)
		{
			if(detectionType == FACE_DETECTION)
				faceDetectionView.setPreviewSize(previewSize);
			else if(detectionType == CV_DETECTION)
				cVisionView.setPreviewSize(previewSize);
		}
		if(detectionType == FACE_DETECTION) {
			faceDetectionView.setCam(cam);
			faceDetectionView.setRunning(true);
		}
		else if(detectionType == CV_DETECTION) {
			cVisionView.setCam(cam);
			cVisionView.setRunning(true);
		}
		previewRunning = false;
		orientationEL = new OrientationEventListener(this, SensorManager.SENSOR_DELAY_NORMAL)
		{
			@Override
			public void onOrientationChanged(int o)
			{
				if(o == ORIENTATION_UNKNOWN) return;
				o = (o+45)/90*90;
				orientation = o%360;
			}
		};
		if(orientationEL.canDetectOrientation()) orientationEL.enable();
	}

	private void stopCam()
	{
		orientationEL.disable();
		if(detectionType == FACE_DETECTION) {
			if(cam != null) {
				cam.stopFaceDetection();
				cam.setFaceDetectionListener(null);
			}
			faceDetectionView.setRunning(false);
			faceDetectionView.shutdown();
			
		}
		else if(detectionType == CV_DETECTION) {
			cVisionView.setRunning(false);
		}
		if(cam != null) {
			cam.stopPreview();
			previewRunning = false;
			cam.setPreviewCallback(null);
			cam.release();
			cam = null;
		}
		commChannel.disconnect();
	}	
}
