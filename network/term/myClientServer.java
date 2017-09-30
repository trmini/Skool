import java.lang.*;
import java.io.*;
import java.net.*;
import myTCP;

class myClientServer {
  public static void main(String[] args) throws Exception
  {
    int PortNum = 51979;
    int PackSize = 1024;
    int PackNum = 1000; 
    if ( (args.length >= 1) && (args[0].length() == 2) && (args[0].equals("-s")) ) {
      try {
        PortNum = Integer.parseInt(args[1]);
      }
      catch (Exception error) {
        System.out.println(error);
	return;
      }
      server(PortNum);
    }
    else if ( (args.length >= 3) && (args[0].length() == 2) && (args[0].equals("-c")) ) {
      try {
        PortNum = Integer.parseInt(args[2]);
        if ( args.length >= 4 ) {
	  PackSize = Integer.parseInt(args[3]);
	  if ( args.length >= 5) {
	    PackNum = Integer.parseInt(args[4]);
	  }
	}
      } catch (Exception error) {
        System.out.println(error);
      }
      client(args[1], PortNum, PackSize, PackNum);
    }  
    else {
      System.out.println("Usage: myClientServer -c <IP Address> <Port Num> <Packet Size = 1024> <Iterration = 1000>");
      System.out.println("or");
      System.out.println("       myClientServer -s <Port Num>");
    }
  }

  public static int server(int PortNum) throws Exception
  {
    if (PortNum <= 1024) {
      System.out.println("Invalid port number!");
      System.out.println("Exiting...");
      return 0;
    }
    ServerSocket welcomeSocket;
    try {
      welcomeSocket = new myServerSocket (PortNum);
    } catch (IOException error) {
      System.out.println("Could not listen on port " + PortNum);
      System.out.println("Exiting...");
      return 0;
    }
    System.out.println("Server is listening on port " + PortNum + "...");
    while(true) {
      mySocket connectionSocket = welcomeSocket.accept();

      DataInputStream inFromClient = new DataInputStream(connectionSocket.getInputStream());
      DataOutputStream outToClient = new DataOutputStream(connectionSocket.getOutputStream());

      int PackSize = 0;
      try {
        PackSize = inFromClient.readInt();
      }
      catch (IOException error) {
        connectionSocket.close();
        continue;
      }

      byte[] BufferData = new byte[PackSize];
      while(true) {
	try {
          inFromClient.readFully(BufferData, 0, PackSize);
          outToClient.write(BufferData, 0, PackSize);
	} catch (IOException error) {
	  connectionSocket.close();
	  break;
	}
      }
    }
  }

  public static void client(String HostName, int PortNum, int PackSize, int PackNum) throws Exception
  {
    if (PortNum <= 0 || PackSize <= 0 || PackNum <= 0) {
      System.out.println("Invalid argument!");
      return;
    }

    mySocket clientSocket;

    try {
      clientSocket = new mySocket(HostName, PortNum);
    } 
    catch(IOException error) {
      System.out.println("Could not make connection to " + HostName + " at port " + PortNum);
      System.out.println("Exiting...");
      return;
    }

    DataOutputStream outToServer = new DataOutputStream(clientSocket.getOutputStream());
    DataInputStream inFromServer = new DataInputStream(clientSocket.getInputStream());

    try {
      outToServer.writeInt(PackSize);
    }
    catch (IOException error) {
      System.out.println("Lost connection with host...");
      System.out.println("Test is not completed!");
      return;
    }

    int i = 0;
    byte[] BufferData = new byte[PackSize];
    long time_delay = 0;

    time_delay = System.currentTimeMillis();
    for(i = 0; i < PackNum; i++) {
      try {
        outToServer.write(BufferData, 0, PackSize);
        inFromServer.readFully(BufferData, 0, PackSize);
      } catch (IOException error) {
        System.out.println("Lost connection with host...");
        System.out.println("Actual number ofpackages sent: " + i);
        break;
      }
    }
    time_delay = System.currentTimeMillis() - time_delay;
    clientSocket.close();

    /* Print out statistic here*/
    /* System.out.println("Message size: " + PackSize + " byte(s)");
    System.out.println("Latency per packet:\t" + (double) time_delay/(2*i) + " ms");
    System.out.println("Throughput:\t" + (double) (16*PackSize*i)/(1024*time_delay) + " Mbs"); */
    /* Print out compact statistic here */
    System.out.println(PackSize + ", " + ((double) time_delay/(2*i)) + ", " + ((double) (16*PackSize*i)/(1024*time_delay)));
    return;
  }
}
