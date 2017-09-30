import java.lang.*;


/* Class that implement a myTCP packet */
public class myTCP {
  protected int m_PacketSize = 512;
  protected byte[] m_data = null;
  protected int m_Seq = 0;
  protected int m_Ack = 0;
  protected int header_LEN = 16;
  protected int data_LEN = 0;
  protected boolean NACK = false;
  protected boolean DATA = false;
  protected byte NACK_MASK = 0x01;
  protected byte DATA_MASK = 0x02;

  protected byte[] srcAddress = null;
  protected byte[] dstAddress = null;

  public myTCP() {
  }
  
  public myTCP(byte[] buf) {
    srcAddress = new byte[]{buf[0], buf[1], buf[2], buf[3]};
    dstAddress = new byte[]{buf[4], buf[5], buf[6], buf[7]};
    m_Seq = ((0x0 | buf[8]) << 8) | buf[9];
    m_Ack = ((0x0 | buf[10]) << 8) | buf[11];
    if ( (buf[12] & NACK_MASK) == 0 ) {
      NACK = false;
    }
    else {
      NACK = true;
    }
    if ( (buf[12] & DATA_MASK) == 0 ) {
      DATA = false;
    }
    else {
      DATA = true;
    }
    header_LEN = 0x0 | buf[13];
    data_LEN =  ( (0x0 | buf[14] ) << 8) | buf[15];
    m_PacketSize = header_LEN + data_LEN;
    m_data = new byte[data_LEN];
    System.arraycopy(buf, header_LEN, m_data, 0, data_LEN);
  }
  
  public byte[] getSrcAddress() {
    return srcAddress;
  }

  public byte[] getDstAddress() {
    return dstAddress;
  }

  private void setSrcAddress(byte[] src) {
    if (srcAddress == null) {
      srcAddress = new byte[4];
    }
    System.arraycopy(src, 0, srcAddress, 0, 4);
  }

  private void setDstAddress(byte[] dst) {
    if (dstAddress == null) {
      dstAddress = new byte[4];
    }
    System.arraycopy(dst, 0, dstAddress, 0, 4);
  }
  
  public void swapAddress() {
    byte[] temp = (byte[]) srcAddress.clone();
    srcAddress = (byte[]) dstAddress.clone();
    dstAddress = (byte[]) temp.clone();
  }

  public boolean NACK() {
    return NACK;
  }

  public boolean DATA() {
    return DATA;
  }


  public void setNACK() {
    NACK = true;
  }

  public void unsetNACK() {
    NACK = false;
  }

  public void setDATA() {
    DATA = true;
  }

  public void unsetDATA() {
   DATA = false;
  }

  public int getSEQ() {
    return m_Seq;
  }

  public int getNACK() {
    return m_Ack;
  }

  public void setSEQ(int sequenceNum) {
    m_Seq = sequenceNum;
  }

  public void setNACK(int nackNum) {
    m_Ack = nackNum;
  }
  
  public int getHeaderLen() {
    return header_LEN;
  }

  public int getDataLen() {
    return data_LEN;
  }
    
  public void setHeaderLen(int Length) {
    header_LEN = Length ;
  }

  public void setDataLen(int Length) {
    data_LEN = Length;
  }

  public byte[] getData() {
    return m_data;
  }

  public void setData(byte[] buf, int Length) {
    data_LEN = Length;
    m_PacketSize = header_LEN + data_LEN;
    if (Length == 0) {
      m_data = null;
      data_LEN = 0;
      DATA = false;
      return;
    }
    if ( m_data == null ) {
      m_data = new byte[data_LEN];
    }
    System.arraycopy(buf, 0, m_data, 0, data_LEN);
    DATA = true;
  }
    
  public byte[] makeNACK(int nackNum) {
    myTCP nackPkt = new myTCP();
    nackPkt.setSrcAddress(dstAddress);
    nackPkt.setDstAddress(srcAddress);
    nackPkt.setSEQ(0);
    nackPkt.setNACK(nackNum);
    nackPkt.setHeaderLen(16);
    nackPkt.setDataLen(0);
    return nackPkt.makePacket();
  }

  public byte[] makePacket() {
    byte[] buf = new byte[header_LEN + data_LEN];
    System.arraycopy(srcAddress, 0, buf, 0, 4);
    System.arraycopy(dstAddress, 0, buf, 4, 4);
    buf[8] = (byte) (m_Seq >> 8);
    buf[9] = (byte) (0x0F & m_Seq);
    buf[10] = (byte) (m_Ack >> 8);
    buf[11] = (byte) (0x0F & m_Ack);
    buf[12] = 0;
    if (NACK == true) buf[12] = (byte) (buf[12] | NACK_MASK);
    if (DATA == true) buf[12] = (byte) (buf[12] | DATA_MASK);
    buf[13] = (byte) (0x0 | header_LEN);
    buf[14] = (byte) (data_LEN >> 8);
    buf[15] = (byte) (0x0F & data_LEN); 
    if (m_data != null) {
      System.arraycopy(m_data, 0, buf, header_LEN, data_LEN);
    }
    return buf;
  }
}
