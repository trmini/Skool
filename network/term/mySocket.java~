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
    private int hostPort = 2155;
    private int localPort = 2155;
    private String hostName = "localhost";
    private String localName = "localhost";
    private InetAddress hostAddr = null;
    private InetAddress localAddr = null;
    private DatagramSocket socket = null;
    private DatagramPacket tmpPacket;

    private volatile int MSS = 512;
    private volatile int my_SEQ = 1000; /* Require some randomness here */
    private volatile int exp_SEQ = 0;
    private volatile int WINSZ = 10;
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

    public void listen(int portNum) {
      setLocalPort(portNum);
      try {
        socket = new DatagramSocket(portNum);
      } catch (SocketException bind_error) {
        System.out.println("Cannot listen on port " + portNum + ": " + bind_error);
	return;
      }
      localAddr = socket.getLocalAddress();
      my_SEQ = 1000; /* can be a random number */
      try {
        socket.setSoTimeout(0);
      } catch (Exception x) {}
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
	  tmpPacket = grantedPacket;
	  /* the three-way handshake preserved */
	  break;
        }
        else {
	  /* Packet was not SYN */
	  continue;
	}
      }
    }


    public void connect(String nameString, int portNum) {
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
	  exp_SEQ = granted.getSEQ() + 1;
	  break;
	}
	continue;
      }
    }

    public int write(byte[] data, int length) {
    /* Asumption: already knew host InetAddress, Port, and more ... */
      int index = 0;
      int max_index = length / MSS;
      int curr_SEQ = my_SEQ;
      DatagramPacket[] packets = new DatagramPacket[max_index];
      byte[] PacketBuffer = new byte[MSS];
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
      System.arraycopy(data, index*MSS, PacketBuffer, 0, (length - index*MSS));
      packet.putSEQ(curr_SEQ++);
      packet.putACK(exp_SEQ);
      packet.putData(PacketBuffer, (length - index*MSS));
      packets[index] = new DatagramPacket(packet.makePacket(),
			packet.getLength(), hostAddr, hostPort);
			
      /** Done packets[] construction
      *   Ready to perform reliable data_send()
      **/
      
      int win_sz = WINSZ;
      int offset = 0;
      int est_RTT = 2000;
      int deviation = 100;
      int timeout = est_RTT + 4*deviation; 
      DatagramPacket rcvPacket = new DatagramPacket(PacketBuffer, MSS);
      while (true) {
        if (offset > max_index) {
	  return 1;
	}
        for (int i = 0; i < win_sz; i++) {
	  if ( (i + offset) > max_index) break;
          try {
	    socket.send(packets[i + offset]);
	  } catch (Exception x) {
	    System.out.println("Connection lost!");
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
	    sample_RTT = System.currentTimeMillis() - sample_RTT;
	    myTCP ack = new myTCP(rcvPacket.getData());
	    if (ack.isACK()) {
	      if (ack.getACK() > my_SEQ) {
	        deviation = (int) (0.875*deviation + 0.125*Math.abs(est_RTT - sample_RTT));
	        est_RTT = (int) (0.875*est_RTT + 0.125*sample_RTT);
		timeout = est_RTT + 4*deviation;
	        offset = offset + (ack.getACK() - my_SEQ);
	        my_SEQ = ack.getACK();
		break;
	      }
	      else {
	        break;
	      }
	    }
	    else continue;
	  } catch (Exception time_exp) {
	    /* Timeout */
	    timeout = 1 + timeout/2;
	    break;
	  }
	}
      }
    }

    public int read(byte[] buf, int length) {
      int timeout = 2000;
      int byteread = 0;
      ArrayList DataList = new ArrayList(15);
      ArrayList SEQInfo = new ArrayList(15);
      myTCP packet;
      byte[] PacketBuffer = new byte[2*MSS];
      DatagramPacket rcvPacket = new DatagramPacket(PacketBuffer, 2*MSS);
      DatagramPacket ackPacket = tmpPacket;
      try {
        socket.setSoTimeout(timeout);
      } catch (Exception settime_error) {}
      while (byteread < length) {
	try {
	  socket.receive(rcvPacket);
	} catch (Exception time_exp) {
          try {
	    socket.send(ackPacket);
	  } catch (Exception x) {}
	  continue;
	}
	packet = new myTCP(rcvPacket.getData());
	if (byteread == 0 && packet.isSYN()) {
	  try {
	    socket.send(tmpPacket);
	  } catch (Exception send_error) {
	    System.out.println("Connecton lost!!");
	    return -1;
	  }
	  continue;
	}

	if (packet.getSEQ() < exp_SEQ ) {
	  timeout = 1 + timeout/2;
          try {
            socket.setSoTimeout(timeout);
          } catch (Exception settime_error) {}
	  continue;
	}
	  
	if ( packet.getSEQ() == exp_SEQ ) {
	  exp_SEQ++;
	  System.arraycopy(packet.getData(), 0, buf, byteread, packet.getDataLength());
	  byteread += packet.getDataLength();
	  int k = SEQInfo.indexOf(exp_SEQ);
	  while (k >=0) {
	    SEQInfo.remove(k);
	    packet = DataList.remove(k);
	    System.arraycopy(packet.getData(), 0, buf, byteread, packet.getDataLength());
	    byteread += packet.getDataLength();
	    exp_SEQ++;
	    k = SEQInfo.indexOf(exp_SEQ);
	  }
	  myTCP ack = new myTCP();
	  ack.putSEQ(my_SEQ);
	  ack.putACK(exp_SEQ);
	  ack.setACK();
          ackPacket = new DatagramPacket(ack.makePacket(), ack.getLength(), hostAddr,
	  				hostPort);
	  /* Calculate timeout */
	  continue;
        }
	 
	if (packet.getSEQ() > exp_SEQ ) {
	  /* Store packet into a buffer to be retrieved later */
	  if (SEQInfo.indexOf(packet.getSEQ()) >= 0) {
	    SEQInfo.add(packet.getSEQ());
	    DataList.add(packet);
	  }
	  continue;
	}
	break;
      }
      return length;
    }
}
