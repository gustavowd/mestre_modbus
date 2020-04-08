# mestre_modbus

No arquivo Makefile, substituir /usr/lib/jvm/java-8-openjdk-amd64/ pela caminho do java da sua máquina

compilar java:
javac MestreModbus.java -h .

Executar java:
java -cp . -Djava.library.path=. MestreModbus
