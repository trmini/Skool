import java.lang.*;
import java.util.*;
import java.net.*;
import myTCP;

/**
*   Computer Network - CS4873
*   Term Project - Fall 2001
*   Implementation of reliable network transport scheme based on UDP
*   Dien Trang Luu
**/

public class mySocket {
    private boolean Debug = true;
    private int hostPort = 2155;
    private int localPort = 2155;
    private String hostName = "localhost";
    private String localName = "localhost";
    private InetAddress hostAddr = null;
    private InetAddress localAddr = null;
    private DatagramSocket socket = null;

    ArrayList writeBuffer = new ArrayList(1000);
    ArrayList readBuffer = new ArrayList(1000);

    private volatile int MSS = 1000;
    private volatile int my_SEQ = 1000; /* Require some randomness here */
    private volatile int my_ACK = my_SEQ;
    private volatile int exp_SEQ = 0;
    private volatile int WINSZ = 1;
    private volatile int WINSZ_threshold = 20;
    private volatile int timeout = 500;
    private volatile int EST_time = timeout;
    private volatile int deviation = timeout/20;
    private DatagramPacket synPacket;
    private volatile boolean CLOSE_FLAG = false;

    public mySocket() {
      try {
      	localAddr = InetAddress.getByName(localName);
      } catch (Exception e) {
        localAddr = null;
      }
    }

    public void setLocalPort(int portNum) {
      localPort = portNum;
    }
    public int getLocalPort() {
      return localPort;
    }

    public void setHostPort(int portNum) {
      hostPort = portNum;
    }
    public int getHostPort() {
      return hostPort;
    }

    public void setHostName(String nameString) {
      hostName = nameString;
      try {
        hostAddr = InetAddress.getByName(hostName);
      } catch (Exception x) {
        hostAddr = null;
      }
    }
    public String getHostName() {
      return hostName;
    }
    public void listen(int portNum) throws Exception {
      setLocalPort(portNum);
      try {
        socket = new DatagramSocket(portNum);
      } catch (SocketException bind_error) {
        return;
      }
      localAddr = socket.getLocalAddress();
      try {
        socket.setSoTimeout(0);
      } catch (Exception settime_error) {
	    System.out.println("Cannot set timeout!");
		return;
	  }
      byte[] rcvBuffer = new byte[2*MSS];
      DatagramPacket rcvPacket = new DatagramPacket(rcvBuffer, 2*MSS);
      while (true) {
        try {
          socket.receive(rcvPacket);
        } catch (Exception x) {
	      System.out.println("Error while listening!");
	      return;
        }
        myTCP request = new myTCP(rcvPacket.getData());
        if (request.isSYN()) {
	  /* Construct granted_packet */
	  if (Debug) {
	    System.out.println("listen(): Received first SYN packet!");
	  }
	  exp_SEQ = request.getSEQ()+1;
	  hostAddr = rcvPacket.getAddress();
	  hostPort = rcvPacket.getPort();
	  myTCP granted = new myTCP();
	  granted.putSEQ(my_SEQ++);
	  granted.putACK(exp_SEQ);
	  granted.setSYN();
	  DatagramPacket grantedPacket = new DatagramPacket(granted.makePacket(),
	  		granted.getLength(), hostAddr, hostPort);
	  try {
	    socket.send(grantedPacket);
	  } catch (Exception e) {
	    System.out.println("Could not establish connection to host!");
	    return;
	  }
	  synPacket = grantedPacket;
	  /* the three-way handshake preserved */
	  break;
        }
        else {
	  /* Packet was not SYN */
	  if (Debug) {
	    System.out.println("listen(): Received a packet that is not SYN!");
	  }
	  continue;
	}
      }
      netRead serverReader = new netRead();
      netWrite serverWriter = new netWrite();
      serverReader.start();
      serverWriter.start();
    }


    public void connect(String nameString, int portNum) throws Exception {
      /* This will be run on a client
      *  Client will advertise its InetAddress and Port for
      *  server to send data
      **/
      /* Server Information */
      setHostName(nameString);
      setHostPort(portNum);

      /* Bind UDPSocket to local host */
      try {
      	socket = new DatagramSocket(localPort);
	/* Can be a random port, prefer 2155 for this project */
      } catch (SocketException e) {
        System.out.println("Cannot bind to local port:" + localPort);
	return;
      }
      setLocalPort(socket.getLocalPort());
      localAddr = socket.getLocalAddress();
      socket.connect(hostAddr, hostPort);
      myTCP request = new myTCP();
      request.putSEQ(my_SEQ++);
      request.putACK(0);
      request.setSYN();
      DatagramPacket requestPacket = new DatagramPacket(request.makePacket(),
      		request.getLength(), hostAddr, hostPort);
      try {
        socket.send(requestPacket);
      } catch (Exception e) {
        System.out.println("Could not make connection to host!");
	return;
      }
      byte[] rcvBuffer = new byte[2*MSS];
      DatagramPacket grantedPacket = new DatagramPacket(rcvBuffer, 2*MSS);
      try {
        socket.setSoTimeout(2000);
      } catch (Exception x) { }
      while (true) {
        try {
          socket.receive(grantedPacket);
        } catch (Exception x) {
	  /* Time's up */
	  try {
	    socket.send(requestPacket);
	  } catch(Exception e) {
              System.out.println("Could not make connection to host!");
	  }
	  continue;
        }
        myTCP granted = new myTCP(grantedPacket.getData());
	if ( granted.isSYN() && (granted.getACK() == my_SEQ) ) {
	  if (Debug) {
	    System.out.println("connect(): Received a SYN reply packet!");
	  }
	  exp_SEQ = granted.getSEQ() + 1;
	  my_ACK = my_SEQ;
	  break;
	}
	continue;
      }
      netRead clientReader = new netRead();
      netWrite clientWriter = new netWrite();
      clientWriter.start();
      clientReader.start();
    }

  public void close() throws Exception {
    synchronized (this) {
      CLOSE_FLAG = true;
    }
    socket.close();
  }

  public int write(byte[] data, int length) throws Exception {
    int index = 0;
    int max_index = length / MSS;
    byte[] PacketBuffer = new byte[MSS+1];
    for (index = 0; index < max_index; index++) {
      myTCP packet = new myTCP();
      System.arraycopy(data, index*MSS, PacketBuffer, 0, MSS);
      packet.putData(PacketBuffer, MSS);
      synchronized (this) {
        writeBuffer.add(packet);
      }
    }
    /* Construct the remainder of the data */
    if (length > index*MSS) {
      myTCP packet = new myTCP();
      System.arraycopy(data, index*MSS, PacketBuffer, 0, (length - index*MSS));
      packet.putData(PacketBuffer, (length - index*MSS));
      synchronized (this) {
        writeBuffer.add(packet);
      }
    }
    return length;
  }

  public int read(byte[] buf, int length) throws Exception {
    int byteread = 0;
    while (byteread < length) {
      if (readBuffer.isEmpty()) {
        Thread.sleep(15);
        continue;
      }
      myTCP packet = (myTCP) readBuffer.remove(0);
      if (packet.getDataLength() > 0) {
        System.arraycopy(packet.getData(), 0, buf, byteread, packet.getDataLength());
        byteread += packet.getDataLength();
      }
    }
    return byteread;
  }

  class netWrite extends Thread {
    int est_RTT = timeout;
    int deviation = timeout/10;
    long curr_Time;
  
    public netWrite() {
    }

    public void run() {
      while (!CLOSE_FLAG) {
        netwriter();
	try {
	  sleep(timeout);
	} catch (Exception sleep_err) {}
      }
    }

    private int netwriter() {
      int base_SEQ;
      int win_sz = WINSZ;
      int packet_num;
      synchronized (this) {
        packet_num = writeBuffer.size();
      }
      
      if (packet_num == 0) {
        /* Nodata in buffer, but needed to be able to send ACK*/
        myTCP ack = new myTCP();
        ack.putSEQ(my_SEQ);
	ack.putACK(exp_SEQ);
	ack.setACK();
	synchronized (this) {
	  my_SEQ++;
	}
	DatagramPacket ackOnly = new DatagramPacket(ack.makePacket(),
					ack.getLength(), hostAddr, hostPort);
	curr_Time = System.currentTimeMillis();
	while (ack.getACK() == exp_SEQ) {
	  try {
	    socket.send(ackOnly);
	  } catch (Exception send_error) {}
	  try {
	    sleep(timeout);
	  } catch (Exception sleep_err) {}
	}
	curr_Time = System.currentTimeMillis() - curr_Time;
	EST_time = (int) (0.875*EST_time + 0.125*curr_Time);
	deviation = (int) (0.875*deviation + 0.125*Math.abs(EST_time - curr_Time));
	synchronized(this) {
	  timeout = EST_time + 4*deviation;
	}
	return 1;
      }

      /* Can send data and ACK at the same time */
      base_SEQ = my_SEQ;
      myTCP[] packets = new myTCP[packet_num+1];
      for (int index = 0; index < packet_num; index++) {
        myTCP tmpTCP = (myTCP) writeBuffer.remove(0);
        packets[index] = new myTCP(tmpTCP.makePacket());
        synchronized (this) {
          packets[index].putSEQ(my_SEQ++);
	}
	/* Stamp SEQ, but leave ACK field blank */
      }
      
      int sent_SEQ;
      int packet_sent = 0;
      while ( packet_sent < packet_num ) {
	curr_Time = System.currentTimeMillis();
        for (int i = 0; i < win_sz; i++) {
	  if (i + packet_sent >= packet_num) break;
	  if (packets[i+packet_sent].getACK() == 0) {
	    synchronized (this) {
	      packets[i+packet_sent].putACK(exp_SEQ);
	    }
	  }
	  DatagramPacket packet = new DatagramPacket(packets[i+packet_sent].makePacket(),
	                   packets[i+packet_sent].getLength(), hostAddr, hostPort);
          try {
	    socket.send(packet);
	  } catch (Exception send_error) {
	    System.out.println(send_error);
	    return -1;
	  }
        } 

	int counter = 1;
        while (true) { /* Waiting to be ACKed */
	  if (my_ACK <= base_SEQ) {
	    if ((counter % 5) == 0) {
	      /* Wait for 5 timeouts to resend the whole windows */
	      win_sz = 1 + win_sz/2;
	      break;
	    }
	    try {
	      sleep(timeout);
	    } catch (Exception sleep_error) {}
	    counter ++;
	    continue;
          }
	  
	  curr_Time = System.currentTimeMillis() - curr_Time;
	  synchronized (this) {
	    curr_Time = curr_Time / (my_ACK - base_SEQ);
	  }
	  EST_time = (int) (0.875*EST_time + 0.125*curr_Time);
	  deviation = (int) (0.875*deviation + 0.125*Math.abs(EST_time - curr_Time));
	  synchronized(this) {
	    timeout = EST_time + 4*deviation;
	  }
	  if (my_ACK == my_SEQ) {
	    synchronized (this) {
	      WINSZ = win_sz;
	    }
	    return 1;
	  }
	  /* Slide window */
	  synchronized (this) {
	    packet_sent += (my_ACK - base_SEQ);
	    win_sz = 1 + 2*(my_ACK-base_SEQ);
	    base_SEQ = my_ACK;
	  }
	  if (win_sz > WINSZ_threshold) {
	    win_sz += 1;
	  }
	  break;
	}
      }	
      return 1;
    }
  }
  
  class netRead extends Thread {
	
    public netRead() {
    
    }
    
    public void run() {
      if (reader() == 0) {
        socket.close();
      }
    }
    
    private int reader() {
      ArrayList DataList = new ArrayList(15);
      ArrayList SEQInfo = new ArrayList(15);
      byte[] PacketBuffer = new byte[2*MSS];
      myTCP packet;
      DatagramPacket rcvPacket = new DatagramPacket(PacketBuffer, 2*MSS);
      try {
        socket.setSoTimeout(20000);
      } catch (Exception settime_error) {}

      while (!CLOSE_FLAG) {
        try {
          socket.receive(rcvPacket);
        } catch (Exception receive_error) {
	  System.out.println("Cannot get data from socket");
	  continue;
	}
        packet = new myTCP(rcvPacket.getData());
        if (packet.isSYN()) {
          continue;
        }

	if (packet.isFIN()) {
	  if (Debug) {
	    System.out.println("read(): receive a FIN packet");
	  }
	  synchronized (this) {
	    CLOSE_FLAG = true;
	  }
	  return 0;
	}

        /*if (packet.isACK()) {
	  if (packet.getACK() > my_ACK) {
	    synchronized (this) {
	      my_ACK = packet.getACK();
	    }
	    if (packet.getSEQ() == exp_SEQ) {
	      synchronized (this) {
	        exp_SEQ++;
	      }
	    }
	  }
	  continue;
	}*/
	
	if (packet.getACK() > my_ACK) {
	  if (Debug) {
	    System.out.println("reader(): Update ACK SEQ["  + packet.getSEQ() + "] ACK[" + packet.getACK() + "]");
	  }  
	  synchronized (this) {
	    my_ACK = packet.getACK();
	  }
	}  

        if (packet.getSEQ() < exp_SEQ ) {
	  if (Debug) {
	    System.out.println("reader(): Dup SEQ["  + packet.getSEQ() + "] ACK[" + packet.getACK() + "]");
	  }  
          continue;
        }

        if ( packet.getSEQ() == exp_SEQ ) {
	  if (Debug) {
	    System.out.println("reader(): Correct SEQ["  + packet.getSEQ() + "] ACK[" + packet.getACK() + "]");
	  }  
	  synchronized (this) {
            exp_SEQ++;
	    readBuffer.add(packet);
	  }
          int k = (int) SEQInfo.indexOf(new Integer(exp_SEQ));
          while (k >= 0) {
            SEQInfo.remove(k);
            packet = (myTCP) DataList.remove(k);
            synchronized (this) {
	      exp_SEQ++;
	      readBuffer.add(packet);
	    }
            k = (int) SEQInfo.indexOf(new Integer(exp_SEQ));
          }
	  continue;
        }

        if (packet.getSEQ() > exp_SEQ ) {
          /* Store packet into a buffer to be retrieved later */
          if ( !SEQInfo.contains(new Integer(packet.getSEQ())) ) {
	    if (!Debug) {
	      System.out.println("read(): Store packet[" + exp_SEQ + "] into buffer");
	    }
            SEQInfo.add(new Integer(packet.getSEQ()));
            DataList.add(packet);
          }
	  continue;
	}
      }
      return 1;
    }
  }
}
