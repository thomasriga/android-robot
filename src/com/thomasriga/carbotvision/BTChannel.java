package com.thomasriga.carbotvision;

import android.content.Context;
import android.content.Intent;
import at.abraxas.amarino.Amarino;
import at.abraxas.amarino.AmarinoIntent;
import android.content.BroadcastReceiver;
import android.util.Log;

public class BTChannel extends BroadcastReceiver implements CommChannel
{
	private Context context;
	private static final String DEVICE_ADDRESS = "00:07:80:49:7E:6A";
	
	public BTChannel(Context context)
	{	
		this.context = context;
	}

	public void connect() {
		Amarino.connect(this.context, DEVICE_ADDRESS);
	}
	
	
	public void send(int cmd) {
    	Amarino.sendDataToArduino(this.context, DEVICE_ADDRESS, 'A', cmd);
	}

	public void send(int [] cmds) {
    	Amarino.sendDataToArduino(this.context, DEVICE_ADDRESS, 'A', cmds);
	}
	
	public void disconnect() {
		Amarino.disconnect(this.context, DEVICE_ADDRESS);
	}

	@Override
	public void onReceive(Context context, Intent intent) {
        String data = null;
        final String address = intent.getStringExtra(AmarinoIntent.EXTRA_DEVICE_ADDRESS);
        final int dataType = intent.getIntExtra(AmarinoIntent.EXTRA_DATA_TYPE, -1);
        if (dataType == AmarinoIntent.STRING_EXTRA) {
                data = intent.getStringExtra(AmarinoIntent.EXTRA_DATA);
                if (data != null) {
                        // do whatever you want to do with the data
                        Log.i(this.getClass().getName(), data);
                }
        }
	}
}
