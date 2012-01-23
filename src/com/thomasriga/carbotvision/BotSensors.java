package com.thomasriga.carbotvision;

import java.util.ArrayList;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.List;
import java.util.Vector;
import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.util.Log;

public class BotSensors implements SensorEventListener, Runnable {	
	private SensorManager sm;
	private Hashtable<Integer, Sensor> sensors = new Hashtable<Integer, Sensor>();
	private int sensorDelay;
	private int messageDelay;
	private Hashtable<Integer, float []> values = new Hashtable<Integer, float []>();
	private CarBotVision carBotVision;
	private static int DEFAULT_MESSAGE_DELAY = 1000; // ms.
	
	public BotSensors(CarBotVision carBotVision) {
		this(carBotVision, null);
	}	

	public BotSensors(CarBotVision carBotVision, Vector<Integer> types) {
		this(carBotVision, types, SensorManager.SENSOR_DELAY_UI, DEFAULT_MESSAGE_DELAY);
	}

	public BotSensors(CarBotVision carBotVision, Vector<Integer> types, int sensorDelay, int messageDelay) {
		this.carBotVision = carBotVision;
		this.sm = (SensorManager) carBotVision.getSystemService(Context.SENSOR_SERVICE);
		this.sensorDelay = sensorDelay;
		Vector<Integer> t;
		if(types == null) {
			t = new Vector<Integer>();
			/*
			t.add(new Integer(Sensor.TYPE_ORIENTATION));	
			t.add(new Integer(Sensor.TYPE_ACCELEROMETER));			
			t.add(new Integer(Sensor.TYPE_LIGHT));
			t.add(new Integer(Sensor.TYPE_TEMPERATURE));
			t.add(new Integer(Sensor.TYPE_GYROSCOPE));
			t.add(new Integer(Sensor.TYPE_TEMPERATURE));
			t.add(new Integer(Sensor.TYPE_GRAVITY));
			t.add(new Integer(Sensor.TYPE_LINEAR_ACCELERATION));
			t.add(new Integer(Sensor.TYPE_MAGNETIC_FIELD));
			t.add(new Integer(Sensor.TYPE_PROXIMITY));
			t.add(new Integer(Sensor.TYPE_ROTATION_VECTOR));
			*/
		}
		else
			t = types;  	
		for (Integer type : t)
			addSensor(type.intValue()); 
		Thread thread = new Thread(null, this, "BotSensors");
		thread.start();
	}
	
	public void run() {
		synchronized(this) {
			try {
				this.wait(messageDelay);
				this.notifyAll();
			}
			catch(InterruptedException e) {
				e.printStackTrace();
			}
			carBotVision.sendCommand(byteArray());
		}
	}

	public String typeToString(int type) {
		if(type == Sensor.TYPE_ACCELEROMETER) //	A constant describing an accelerometer sensor type.
			return "TYPE_ACCELEROMETER " + type;
		else if(type == Sensor.TYPE_ALL) //		A constant describing all sensor types.
			return "TYPE_ALL " + type;
		else if(type == Sensor.TYPE_GRAVITY) //		A constant describing a gravity sensor type.
			return "TYPE_GRAVITY " + type;
		else if(type == Sensor.TYPE_GYROSCOPE) //		A constant describing a gyroscope sensor type
			return "TYPE_GYROSCOPE " + type;
		else if(type == Sensor.TYPE_LIGHT) //		A constant describing an light sensor type.
			return "TYPE_LIGHT " + type;
		else if(type == Sensor.TYPE_LINEAR_ACCELERATION) //		A constant describing a linear acceleration sensor type.
			return "TYPE_LINEAR_ACCELERATION " + type;
		else if(type == Sensor.TYPE_MAGNETIC_FIELD) //		A constant describing a magnetic field sensor type.
			return "TYPE_MAGNETIC_FIELD " + type;
		else if(type == Sensor.TYPE_ORIENTATION) //		 This constant is deprecated. use SensorManager.getOrientation() instead.
			return "TYPE_ORIENTATION " + type;
		else if(type == Sensor.TYPE_PRESSURE) //		A constant describing a pressure sensor type
			return "TYPE_PRESSURE " + type;
		else if(type == Sensor.TYPE_PROXIMITY) //		A constant describing an proximity sensor type.
			return "TYPE_PROXIMITY " + type;
		else if(type == Sensor.TYPE_ROTATION_VECTOR) //		A constant describing a rotation vector sensor type.
			return "TYPE_ROTATION_VECTOR " + type;
		else if(type == Sensor.TYPE_TEMPERATURE) //		 This constant is deprecated. use Sensor.TYPE_AMBIENT_TEMPERATURE instead.
			return "TYPE_TEMPERATURE " + type;
		else
			return "TYPE_UNKNOWN " + type;
	}
	
	private Sensor getSensor(int type) throws Exception {
		List<Sensor> sensors = sm.getSensorList(type);
		if (sensors != null && sensors.size() > 0) {
			return sensors.get(0);
		}
		else
			throw new Exception("Sensor of type " + type + " not found");
	}

	private void addSensor(int type) {
		try {
			Integer key = new Integer(type);
			sensors.put(key, getSensor(type));
			sm.registerListener(this, sensors.get(key), this.sensorDelay);
			Log.d(this.getClass().getName(), "registered sensor of type " + typeToString(type));
		}
		catch(Exception e) {
			e.printStackTrace();
		}
	}

	public void destroy() {
		if (sm != null) {
			sm.unregisterListener(this);
		}
	}

	public void onAccuracyChanged(Sensor sensor, int accuracy) {
		// TODO Auto-generated method stub
	}

	public void onSensorChanged(SensorEvent event) {
		float [] tmp = new float[event.values.length];
		for(int x = 0; x < event.values.length; x++)
			tmp[x] = event.values[x];
		values.put(new Integer(event.sensor.getType()), tmp);			
	}

	public float [] getValues(int type) {
		return values.get(new Integer(type));
	}	
	
	public Hashtable<Integer, float []> hashTable() {
		return values;
	}
	
	public byte [] byteArray() {
		// returns byte array with following encoding:
		// first byte of every message unit has the sensor type
		// second byte has the number <x> of sensor values
		// followed by <x> bytes containing the sensor values
		Enumeration<Integer> keys = values.keys();
		ArrayList<Byte> bytes = new ArrayList<Byte>();
		int c = 0;
		while(keys.hasMoreElements()) {
			Integer key = keys.nextElement();
			float [] vals = values.get(key);
			bytes.set(c, new Byte(key.byteValue()));
			c++;
			bytes.set(c, new Byte(new Integer(vals.length).byteValue()));
			c++;
			for(int x = 0; x < vals.length; x++) {
				bytes.set(c, new Byte(new Float(vals[x]).byteValue()));
				c++;
			}
		}
		int l = bytes.size();
		byte [] res = new byte[l];
		for(int x = 0; x < l; x++)
			res[x] = bytes.get(x).byteValue();
		return res;
	}
}