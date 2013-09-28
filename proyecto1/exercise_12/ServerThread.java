package proyecto1_java;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.LinkedList;


public class ServerThread extends Thread {
	
	private ServerSocket server_socket;
	private LinkedList<TaskThread> workers = new LinkedList<TaskThread>();
	private int port;
	
	public ServerThread(int port)
	{
		this.port = port;
	}
	
	public void run()
	{
		try{
			System.out.println("Startup socket ...");
			this.server_socket = new ServerSocket(this.port); /* crea socket servidor que escuchara en puerto 5000*/
			this.listen();
		}catch(IOException ex){ System.err.println(ex.getMessage()); };
	}
	
	private void listen()
			throws IOException
	{
		Socket client_socket;
		TaskThread task_t;
		
		while(true)
		{
			client_socket = this.server_socket.accept(); /* Espero por una conexion */
			task_t = new TaskThread(client_socket);
			task_t.run();
			this.workers.add(task_t);
		}
	}
	
	
	
}


