package proyecto1_java;

public class Peer {

	public static final String VERSION = "NAME\n   Peer implemented with Java\nAUTHORS\n   Trippel Nagel Juan Manuel\n   Garcia Nicolas\n";
	
	private ServerThread server_thread;
	private ClientThread client_thread;

	public Peer(int port, int buddy_port, String buddy_hostname)
	{
		try{
			this.server_thread = new ServerThread(port);
			this.client_thread = new ClientThread(port, buddy_port, buddy_hostname);
	
			this.server_thread.start();
			this.client_thread.start();
			
			this.client_thread.join();
			
		}catch(Exception ex){System.out.print(ex.getMessage());}
	}
	
	public Peer(int port)
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
		if(args.length > 2){
			if(args[1].equals("-s"))
				new Peer(Integer.parseInt(args[0]), Integer.parseInt(args[3]), args[2]);
			else
				System.out.println("usage: Pair port [-s buddy_hostname buddy_port]");
		}
		else if(args.length == 0)
				System.out.println("usage: Pair port [-s buddy_hostname buddy_port]");
		else
			new Peer(Integer.parseInt(args[0]));
		
	}

}

