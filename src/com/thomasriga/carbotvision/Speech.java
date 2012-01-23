package com.thomasriga.carbotvision;

import java.util.Locale;

import android.content.Context;
import android.speech.tts.TextToSpeech;

public class Speech implements TextToSpeech.OnInitListener
{
	private TextToSpeech tts;
	private boolean initOk = false;
	
	public Speech(Context context)
	{	
		this.tts = new TextToSpeech(context, this);
		tts.setLanguage(Locale.ITALY);
	}

	public void onInit(int status) {
		this.initOk = true;	
	}
	
	public void say(String msg) {
		if(initOk)
			tts.speak(msg, TextToSpeech.QUEUE_FLUSH, null);
	}
	
	public void shutdown() {
		tts.shutdown();
	}

}
