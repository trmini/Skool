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

    private volatile int MSS = 100;
    private volatile int my_SEQ = 1000; /* Require some randomness here */
    private volatile int my_ACK = my_SEQ;
    private volatile int exp_SEQ = 0;
    private volatile int WINSZ = 20;
    private volatile int timeout = 2500;
    private DatagramPacket synPacket;

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

    public int write_tmp(byte[] data, int length) throws Exception {
    /* Asumption: already knew host InetAddress, Port, and more ... */
      int index = 0;
      int max_index = length / MSS;
      int curr_SEQ = my_SEQ;
      DatagramPacket[] packets = new DatagramPacket[max_index+1];
      byte[] PacketBuffer = new byte[2*MSS];
      myTCP packet = new myTCP();
      for (index = 0; index < max_index; index++) {
	System.arraycopy(data, index*MSS, PacketBuffer, 0, MSS);
        packet.putSEQ(curr_SEQ++);
	packet.putACK(exp_SEQ);
        packet.putData(PacketBuffer, MSS);
        /* Construct DatagramPacket for sending */
        packets[index] = new DatagramPacket(packet.makePacket(),
        	packet.getLength(), hostAddr, hostPort);
      }
      /* Construct the remainder of the data */
	  if (length > index*MSS) {
        System.arraycopy(data, index*MSS, PacketBuffer, 0, (length - index*MSS));
        packet.putSEQ(curr_SEQ++);
        packet.putACK(exp_SEQ);
        packet.putData(PacketBuffer, (length - index*MSS));
        packets[index] = new DatagramPacket(packet.makePacket(),
		  	packet.getLength(), hostAddr, hostPort);
	  }

      /** Done packets[] construction
      *   Ready to perform reliable data_send()
      **/
      if (Debug) {
        System.out.println("write(): Finish constructing send packets!");
      }
      int win_sz = WINSZ;
      int offset = 0;
      int est_RTT = 20000;
      int deviation = 100;
      int timeout = est_RTT + 4*deviation;
      DatagramPacket rcvPacket = new DatagramPacket(PacketBuffer, 2*MSS);
      while (true) {
        if (offset > max_index) {
            if (Debug) {
	      System.out.println("write(): finish length = " + length);
            }
	    return length;
        }
        for (int i = 0; i < win_sz; i++) {
          if ( (i + offset) > max_index) break;
          try {
	        if (Debug) {
		  System.out.println("write(): write packet[" + (exp_SEQ+i) + "]"
		   + " in window_index[" + i + "] of packet_oder[" + (i+offset) + "]!");
		}
	        socket.send(packets[i + offset]);
	      } catch (Exception x) {
	        System.out.println("write(): Connection lost!");
	        return -1;
	      }
        }
	    long sample_RTT = System.currentTimeMillis();
	    while (true) {
	      try {
	        socket.setSoTimeout(timeout);
	      } catch (Exception settime_error) {}
	      try {
	        socket.receive(rcvPacket);
		  } catch (Exception time_exp) {
		    /* Timeout */
	        break;
		  }
          sample_RTT = System.currentTimeMillis() - sample_RTT;
          myTCP ack = new myTCP(rcvPacket.getData());
          if (ack.isACK()) {
            if (Debug) {
	      System.out.println("write(): receive ACK = " + ack.getACK());
            }
            if (ack.getACK() >= my_SEQ) {
              deviation = (int) (0.875*deviation + 0.125*Math.abs(est_RTT - sample_RTT));
              est_RTT = (int) (0.875*est_RTT + 0.125*sample_RTT);
	      timeout = est_RTT + 4*deviation;
              offset = offset + (ack.getACK() - my_SEQ);
              if (Debug) {
		System.out.println("write(): package sent = " + offset);
              }
              my_SEQ = ack.getACK();
		      break;
            }
	        else {
			  /* Receive ACK that is already ACKed */
			  timeout = 1 + timeout/2;
	          continue;
	        }
	      }
	      else continue;
	    }
      }
    }

    public int read_tmp(byte[] buf, int length) throws Exception{
      int est_RTT = 2000;
      int deviation = 100;
      int timeout = est_RTT + 4*deviation;
	  long sample_RTT = System.currentTimeMillis();
      int byteread = 0;
      ArrayList DataList = new ArrayList(15);
      ArrayList SEQInfo = new ArrayList(15);
      myTCP packet;
      byte[] PacketBuffer = new byte[2*MSS];
      DatagramPacket rcvPacket = new DatagramPacket(PacketBuffer, 2*MSS);
      DatagramPacket ackPacket = synPacket;

	  try {
        socket.setSoTimeout(timeout);
      } catch (Exception settime_error) {}

	  while (byteread < length) {
	    try {
          socket.receive(rcvPacket);
        } catch (Exception time_exp) {
          try {
	        socket.send(ackPacket);
			sample_RTT = System.currentTimeMillis();
	      } catch (Exception x) {
		    System.out.println("Connection to host lost!");
			return -1;
		  }
	      continue;
	    }
        packet = new myTCP(rcvPacket.getData());

        if (byteread == 0 && packet.isSYN()) {
	  if (Debug) {
	    System.out.println("read(): receive 2nd SYN from client!");
	  }
	      try {
            socket.send(synPacket);
          } catch (Exception send_error) {
            System.out.println("Connecton lost!! Send SYN fail");
            return -1;
          }
          continue;
        }

        if (packet.getSEQ() < exp_SEQ ) {
	  if (Debug) {
	    System.out.println("read(): receive duplicated packet[" + packet.getSEQ() + "]!");
	  }
          timeout = 1 + timeout/2;
          try {
            socket.setSoTimeout(timeout);
          } catch (Exception settime_error) {}
          continue;
        }

        if ( packet.getSEQ() == exp_SEQ ) {
	  if (Debug) {
	    System.out.println("read(): receive correct in order packet!");
	  }
	  sample_RTT = sample_RTT - System.currentTimeMillis();
	  deviation = (int) (0.875*deviation + 0.125*Math.abs(est_RTT - sample_RTT));
          est_RTT = (int) (0.875*est_RTT + 0.125*sample_RTT);
          timeout = est_RTT + 4*deviation;
          exp_SEQ++;
          System.arraycopy(packet.getData(), 0, buf, byteread, packet.getDataLength());
          byteread += packet.getDataLength();
          int k = (int) SEQInfo.indexOf(new Integer(exp_SEQ));
          while (k >=0) {
	    if (Debug) {
	      System.out.println("read(): retrieve packet[" + exp_SEQ + "] from buffer");
	    }
            SEQInfo.remove(k);
            packet = (myTCP) DataList.remove(k);
            System.arraycopy(packet.getData(), 0, buf, byteread, packet.getDataLength());
            byteread += packet.getDataLength();
            exp_SEQ++;
            k = (int) SEQInfo.indexOf(new Integer(exp_SEQ));
          }
          myTCP ack = new myTCP();
          ack.putSEQ(my_SEQ);
          ack.putACK(exp_SEQ);
          ack.setACK();
          ackPacket = new DatagramPacket(ack.makePacket(), ack.getLength(), hostAddr,
	  				hostPort);
	      continue;
        }

        if (packet.getSEQ() > exp_SEQ ) {
          /* Store packet into a buffer to be retrieved later */
	  if (Debug) {
	    System.out.println("read(): Store packet[" + exp_SEQ + "] into buffer");
	  }
          if ( !SEQInfo.contains(new Integer(packet.getSEQ())) ) {
            SEQInfo.add(new Integer(packet.getSEQ()));
            DataList.add(packet);
          }
	      continue;
	    }
	    break;
      }
      if (Debug) {
         System.out.println("read(): done, byteread = " + byteread);
      }
      try {
        socket.send(ackPacket);
	sample_RTT = System.currentTimeMillis();
      } catch (Exception x) {
         System.out.println("Connection to host lost!");
  	 return -1;
      }
      return length;
    }

  public void close() throws Exception {
    socket.close();
  }

  public int write(byte[] data, int length) throws Exception {
    int index = 0;
    int max_index = length / MSS;
    myTCP packet = new myTCP();
    byte[] PacketBuffer = new byte[MSS+1];
    for (index = 0; index < max_index; index++) {
      System.arraycopy(data, index*MSS, PacketBuffer, 0, MSS);
      packet.putData(PacketBuffer, MSS);
      synchronized (this) {
        writeBuffer.add(packet);
      }
    }
    /* Construct the remainder of the data */
    if (length > index*MSS) {
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
    myTCP packet;
    while (byteread < length) {
      if (readBuffer.isEmpty()) {
        Thread.sleep(timeout);
        continue;
      }
      synchronized (this) {
        packet = (myTCP) readBuffer.remove(0);
      }
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
      while (true) {
        netwriter();
	try {
	  sleep(timeout);
	} catch (Exception sleep_err) {}
      }
    }

    private int netwriter() {
      int packet_num;
      int base_SEQ;
      int win_sz = WINSZ;
      int ack_num = 0;
      synchronized (this) {
        packet_num = writeBuffer.size();
      }
      base_SEQ = my_SEQ;
      ArrayList windows = new ArrayList(WINSZ+1);
      myTCP[] packets = new myTCP[packet_num+1];
      
      if (packet_num == 0) {
        /* Nodata in buffer, but needed to be able to send ACK*/
        packets[0] = new myTCP();
        packets[0].putSEQ(my_SEQ);
	packets[0].putACK(exp_SEQ);
	packets[0].setACK();
	synchronized (this) {
	  my_SEQ++;
	}
	DatagramPacket ackOnly = new DatagramPacket(packets[0].makePacket(),
		packets[0].getLength(), hostAddr, hostPort);
	while (packets[0].getACK() == exp_SEQ) {
	  try {
	    sleep(timeout);
	  } catch (Exception sleep_err) {}
	  try {
	    if (Debug) {
	      System.out.println("ACKonly: [" + packets[0].getSEQ() + "][" + packets[0].getACK() + "]");
	    }
	    socket.send(ackOnly);
	  } catch (Exception send_error) {}
	}
	return 1;
      }
      
      /* Can send data and ACK at the same time */
      for (int index = 0; index < packet_num; index++) {
        myTCP tmpTCP = (myTCP) writeBuffer.remove(0);
        packets[index] = new myTCP(tmpTCP.makePacket());
        packets[index].putSEQ(my_SEQ);
        synchronized (this) {
	  my_SEQ++;
	}
	/* Put SEQ, but leave ACK blank */
      }
      int sent_SEQ;
      for (int i = 0; i < win_sz; i++) {  /* Constructing first window with ACK */
        if ( i >= packet_num ) break;         /* is current exp_SEQ */
	synchronized (this) {
	  packets[i].putACK(exp_SEQ);
	}
	windows.add(new DatagramPacket(packets[i].makePacket(),
	                packets[i].getLength(), hostAddr, hostPort));
      }
      /* Keep going until finishing current data */
      while ( ack_num < packet_num ) {
        if (windows.size() == 0) return 1;
        for (int i = 0; i < windows.size(); i++) {
          try {
	    socket.send((DatagramPacket) windows.get(i));
	    if (Debug) {
	      DatagramPacket tmpPacket = (DatagramPacket) windows.get(i);
	      myTCP tmpTCP = new myTCP(tmpPacket.getData());
	      System.out.println("Sent[" + i + "]: SEQ[" + tmpTCP.getSEQ() + "] ACK["
	                       + tmpTCP.getACK() + "]");
            }
	  } catch (Exception send_error) {
	    System.out.println(send_error);
	    return -1;
	  }
        } 
	DatagramPacket tmpPacket = (DatagramPacket) windows.get(0);
	myTCP tmpTCP = new myTCP(tmpPacket.getData());
	base_SEQ = tmpTCP.getSEQ();
	tmpPacket = (DatagramPacket) windows.get(windows.size()-1);
	tmpTCP = new myTCP(tmpPacket.getData());
	sent_SEQ = tmpTCP.getSEQ();

	int counter = 1;
        while (true) { /* Waiting to be ACKed */
	  if (my_ACK <= base_SEQ) {
	    if ((counter % 3) == 0) {
	      /* Resend windows */
	      break;
	    }
	    try {
	      sleep(timeout);
	    } catch (Exception time_exp) {}
	    counter ++;
          }
	  
	  if (my_ACK == my_SEQ) {
	    return 1;
	  }
	  /* Sliding window */
	  int ack_in_win;
	  synchronized (this) {
	    ack_in_win = (my_ACK - base_SEQ);
	    /* ack_num = number of packet in windows have been ACKed */
	  }
	  ack_num += ack_in_win;
	  for (int i =  ack_in_win; i > 0; i--) {
	    if (!windows.isEmpty()) {
	      windows.remove(0);
	      continue;
	    }
	    break;
	  }
	  int left_in_win = windows.size();
	  for (int i = 0; i < win_sz - left_in_win; i++) {
	    int index = i + ack_num + left_in_win;
            if (index >= packet_num) break;
	      synchronized (this) {
	        if (Debug) {
		  System.out.println("index = "+index+ "ack_num=" +ack_num +
		                      " leftinwin=" +left_in_win+" i=" + i);
		}
	        packets[index].putACK(exp_SEQ);
	      }
	      windows.add(new DatagramPacket(packets[index].makePacket(),
	                packets[index].getLength(), hostAddr, hostPort));
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
        socket.setSoTimeout(0);
      } catch (Exception settime_error) {}

      while (true) {
        try {
          socket.receive(rcvPacket);
        } catch (Exception receive_error) {
	  System.out.println("Cannot get data from socket");
	  continue;
	}
        packet = new myTCP(rcvPacket.getData());
	if (Debug) {
	  System.out.println("A Packet SEQ[" + packet.getSEQ() + "] ACK[" +
	                 packet.getACK() + "]");
	}
        if (packet.isSYN()) {
	  if (Debug) {
	    System.out.println("read(): receive a SYN packet!");
	  }
          continue;
        }

	if (packet.isFIN()) {
	  if (Debug) {
	    System.out.println("read(): receive a FIN packet");
	  }
	  return 0;
	}
	
	synchronized (this) {
	  if (packet.getACK() > my_ACK) {
	    if (Debug) {
	      System.out.println("reader(): Receive a ACK["  + packet.getACK() + "] SEQ[" + packet.getSEQ() + "]");
	    }
	    my_ACK = packet.getACK();
	  }
	}  

        if (packet.getSEQ() < exp_SEQ ) {
	  if (Debug) {
	    System.out.println("read(): dup_packet SEQ[" + packet.getSEQ() + "], ACK["+packet.getACK()+"]!");
	  }
          continue;
        }
        if ( packet.getSEQ() == exp_SEQ ) {
	  synchronized (this) {
            exp_SEQ++;
	    readBuffer.add(packet);
	  }
	  if (Debug) {
	    System.out.println("read(): receive correct");
	  }
          int k = (int) SEQInfo.indexOf(new Integer(exp_SEQ));
          while (k >=0) {
	    /*if (Debug) {
	      System.out.println("read(): retrieve packet[" + exp_SEQ + "] from buffer");
	    }*/
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
	    if (Debug) {
	      System.out.println("read(): Store packet[" + exp_SEQ + "] into buffer");
	    }
            SEQInfo.add(new Integer(packet.getSEQ()));
            DataList.add(packet);
          }
	  continue;
	}
      }
    }
  }
}
