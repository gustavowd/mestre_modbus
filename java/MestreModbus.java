/* MestreModbus.java */

public class MestreModbus {
    native void modbusOpen(); /* (1) */
    static public void main(String argv[]) {
        System.loadLibrary("mmodbus"); /* (2) */
        MestreModbus mestre = new MestreModbus();
        mestre.modbusOpen(); /* (3) */
    }
}

