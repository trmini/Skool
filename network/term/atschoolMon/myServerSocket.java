import java.lang.*;
import java.net.*;
import mySocket;

public class myServerSocket {
  protected mySocket m_ServerSocket;
  protected int m_Port = 0;
  protected int m_bindAddr = null;

  public myServerSocket(int serverPort) {
    m_ServerSocket = new myServerSocket(serverPort);
    m_Port = serverPort;
  }

  public myServerSocket(int serverPort, InetAddress bindAddr) {
    m_ServerSocket = new myServerSocket(serverPort, bindAddr);
    m_Port = serverPort;
    m_bindAddr = bindAddr;
  }
  
  public mySocket accept() {
    mySocket connectionSocket = m_ServerSocket.accept();
    return connectionSocket;
  }

  public void close() {
    m_ServerSocket.close();
    m_Port = 0;
  }
  
  public int getLocalPort() {
    return m_Port;
  }

  public InetAddress getInetAddress() {
    return m_bindAddr;
  }
}
