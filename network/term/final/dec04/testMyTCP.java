import java.lang.*;
import java.*;
import myTCP;

class testMyTCP {
  public static void main(String[] args) throws Exception {

    myTCP testPacket = new myTCP();
	byte[] buf = new byte[]{1, 2, 3, 4, 5};
	testPacket.putSEQ(1979);
	testPacket.putACK(10000);
	//testPacket.putData(buf, 5);
	testPacket.setACK();
	if (testPacket.isACK()) {
	  System.out.println("This is an ACK packet: \n\t" + testPacket);
	  testPacket.unsetACK();
	}
	if (!testPacket.isACK()) {
	  testPacket.setSYN();
	  if (testPacket.isSYN()) {
	    System.out.println("Packet change from ACK to SYN: \n\t" + testPacket);
	  }
	}
	if (testPacket.isSYN()) {
	  myTCP otherPacket = new myTCP();
	  otherPacket = new myTCP(testPacket.makePacket());
	  System.out.println("A copy of the above: \n\t" + otherPacket);
	}
  }
}
