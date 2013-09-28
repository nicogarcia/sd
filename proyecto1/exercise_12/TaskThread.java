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
	private double temp = (new Random().nextDouble() * 100 ) % 27;
	
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
			request = MessageType.values()[input.readInt()];
			
			switch (request) {
			case GET_TEMP:
			{
				answer = String.format("Temperature: %.2f°C\n", temp + (new Random().nextDouble() * 100 ) % 5 );
				break;	
			}
			case GET_TIME:
			{
				Calendar cal = Calendar.getInstance();
				cal.getTime();
				SimpleDateFormat sdf = new SimpleDateFormat("yyyy.MM.dd G 'at' HH:mm:ss z");
				answer = sdf.format(cal.getTime()) + "\n";
				break;
			}
			case GET_USERS:
			{
				answer = System.getProperty("user.name") + "\n";
				break;
			}
			case GET_UNAME:
			{
				answer = "Node: " + InetAddress.getLocalHost().getHostName() + "\n";
				answer += "Operating system: " + System.getProperty("os.name") + "\n";
				break;
			}
			case CLOSE_CONN:
			{
				answer = "Connection closed\n";
				connected = false;
				break;
			}
			case GET_SERV_VERSION:
			{
				answer = Peer.VERSION;
				break;
			}		
			}
			output.write(answer.getBytes());
		}
		
		client_socket.close();
	}
	
}
