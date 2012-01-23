package com.thomasriga.carbotvision;

public interface CommChannel {
	public void connect();
	public void send(int cmd);
	public void send(int [] cmds);
	public void disconnect();
}
