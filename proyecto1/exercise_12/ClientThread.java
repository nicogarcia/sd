package proyecto1_java;

import java.io.BufferedReader;
import java.net.InetAddress;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.InetSocketAddress;
import java.net.Socket;

import proyecto1_java.Message.MessageType;


public class ClientThread extends Thread {

	public static final int MAX_ATTEMPS = 5;
	
	private BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
	private Socket client_socket;
	private int buddy_port,
				port;
	
	private boolean connected;
	private String buddy_hostname;
	
	public ClientThread(int port){ this.port = port; }
	public ClientThread(int port, int buddy_port, String buddy_hostname)
	{
		this.port = port;
		this.buddy_hostname = buddy_hostname;
		this.buddy_port = buddy_port;
		this.connect();
	}
	
	private void connect(){
		int attemps = 0,
			backoff = 1000;
		System.out.print("Connecting ...");
		while(attemps < MAX_ATTEMPS && !connected){
		try{
			this.client_socket = new Socket(); /*conectar a un servidor en localhost con puerto 5000*/	
			this.client_socket.connect(new InetSocketAddress(this.buddy_hostname,  this.buddy_port));
			System.out.println("\nConnected with " + this.buddy_hostname + " " + this.buddy_port);
			connected = true;
		}catch(IOException ex){ 
			attemps++; 
			System.out.print("."); 
			backoff *= 1.5;
			try{ sleep(backoff); }catch(InterruptedException ie){ System.err.println("ERROR : " + ie.getMessage()); 
			} };
		}
		if(attemps == MAX_ATTEMPS)
			System.err.println("\nERROR : Failed to connect");
	}
			
	
	public void run()
	{
		String command,
			   args[];
		
		boolean exit = false;
		try{
			
			while(!exit){
				
				System.out.print("peer");
				if(connected)
					System.out.print("~");
				System.out.print("> ");
				
				args = reader.readLine().split(" ");
				command = args[0];

				//Comandos que requieren conexion
				if(command.equals("temp"))
					this.request(MessageType.GET_TEMP);
				else if(command.equals("time"))
					this.request(MessageType.GET_TIME);
				else if(command.equals("users"))
					this.request(MessageType.GET_USERS);
				else if(command.equals("uname"))
					this.request(MessageType.GET_UNAME);
				else if(command.equals("version"))
					this.request(MessageType.GET_SERV_VERSION);

				
				else if(command.equals("status"))
					this.status_screen();
				else if(command.equals("help"))
					this.help_screen();
				else if(command.equals("clear"))
					System.out.print("\033[H\033[2J");
				else if(command.equals("exit")){
						if(this.connected)
						this.request(MessageType.CLOSE_CONN);
						exit = true;
					}
				else if(command.equals("close"))
						this.request(MessageType.CLOSE_CONN);
				else if(command.length() > 2 && "connect".startsWith(command)  ){
						if(args.length == 3){
							this.buddy_hostname = args[1];
							this.buddy_port = Integer.parseInt(args[2]);
						}				
						if(buddy_hostname != null && buddy_port > 0)
							this.connect();							
						else
							System.err.println("ERROR : Can't connect with peer.");
					}
				else
					System.out.println("Command is not valid.");
			}
		}catch(IOException ex){ System.err.println("ERROR : " + ex.getMessage()); };
	}
	
	private void help_screen()
	{
		String margin = "       ";
		
		System.out.print("CONNECTION\n");
		System.out.print("connect: connect [buddy_hostname] [buddy_port]\n");
		System.out.print(margin);
		System.out.print("Connect with a process in [buddy_hostname] with port number [buddy_port]\n");

		System.out.print("close: close\n");
		System.out.print(margin);
		System.out.print("Close connection with peer.\n");

		System.out.print("\nAUXILIAR\n");
		System.out.print("status: status\n");
		System.out.print(margin);
		System.out.print("Show actual state.\n");

		System.out.print("clear: clear\n");
		System.out.print(margin);
		System.out.print("Clear terminal screen.\n");

		System.out.print("exit: exit\n");
		System.out.print(margin);
		System.out.print("Finish actual process.\n");

		System.out.print("\nMESSAGES: Need connection. When connected pair~>. \n");

		System.out.print("users: users\n");
		System.out.print(margin);
		System.out.print("Show peer's logged users.\n");

		System.out.print("time: time\n");
		System.out.print(margin);
		System.out.print("Show peer's system time.\n");

		System.out.print("uname: uname\n");
		System.out.print(margin);
		System.out.print("Show peer's machine name.\n");

		System.out.print("version: version\n");
		System.out.print(margin);
		System.out.print("Request peer's version.\n");

		System.out.print("temp: temp\n");
		System.out.print(margin);
		System.out.print("Show peer's temperature.\n");
		
	}


	private void status_screen()
	{
		System.out.print("ME\n");
		System.out.print("  PORT: " + this.port + "\n");
		try{
			System.out.print("  HOSTNAME: " + InetAddress.getLocalHost().getHostName() + "\n");
		}catch(Exception ex) { System.out.println(ex.getMessage()); }
		
		System.out.print("PEER\n");
		System.out.print("  PORT: " + this.buddy_port + "\n");
		System.out.print("  HOSTNAME: " + this.buddy_hostname +"\n");

		System.out.print("STATE: ");
		if(connected)
			System.out.print("CONNECTED\n");
		else
			System.out.print("NO CONNECTED\n");

	}
	
	public byte[] intToByteArray(int value) {
	    return new byte[] {
	            (byte)(value >>> 24),
	            (byte)(value >>> 16),
	            (byte)(value >>> 8),
	            (byte)value};
	}
	
	private void request(MessageType type) 
		   throws IOException
	{
		byte[] inData = new byte[256];//allocate space for the entire message
		String resp;
		long start, end, waiting_time;
		int bytesRead = -1;
		DataOutputStream output;
		DataInputStream input;
	
		if(this.connected)
		{
			output = new DataOutputStream(this.client_socket.getOutputStream());
			input = new DataInputStream(this.client_socket.getInputStream());
			
			start = System.nanoTime();
			output.write(intToByteArray(type.ordinal()), 0, 4);					
			input.read(inData, 0, 256);
			end = System.nanoTime();
			waiting_time = (end - start) / 1000;
			
			resp = new String(inData);			
			System.out.print(resp);	
			System.out.print("Waiting time: " + waiting_time + "us.\n");
			
			switch (type) {
			case CLOSE_CONN:
			{
				this.client_socket.close();
				this.connected = false;
				break;
			}
			}
		}
		else System.err.println("ERROR : Need connection.");
	}
	
}
