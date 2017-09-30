import java.lang.*;

/* Class that implement a myTCP packet */
public class myTCP {

  protected int m_SEQ = 0;
  protected int m_NAK = 0;
  protected int d_LEN = 0;
  protected byte h_LEN = 0;
  protected byte m_FLG = 0;
  protected byte SYN_MASK = 0x01;
  protected byte FIN_MASK = 0x02;
  protected byte RST_MASK = 0x04;
  protected byte PSH_MASK = 0x08;
  protected byte NAK_MASK = 0x10;
  protected byte URG_MASK = 0x20;
  protected byte[] m_DATA = null;

  public myTCP() {
  }
  
  public myTCP(byte[] buf) {
    m_SEQ = ((0x0 | buf[0]) << 8) | buf[1];
    m_NAK = ((0x0 | buf[2]) << 8) | buf[3];
    d_LEN =  ( (0x0 | buf[6] ) << 8) | buf[7];
    h_LEN = buf[4];
    m_FLG = buf[5];
    m_DATA = new byte[d_LEN];
    System.arraycopy(buf, h_LEN, m_DATA, 0, d_LEN);
  }
  
  public boolean isSYN() {
    if ( (m_FLG & SYN_MASK) != 0) return true;
    else return false;
  }
  
  public boolean isFIN() {
    if ( (m_FLG & FIN_MASK) != 0) return true;
    else return false;
  }
  
  public boolean isRST() {
    if ( (m_FLG & RST_MASK) != 0 ) return true;
    else return false;
  }
  
  public boolean isPSH() {
    if ( (m_FLG & PSH_MASK) != 0 ) return true;
    else return false;
  }
  
  public boolean isNAK() {
    if ( (m_FLG & NAK_MASK) != 0 ) return true;
    else return false;
  }
  
  public boolean isURG() {
    if ( (m_FLG & URG_MASK) != 0 ) return true;
    else return false;
  }

  public void setNAK() {
    m_FLG = (byte) (m_FLG | NAK_MASK);
  }

  public void unsetNAK() {
    m_FLG = (byte) (m_FLG & (~NAK_MASK));
  }

  public int getSEQ() {
    return m_SEQ;
  }

  public int getNAK() {
    return m_NAK;
  }

  public void putSEQ(int sequenceNum) {
    m_SEQ = sequenceNum;
  }

  public void putNAK(int nackNum) {
    m_NAK = nackNum;
  }
  
  public byte getHeaderLength() {
    return h_LEN;
  }

  public int getDataLength() {
    return d_LEN;
  }
    
  public void setHeader(byte Length) {
    h_LEN = Length ;
  }

  public void setData(int Length) {
    d_LEN = Length;
  }

  public byte[] getData() {
    return m_DATA;
  }

  public void putData(byte[] buf, int Length) {
    d_LEN = Length;
    if (Length == 0) {
      m_DATA = null;
      return;
    }
    if ( m_DATA == null ) {
      m_DATA = new byte[d_LEN];
    }
    System.arraycopy(buf, 0, m_DATA, 0, d_LEN);
  }
    
  public byte[] makeNAK(int nakNum) {
    myTCP nakPkt = new myTCP();
    nakPkt.putSEQ(0);
    nakPkt.putNAK(nakNum);
    nakPkt.putData(null, 0);
    return nakPkt.makePacket();
  }

  public byte[] makePacket() {
    byte[] buf = new byte[h_LEN + d_LEN];
    buf[0] = (byte) (m_SEQ >> 8);
    buf[1] = (byte) (0x0F & m_SEQ);
    buf[2] = (byte) (m_NAK >> 8);
    buf[3] = (byte) (0x0F & m_NAK);
    buf[4] = h_LEN;
    buf[5] = m_FLG;
    buf[6] = (byte) (d_LEN >> 8);
    buf[7] = (byte) (0x0F & d_LEN); 
    if (m_DATA != null) {
      System.arraycopy(m_DATA, 0, buf, h_LEN, d_LEN);
    }
    return buf;
  }
}
