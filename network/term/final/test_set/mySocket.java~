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
    private boolean Debug = false;
    private int hostPort = 2155;
    private int localPort = 2155;
    private String hostName = "localhost";
    private String localName = "localhost";
    private InetAddress hostAddr = null;
    private InetAddress localAddr = null;
    private DatagramSocket socket = null;

    ArrayList writeBuffer = new ArrayList(1000);
    ArrayList readBuffer = new ArrayList(1000);

    private volatile int MSS = 1500;
    private volatile int my_SEQ = 1000; /* Require some randomness here */
    private volatile int my_ACK = my_SEQ;
    private volatile int exp_SEQ = 0;
    private volatile int WINSZ = 10;
    private volatile int est_time = 1000;
    private volatile int deviation = 200;
    private volatile int timeout = 1200;
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
        Thread.sleep(20);
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

  class myTimer extends Thread {
    public myTimer(){}
    public void run() {
      long currentTime = System.currentTimeMillis();
      try {
        this.wait();
      } catch (Exception wait_error) {}
      currentTime = System.currentTimeMillis() - currentTime;
      synchronized (this) {
        est_time = (int) (0.875*est_time + 0.125*currentTime);
	deviation = (int) (0.875*deviation + 0.125*Math.abs(est_time-currentTime)); 
	timeout = est_time + 2*deviation;
      }
    }
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
      myTimer timer = new myTimer();
      
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
	timer.start();
	try {
	  socket.send(ackOnly);
	} catch (Exception send_error) {}
	while (packets[0].getACK() == exp_SEQ) {
	  try {
	    sleep(timeout);
	  } catch (Exception sleep_err) {}
	  try {
	    socket.send(ackOnly);
	  } catch (Exception send_error) {}
	}
	try {
	  notifyAll();
	} catch (Exception thread_error) {}
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
        if (packet.isSYN()) {
          continue;
        }

	if (packet.isFIN()) {
	  return 0;
	}
	
	synchronized (this) {
	  if (packet.getACK() > my_ACK) {
	    my_ACK = packet.getACK();
	  }
	}  

        if (packet.getSEQ() < exp_SEQ ) {
          continue;
        }
        if ( packet.getSEQ() == exp_SEQ ) {
	  synchronized (this) {
            exp_SEQ++;
	    readBuffer.add(packet);
	  }
          int k = (int) SEQInfo.indexOf(new Integer(exp_SEQ));
          while (k >=0) {
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
            SEQInfo.add(new Integer(packet.getSEQ()));
            DataList.add(packet);
          }
	  continue;
	}
      }
    }
  }
}
