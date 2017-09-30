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

    public void receive(byte[] buf) {
      int indexPkt = 0;
      int expSEQ = o_Seq;
      int offset = expSEQ;
      byte[] buffer = new byte[m_PacketSize];
      DatagramPacket udpPkt = new DatagramPacket(buffer, m_PacketSize);
      nakThread NAKtimer = new nakThread();
      NAKtimer.start();
      while (true) {
      	m_DatagramSocket.receive(udpPkt);
	myTCP rcvPkt = new myTCP(udpPkt.getData());
        if ( expSeq == rcvPkt.getSEQ()) {
	  NAKtimer.pauseReq();
	  myTCP nakPkt = new myTCP();
	  System.arraycopy(rcvPkt.getData(), 0, buf, index, rcvPkt.getDataLen());
	  index += rcvPkt.getDataLen();
	  expSEQ++;
	  NAKtimer.putNAK(nakPkt.makeNAK(expSEQ), nakPkt.getLength(), o_DatagramSocket);
	  NAKtimer.setTimer(m_Timer);
	  NAKtimer.interrupt();
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
      return buf;
    }

  }

public class nakThread extends Thread (
  private volatile DatagramPacket packet;
  private volatile DatagramSocket socket;
  private volatile int desiredTime = 0;
  private volatile boolean done = false;
  private volatile boolean suspended = true;
  private volatile boolean lock;

  public void run() {
    while (!done) {
      try {
        desiredTime = 0;
        Thread.sleep(100000);
      } catch (InteruptedException x) {
        if (desiredTime >= 0) {
	  try {
	    Thread.sleep(desiredTime)
             socket.send(packet);
          } catch (InteruptedException x) {}
	}
	continue;
      }
    }
    return;
  }
  
  public void putNAK (byte[] NAK_pkt, int NAK_len, DatagramSocket host) {
    socket = new DatagramSocket(host);
    packet = new DatagramPacket(NAK_pkt, NAK_len, socket.getInetAddress(), socket.getPort());
  }

  public void setTimer(int desiredTime) {
    while (lock);
    timer = desiredTime;
  }
  
  public void cancel() {
    done = true;
  }

  public void pauseReq() {
    suspended = true;
  }
  
  public void resumeReq() {
    suspended = false;
  }
  
}
