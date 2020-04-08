/* MestreModbus.java */

public class MestreModbus {
    native int modbusOpen(String str); /* (1a) */
    native int modbusUpdateFiO2(short FiO2); /* (1b) */
    native int modbusUpdateInsp(short inst); /* (1c) */
    native int modbusUpdateBpm(short bpm); /* (1d) */
    native int modbusUpdateMaxPaw(short max_paw); /* (1e) */
    static public void main(String argv[]) {
            System.loadLibrary("mmodbus"); /* (2) */
            MestreModbus mestre = new MestreModbus();
            int status = mestre.modbusOpen("/dev/ttyACM0");
            if (status == 0){
                status = mestre.modbusUpdateFiO2((short) 25);
                status = mestre.modbusUpdateInsp((short) 15);
                status = mestre.modbusUpdateBpm((short) 20);
                status = mestre.modbusUpdateMaxPaw((short) 5);
            }
    }
}

