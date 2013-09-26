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
		try{
			this.client_socket = new Socket(); /*conectar a un servidor en localhost con puerto 5000*/	
			this.client_socket.connect(new InetSocketAddress(this.buddy_hostname,  this.buddy_port));
			System.out.println("Conexión establecida con " + this.buddy_hostname + " " + this.buddy_port);
			connected = true;
		}catch(IOException ex){ System.err.println("ERROR : " + ex.getMessage()); };
	}
			
	
	public void run()
	{
		String command,
			   args[];
		
		boolean exit = false;
		try{
			
			while(!exit){
				
				System.out.print("(pair");
				if(connected)
					System.out.print("~");
				System.out.print(")");
				
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
							System.err.println("ERROR : Falta especificar puerto y hostname del compañero");
					}
				else
					System.out.println("El comando ingresado no es valido");
			}
		}catch(IOException ex){ System.err.println("ERROR : " + ex.getMessage()); };
	}
	
	private void help_screen()
	{
		String margin = "       ";
		System.out.print("connect: connect [buddy_hostname] [buddy_port]\n");
		System.out.print(margin);
		System.out.print("Intenta establecer una conexión con el proceso situado en [buddy_hostname] con número de puerto [buddy_port]\n");

		System.out.print("status: status\n");
		System.out.print(margin);
		System.out.print("Muestra el estado actual.\n");

		System.out.print("clear: clear\n");
		System.out.print(margin);
		System.out.print("Limpia la pantalla de la consola.\n");

		System.out.print("exit: exit\n");
		System.out.print(margin);
		System.out.print("Termina la ejecución actual.\n");

		System.out.print("\nMensajes: Para intercambiar mensajes con otro proceso se necesita que exista una conexión\n");
		System.out.print(margin);
		System.out.print("Cuando exista una conexión se notara (pair~). \n\n");
		System.out.print("users: users\n");
		System.out.print(margin);
		System.out.print("Muestra los usuarios logueados en el sistema par.\n");

		System.out.print("time: time\n");
		System.out.print(margin);
		System.out.print("Muestra la hora del sistema par.\n");

		System.out.print("uname: uname\n");
		System.out.print(margin);
		System.out.print("Muestra el nombre de la maquina par.\n");

		System.out.print("temp: temp\n");
		System.out.print(margin);
		System.out.print("Muestra la temperatura actual de la localidad de la maquina par.\n");

		System.out.print("version: version\n");
		System.out.print(margin);
		System.out.print("Muestra la version del proceso par.\n");
		
		System.out.print("close: close\n");
		System.out.print(margin);
		System.out.print("Cierra la conexión con el par.\n");
		
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
		int bytesRead = -1;
		DataOutputStream output;
		DataInputStream input;
	
		if(this.connected)
		{
			output = new DataOutputStream(this.client_socket.getOutputStream());
			input = new DataInputStream(this.client_socket.getInputStream());
			
			output.write(intToByteArray(type.ordinal()), 0, 4);					
			input.read(inData, 0, 256); 
			resp = new String(inData);			
			System.out.print(resp);	
						
			switch (type) {
			case CLOSE_CONN:
			{
				this.client_socket.close();
				this.connected = false;
				break;
			}
			}
		}
		else System.err.println("ERROR : El intercambio de mensajes solo puede realizarse cuando existe una conexión");
	}
	
}
