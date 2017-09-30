import java.lang.*;
import java.net.*;

/**
*   Computer Network - CS4873
*   Term Project - Fall 2001
*   Implementation of reliable network transport scheme based on UDP
*   Dien Trang Luu
**/

public class mySocket {

    private int m_PortNum = 0;
    private int o_PortNum = 0;
    private InetAddress m_InetAddress = null;
    private InetAddress o_InetAddress = null;
    private int m_Seq = 0;
    private int offsetSeq = 0;
    private Random random = new Random();
    private int RANDOM_RANGE = 1000;

    private int m_PacketSize = 512; /* Size for each packet, default 512 bytes */
    private int m_BuffWindow = 10;


    private DatagramSocket m_DatagramSocket = null;
    private DatagramPacket[] m_SendPackets = null;
    private DatagramPacket[] m_ReceivePackets = null;

    /**
    *  Constructor Implementation
    **/
    public mySocket() {
      offsetSeq = random.nextInt(RANDOM_RANGE);
      try {
      	m_InetAddress = InetAddress.getLocalAddress();
        m_DatagramSocket = new DatagramSocket();
      	m_PortNum = m_DatagramSocket.getLocalPort();

      }
      catch {
      }
    }

    public mySocket(int portNum) {
      offsetSeq = random.nextInt(RANDOM_RANGE);
      m_DatagramSocket = new DatagramSocket(portNum);
      m_PortNum = portNum;
      m_InetAddress = m_DatagramSocket.getLocalAddress();

    }

    public mySocket(int portNum,  InetAddress hostAddress) {
      offsetSeq = random.nextInt(RANDOM_RANGE);
      m_DatagramSocket = new DatagramSocket(portNum, hostAddress);
      m_PortNum = portNum;
      m_InetAddress = hostAddress;
    }

    /**
    *  Methods Implementation
    **/
    public void close() {
      m_PortNum = 0;
      m_InetAddress = null;
      o_PortNum = 0;
      o_InetAddress = null;
      m_DatagramSocket.close();
    }

    public InetAddress getInetAddress() {
      return o_InetAddress;
    }

    public InetAddress getLocalAddress() {
      return m_InetAddress;
    }

    public int getPort() {
      return o_PortNum;
    }

    public int getLocalPort() {
      return m_PortNum;
    }

    public int setPort(int Port) {
      o_PortNum = Port;
      return o_PortNum;
    }

    public int setLocalPort(int Port) {
      m_PortNum = Port;
      return m_PortNum;
    }


    public mySocket connect(String hostAddress, int port) {
      mySocket connectSocket;
      return connectSocket;
    }

    public mySocket accept() {
      byte buf[m_PacketSize];
      DatagramPacket packet = new DatagramPacket(buf, m_PacketSize);
      mySocket acceptSocket;
      m_DatagramSocket.receive(packet);
      acceptSocket.setPort(packet.getPort());
      acceptSocket.setInetAddress(packet.getAddress());
      int nack_seq = (int) buf[0] + 1;
      buf[0] = nack_seq;
      packet = new DatagramPacket(buf, m_PacketSize, acceptSocket.getInetAddress(),
      		acceptSocket.getPort());
      m_DatagramSocket.send(packet);
      /* timeoout(); */
      m_DatagramSocket.receive(packet);

      /* verification */
      return acceptSocket;
    }

    public boolean receive(byte[] buf) {
      byte [] actualData = new byte[1000];
      int index = 0;
      int timeOut = TIMEOUT;
      int expSeq = o_Seq;
      int offset = expSeq;
      byte[] buffer = new byte[m_PacketSize];
      DatagramPacket Pkt = new DatagramPacket(buffer, m_PacketSize);
      byte[] dataBuf = new byte[m_PacketSize];
      while (true) {
      	m_DatagramSocket.receive(Pkt);
        dataBuf = (byte[]) Pkt.getData().clone();
	myTCP rcvPkt = new myTCP(dataBuf);
        if ( expSeq == rcvPkt.getSeq()) {
	  System.arraycopy(rcvPkt.getData(), 0, actualData, index, rcvPkt.getDataLen());
	  index += recvPkt.getDataLen();
	  expSeq++;
	  scheduletimeout;
        }
        else {
          if (expSeq < receivePkt.getSeq()) {
	    rcvPkt.makeNACK(expSeq);

          }
          else {
            continue;
          }
        }



      }
    }

    private boolean send(byte [] buf) {
      int window_sz = 10;
      int window_pkt = window_sz;
      int index = 0;
      int max_index = buf.length()/myPacketSize;
      for (index = 0; index < max_index; index++) {
      /* Attention: Construction of packets without header, sequence num needed, acked needed */
        myDatagramPackets[index] = new DatagramPacket(buf[index*myPacketSize], myPacketSize, otherInetAddress, otherPortNum);
      }
      if ((index-1)*myPacketSize < buf.length()) {
      /* Attention: Construction of the last packet */
        myDatagramPackets[index] = new DatagramPacket(buf[index*myPacketSize], (buf.length() - (index*myPacketSize)), otherInetAddress, otherPortNum);
      }
      index = 0;
      while (index <= max_index) {
        window_pkt = index + window_sz;
        while (index < window_pkt) {
          myDatagramSocket.send(myDatagramPackets[index]);
	  index++;
        }
	/*Waiting for NACK */
        byte temp[myPacketSize];
        DatagramPacket nackPacket = new DatagramPacket(temp, temp.length());
	/* Install NACK timer here */
        Thread ackTimer;
	
        if ((int) temp[0] <= window_pkt) {
	  index = (int) temp[0];
	  window_pkt = index + window_sz; /* Slide sending window */
	  continue;
	}
	else {
	  break;
	}
      }
    }
  }
