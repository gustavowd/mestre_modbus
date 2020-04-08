/* MestreModbus.java */

public class MestreModbus {
    native void modbusOpen(); /* (1a) */
    native void modbusUpdateFiO2(short FiO2); /* (1b) */
    static public void main(String argv[]) {
        System.loadLibrary("mmodbus"); /* (2) */
        MestreModbus mestre = new MestreModbus();
        mestre.modbusOpen(); /* (3) */
        mestre.modbusUpdateFiO2((short) 25); /* (3) */
    }
}

