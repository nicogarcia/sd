package proyecto1_java;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.InetAddress;
import java.net.Socket;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Random;

import proyecto1_java.Message.MessageType;

public class TaskThread extends Thread {
	
	private Socket client_socket;
	
	public TaskThread(Socket client_socket){
		this.client_socket = client_socket;
	}
	
	
	public void run(){
		try{
			this.task();
		}catch(IOException ex){};
	}
	
	private void task()
			throws IOException
	{
		DataInputStream input;
		DataOutputStream output;
		MessageType request;
		String answer = null;
		boolean connected = true;
		
		//Canales de input y output de datos
		input = new DataInputStream(client_socket.getInputStream());
		output = new DataOutputStream(client_socket.getOutputStream());
		
		while(connected){
			//Recepcion de mensaje
			request = MessageType.values()[input.read()];
			switch (request) {
			case GET_TEMP:
			{
				answer = String.valueOf(((new Random().nextDouble() * 100 ) % 27));
				break;	
			}
			case GET_TIME:
			{
				Calendar cal = Calendar.getInstance();
				cal.getTime();
				SimpleDateFormat sdf = new SimpleDateFormat("HH:mm:ss");
				answer = sdf.format(cal.getTime());
				break;
			}
			case GET_USER:
			{
				answer = System.getProperty("user.name");
				break;
			}
			case GET_UNAME:
			{
				answer = InetAddress.getLocalHost().getHostName();
				break;
			}
			case CLOSE:
			{
				answer = "Cerrando la conexión";
				connected = false;
				break;
			}
			default:
				break;
			}
			output.writeUTF(answer);
		}
		
		client_socket.close();
	}
	
}
