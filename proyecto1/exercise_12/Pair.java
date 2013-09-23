package proyecto1_java;
public class Pair {
	
	private ServerThread server_thread;
	private ClientThread client_thread;

	public Pair(int port, int buddy_port, String buddy_hostname)
	{
		try{
			this.server_thread = new ServerThread(port);
			this.client_thread = new ClientThread(port, buddy_port, buddy_hostname);
	
			this.server_thread.start();
			this.client_thread.start();
			
			this.client_thread.join();
			
		}catch(Exception ex){System.out.print(ex.getMessage());}
	}
	
	public Pair(int port)
	{
		try{
			this.server_thread = new ServerThread(port);
			this.client_thread = new ClientThread(port);
	
			this.server_thread.start();
			this.client_thread.start();
			
			this.client_thread.join();
			
		}catch(Exception ex){System.out.print(ex.getMessage());}
	}
	
	public static void main(String args[]) {
		
		if(args.length == 3){
			new Pair(Integer.parseInt(args[0]), Integer.parseInt(args[2]), args[1]);
		}
		else
			new Pair(Integer.parseInt(args[0]));
		
	}

}

