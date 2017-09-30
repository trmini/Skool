import java.lang.*;

/* Class that implement a myTCP packet */
class myTCP {

  int m_SEQ = 0;
  int m_ACK = 0;
  int d_LEN = 0;        /* 2 bytes                               */
  byte h_LEN = 8;

  byte m_FLG = 0; 
  byte SYN_MASK = 0x01; /*  m_FLG - Option Flag                  */
  byte FIN_MASK = 0x02; /*  [ ][ ][URG][ACK][PSH][RST][FIN][SYN] */
  byte RST_MASK = 0x04;
  byte PSH_MASK = 0x08;
  byte ACK_MASK = 0x10;
  byte URG_MASK = 0x20;

  protected byte[] m_DATA = null;

  public myTCP() {
  }

  public myTCP(byte[] buf) {
    m_SEQ = (((int) buf[0]) << 8) | (0x000000FF & buf[1]);
    m_ACK = (((int) buf[2]) << 8) | (0x000000FF & buf[3]);
    h_LEN = buf[4];
    m_FLG = buf[5];
    d_LEN = (((int) buf[6]) << 8) | (0x000000FF & buf[7]);
    if (d_LEN > 0) {
      m_DATA = new byte[d_LEN+1];
      System.arraycopy(buf, h_LEN, m_DATA, 0, d_LEN);
    }
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

  public boolean isACK() {
    if ( (m_FLG & ACK_MASK) != 0 ) return true;
    else return false;
  }

  public boolean isURG() {
    if ( (m_FLG & URG_MASK) != 0 ) return true;
    else return false;
  }

  public void setACK() {
    m_FLG = (byte) (m_FLG | ACK_MASK);
  }

  public void unsetACK() {
    m_FLG = (byte) (m_FLG & (~ACK_MASK));
  }

  public void setSYN() {
    m_FLG = (byte) (m_FLG | SYN_MASK);
  }

  public void unsetSYN() {
    m_FLG = (byte) (m_FLG & (~SYN_MASK));
  }

  public void setURG() {
    m_FLG = (byte) (m_FLG | URG_MASK);
  }

  public void unsetURG() {
    m_FLG = (byte) (m_FLG & (~URG_MASK));
  }


  public int getSEQ() {
    return m_SEQ;
  }

  public int getACK() {
    return m_ACK;
  }

  public void putSEQ(int sequenceNum) {
    m_SEQ = sequenceNum;
  }

  public void putACK(int ackNum) {
    m_ACK = ackNum;
  }

  public byte getHeaderLength() {
    return h_LEN;
  }

  public int getDataLength() {
    return d_LEN;
  }

  public int getLength() {
    return h_LEN + d_LEN;
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
    m_DATA = new byte[d_LEN];
    System.arraycopy(buf, 0, m_DATA, 0, d_LEN);
  }

  public byte[] makeACK(int ACKNum) {
    myTCP ACKPkt = new myTCP();
    ACKPkt.putSEQ(0);
    ACKPkt.putACK(ACKNum);
    ACKPkt.putData(null, 0);
    return ACKPkt.makePacket();
  }

  public byte[] makePacket() {
    byte[] buf = new byte[h_LEN + d_LEN];
    buf[0] = (byte) (m_SEQ >>> 8);
    buf[1] = (byte) (0x000000FF & m_SEQ);
    buf[2] = (byte) (m_ACK >>> 8);
    buf[3] = (byte) (0x000000FF & m_ACK);
    buf[4] = h_LEN;
    buf[5] = m_FLG;
    buf[6] = (byte) (d_LEN >>> 8);
    buf[7] = (byte) (0x000000FF & d_LEN);
    if (m_DATA != null) {
      System.arraycopy(m_DATA, 0, buf, h_LEN, d_LEN);
    }
    return buf;
  }

  public String toString() {
    byte[] buf = new byte[h_LEN + d_LEN];
    buf[0] = (byte) (m_SEQ >>> 8);
    buf[1] = (byte) (0x000000FF & m_SEQ);
    buf[2] = (byte) (m_ACK >>> 8);
    buf[3] = (byte) (0x000000FF & m_ACK);
    buf[4] = h_LEN;
    buf[5] = m_FLG;
    buf[6] = (byte) (d_LEN >>> 8);
    buf[7] = (byte) (0x000000FF & d_LEN);
    if (m_DATA != null) {
      System.arraycopy(m_DATA, 0, buf, h_LEN, d_LEN);
    }
    String newstring = new String();
    for (int i = 0; i < h_LEN + d_LEN; i ++) {
      newstring += "["+buf[i]+"] ";
    }
    return newstring;
  }
}
