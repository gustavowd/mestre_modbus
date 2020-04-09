

#include <serial.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sched.h>
#include <pthread.h>
#include <signal.h>


int serial_handle = 0;

#define FiO2_ADDR       40000
#define INSP_ADDR       40001
#define INSP_SF_ADDR    40002
#define BPM_ADDR        40003
#define PAW_ADDR        40004
#define MAX_PAW_ADDR    40005
#define MIN_PEPP_ADDR   40006
#define MAX_PEEP_ADDR   40007
#define ON_OFF_ADDR   	40008

#define CURRENT_PAW_ADDR	50000
#define ALARM_PAW_ADDR		50001
#define ALARM_PEPP_LOSS_ADDR   	50002
#define ALARM_PEEP_HIGH_ADDR   	50003
#define ALARM_PAIR_ADDR    	50004
#define ALARM_PO2_ADDR        	50005

static const uint8_t aucCRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40
};

static const uint8_t aucCRCLo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
    0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
    0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
    0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
    0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
    0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 
    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
    0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
    0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 
    0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
    0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
    0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
    0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
    0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
    0x41, 0x81, 0x80, 0x40
};

uint16_t usMBCRC16( uint8_t * pucFrame, uint16_t usLen )
{
    uint8_t           ucCRCHi = 0xFF;
    uint8_t           ucCRCLo = 0xFF;
    int             iIndex;

    while( usLen-- )
    {
        iIndex = ucCRCLo ^ *( pucFrame++ );
        ucCRCLo = ( uint8_t )( ucCRCHi ^ aucCRCHi[iIndex] );
        ucCRCHi = aucCRCLo[iIndex];
    }
    return ( uint16_t )( ucCRCHi << 8 | ucCRCLo );
}



//Função para ler os input registers
uint8_t read_input_reg(uint8_t endereco_slave, uint16_t init_address, uint16_t num_reg_read, uint16_t *regs){

	uint8_t funcao=4; //função leitura de múltiplos registradores
	uint8_t mensagem[8]; //vetor  mensagem
	uint16_t  tamanho_resposta= 5+(2*num_reg_read);
	//vetor resposta (c não suporta tamanho de vetor determinado por parâmetro)
	uint8_t resposta[148]; // Suporta a leitura de um modelo com até 70 registradores
	uint16_t crc_resp;//variavel auxiliar CRC parte alta
	uint16_t CRC;
	int i;
	int j;

	//monta vetor mensagem
	init_address -= 1;
	mensagem[0] = endereco_slave;
	mensagem[1] = funcao;
	mensagem[2] = (uint8_t)(init_address >> 8);
	mensagem[3] = (uint8_t)init_address;
	mensagem[4] = (uint8_t)(num_reg_read >> 8);
	mensagem[5] = (uint8_t)num_reg_read;

	CRC=usMBCRC16(mensagem,6);

	mensagem[6] = (uint8_t)CRC;
	mensagem[7] = (uint8_t)(CRC>>8);

	resposta[0]=0;

	// Transmite a pergunta
	(void)serialPutBuffer(serial_handle, (char *)mensagem, 8) ;

	/* Recebe a resposta */
	serialGetchar(serial_handle, (char *)&resposta[0]);	// Slave addr
	serialGetchar(serial_handle, (char *)&resposta[1]);	// Function code
	if (resposta[1] != funcao){
		return -1;
	}

	for(j=2;j<tamanho_resposta;j++){
        serialGetchar(serial_handle, (char *)&resposta[j]);	// Dados
	}

	// Teste de CRC
	CRC=usMBCRC16(resposta, (tamanho_resposta-2));
	crc_resp=(resposta[(tamanho_resposta-1)]<<8) | (resposta[(tamanho_resposta-2)]);

	if(CRC==crc_resp){
		for ( i = 0; i < ((tamanho_resposta - 5)>>1); i++){
			*regs++ =  (resposta[4+(i*2)] << 8) + resposta[3+(i*2)];
		}
		return 0;
	}else{
		return -1;
	}
}//fim da função de leitura dos input registers


//Função para ler os holding registers
uint8_t read_holding_reg(uint8_t endereco_slave, uint16_t init_address, uint16_t num_reg_read, uint16_t *regs){

	uint8_t funcao=3; //função leitura de múltiplos registradores
	uint8_t mensagem[8]; //vetor  mensagem
	uint16_t  tamanho_resposta= 5+(2*num_reg_read);
	//vetor resposta (c não suporta tamanho de vetor determinado por parâmetro)
	uint8_t resposta[148]; // Suporta a leitura de um modelo com até 70 registradores
	uint16_t crc_resp;//variavel auxiliar CRC parte alta
	uint16_t CRC;
	int i;
	int j;

	//monta vetor mensagem
	init_address -= 1;
	mensagem[0] = endereco_slave;
	mensagem[1] = funcao;
	mensagem[2] = (uint8_t)(init_address >> 8);
	mensagem[3] = (uint8_t)init_address;
	mensagem[4] = (uint8_t)(num_reg_read >> 8);
	mensagem[5] = (uint8_t)num_reg_read;

	CRC=usMBCRC16(mensagem,6);

	mensagem[6] = (uint8_t)CRC;
	mensagem[7] = (uint8_t)(CRC>>8);

	resposta[0]=0;

	// Transmite a pergunta
	(void)serialPutBuffer(serial_handle, (char *)mensagem, 8) ;

	/* Recebe a resposta */
	serialGetchar(serial_handle, (char *)&resposta[0]);	// Slave addr
	serialGetchar(serial_handle, (char *)&resposta[1]);	// Function code
	if (resposta[1] != funcao){
		return -1;
	}

	for(j=2;j<tamanho_resposta;j++){
        serialGetchar(serial_handle, (char *)&resposta[j]);	// Dados
	}

	// Teste de CRC
	CRC=usMBCRC16(resposta, (tamanho_resposta-2));
	crc_resp=(resposta[(tamanho_resposta-1)]<<8) | (resposta[(tamanho_resposta-2)]);

	if(CRC==crc_resp){
		for ( i = 0; i < ((tamanho_resposta - 5)>>1); i++){
			*regs++ =  (resposta[4+(i*2)] << 8) + resposta[3+(i*2)];
		}
		return 0;
	}else{
		return -1;
	}
}//fim da função de leitura do slave




//Função para escrever os holding registers
uint8_t write_holding_reg(uint8_t endereco_slave, uint16_t init_address, uint16_t num_reg_write, uint16_t *regs){

	uint8_t funcao=16; 		//função escrita de múltiplos registradores
	uint8_t mensagem[148]; 	//vetor mensagem, suporta escrita de até 69 registradores
	uint16_t  tamanho_pergunta = 7+(2*num_reg_write);

	// Tamanho permite a escrita de até 8 registradores
	uint8_t resposta[8];
	uint16_t crc_resp;//variavel auxiliar CRC parte alta
	uint16_t CRC;
	int i;
	int j;

	//monta vetor mensagem
        init_address -= 1;
	mensagem[0] = endereco_slave;
	mensagem[1] = funcao;
	mensagem[2] = (uint8_t)(init_address >> 8);
	mensagem[3] = (uint8_t)init_address;
	mensagem[4] = (uint8_t)(num_reg_write >> 8);
	mensagem[5] = (uint8_t)num_reg_write;
	mensagem[6] = (uint8_t)(num_reg_write * 2);

	for (i=0;i<num_reg_write;i++){
		mensagem[7+(i*2)] = (uint8_t)(regs[i] & 0xFF);
		mensagem[8+(i*2)] = (uint8_t)(regs[i] >> 8);
	}

	CRC=usMBCRC16(mensagem,tamanho_pergunta);

	mensagem[tamanho_pergunta] = (uint8_t)CRC;
	mensagem[tamanho_pergunta+1] = (uint8_t)(CRC>>8);

	/* Esvaziar buffer da serial (caso existe algum lixo) */
	resposta[0]=0;

	// Transmite a pergunta
	(void)serialPutBuffer(serial_handle, (char *)mensagem, tamanho_pergunta+2) ;

	/* Recebe a resposta */
	serialGetchar(serial_handle, (char *)&resposta[0]);	// Slave addr
	serialGetchar(serial_handle, (char *)&resposta[1]);	// Function code
	if (resposta[1] != funcao){
		return -1;
	}

	for(j=2;j<8;j++){
		serialGetchar(serial_handle, (char *)&resposta[j]);	// Dados
	}

	// Teste de CRC
	CRC=usMBCRC16(resposta, (6));
	crc_resp=(resposta[(7)]<<8) | (resposta[(6)]);

	if(CRC==crc_resp){
		return 0;
	}else{
		return -1;
	}
}//fim da função de escrita do slave



void signal_callback_handler(int signum)
{
	// Close serial port
	serialClose(serial_handle);
	
	printf("Caught signal %d\n\r",signum);

    // Terminate program
	exit(signum);
}


#define SWAP(x) ((((x) & 0x00ffUL) << 8) | (((x) & 0xff00UL) >> 8))

uint8_t escreve_FiO2(uint16_t FiO2){
    // Escreve novo valor para o respirador    
    printf("Atualizando valor de FiO2 para %u\n\r", (unsigned int)SWAP(FiO2));
    write_holding_reg(10, FiO2_ADDR, 1, &FiO2);

    // Confere se o valor foi atualizado
    uint16_t updated_FiO2;
    read_holding_reg(10, FiO2_ADDR, 1, &updated_FiO2);

    if (FiO2 == updated_FiO2){
        printf("Confirmada atualização de FiO2 para %u\n\r", (unsigned int)SWAP(updated_FiO2));
        return 0;
    }else{
        printf("Erro na atualização de FiO2 para %u\n\r", (unsigned int)SWAP(updated_FiO2));
        return -1;
    }
}


uint8_t escreve_insp(uint16_t insp){
    // Escreve novo valor para o respirador    
    printf("Atualizando valor de insp para %u\n\r", (unsigned int)SWAP(insp));
    write_holding_reg(10, INSP_ADDR, 1, &insp);
    fflush(stdout);

    // Confere se o valor foi atualizado
    uint16_t updated_insp;
    read_holding_reg(10, INSP_ADDR, 1, &updated_insp);
    fflush(stdout);

    if (insp == updated_insp){
        printf("Confirmada atualização de bpm para %u\n\r", (unsigned int)SWAP(updated_insp));
        return 0;
    }else{
        printf("Erro na atualização de bpm para %u\n\r", (unsigned int)SWAP(updated_insp));
        return -1;
    }
}


uint8_t escreve_bpm(uint16_t bpm){
    // Escreve novo valor para o respirador  
    printf("Atualizando valor de bpm para %u\n\r", (unsigned int)SWAP(bpm));  
    write_holding_reg(10, BPM_ADDR, 1, &bpm);

    // Confere se o valor foi atualizado
    uint16_t updated_bpm;
    read_holding_reg(10, BPM_ADDR, 1, &updated_bpm);

    if (bpm == updated_bpm){
        printf("Confirmada atualização de bpm para %u\n\r", (unsigned int)SWAP(updated_bpm));
        return 0;
    }else{
        printf("Erro na atualização de bpm para %u\n\r", (unsigned int)SWAP(updated_bpm));
        return -1;
    }
}


uint8_t escreve_max_paw(uint16_t max_paw){
    // Escreve novo valor para o respirador
    printf("Atualizando valor de max_paw para %u\n\r", (unsigned int)SWAP(max_paw));    
    write_holding_reg(10, MAX_PAW_ADDR, 1, &max_paw);

    // Confere se o valor foi atualizado
    uint16_t updated_max_paw;
    read_holding_reg(10, MAX_PAW_ADDR, 1, &updated_max_paw);

    if (max_paw == updated_max_paw){
        printf("Confirmada atualização de max_paw para %u\n\r", (unsigned int)SWAP(updated_max_paw));
        return 0;
    }else{
        printf("Erro na autalização de max_paw para %u\n\r", (unsigned int)SWAP(updated_max_paw));
        return -1;
    }
}


uint8_t escreve_on_off(uint16_t on_off){
    // Escreve novo valor para o respirador  
    printf("Atualizando valor de on_off para %u\n\r", (unsigned int)SWAP(on_off));  
    write_holding_reg(10, ON_OFF_ADDR, 1, &on_off);

    // Confere se o valor foi atualizado
    uint16_t updated_on_off;
    read_holding_reg(10, ON_OFF_ADDR, 1, &updated_on_off);

    if (on_off == updated_on_off){
        printf("Confirmada atualização de bpm para %u\n\r", (unsigned int)SWAP(updated_on_off));
        return 0;
    }else{
        printf("Erro na atualização de bpm para %u\n\r", (unsigned int)SWAP(updated_on_off));
        return -1;
    }
}

void le_paw_alarms(void){
    uint16_t read_data[6];
    read_input_reg(10, CURRENT_PAW_ADDR, 6, read_data);

    printf("Paw Atual em cm H2O %u\n\r", (unsigned int)SWAP(read_data[0]));
    printf("Alarmes %u, %u, %u, %u, %u\n\r", (unsigned int)SWAP(read_data[1]), (unsigned int)SWAP(read_data[2]), (unsigned int)SWAP(read_data[3]), (unsigned int)SWAP(read_data[4]), (unsigned int)SWAP(read_data[5]));
}

/*
 *********************************************************************************
 * main:
 *	Let the fun begin
 *********************************************************************************
 */
int main (int argc, char** argv)
{
  signal(SIGINT, signal_callback_handler);
  
  printf("\n\rTentando abrir a porta serial %s\n\r", argv[1]);
  serial_handle = serialOpen (argv[1], 115200);
  if (serial_handle < 0)
  {
	    printf("Não deu certo. Tentando abrir a porta serial /dev/tnt1\n\r");
	    serial_handle = serialOpen ("/dev/tnt1", 115200);
	    if (serial_handle < 0)
	    {
			printf("Não foi possível abrir a porta serial /dev/tnt1!\n\r");
			return -1;
	    }else{
		printf("Porta serial /dev/tnt1 aberta com sucesso!\n\r\n\r");
	    }
  }else{
	printf("Porta serial %s aberta com sucesso!\n\r\n\r", argv[1]);  
  }

  //uint16_t FiO2 = 2;
  //uint16_t max_paw = 5;

  uint16_t insp = 20;
  uint16_t bpm = 15;

  int i = 0;
  
  while(1)
  {
    printf("\n\r");
    for (i=0; i<10; i++){
	le_paw_alarms();
    	sleep(1);
    }
    printf("\n\r");

    printf("Liga respirador!\n\r"); 
    escreve_on_off(SWAP(1));
    printf("\n\r");	

    insp = 30;
    bpm = 10;
    escreve_insp(SWAP(insp));
    escreve_bpm(SWAP(bpm));

    printf("\n\r");
    for (i=0; i<18; i++){
	le_paw_alarms();
    	sleep(1);
    }
    printf("\n\r");

    printf("\n\r");

    insp = 5;
    bpm = 30;
    escreve_insp(SWAP(insp));
    escreve_bpm(SWAP(bpm));

    printf("\n\r");
    for (i=0; i<18; i++){
	le_paw_alarms();
    	sleep(1);
    }
    printf("\n\r");

    printf("\n\r");

    insp = 15;
    bpm = 20;
    escreve_insp(SWAP(insp));
    escreve_bpm(SWAP(bpm));

    printf("\n\r");
    for (i=0; i<12; i++){
	le_paw_alarms();
    	sleep(1);
    }
    printf("\n\r");

    printf("\n\r");

    printf("Desliga respirador!\n\r");
    escreve_on_off(SWAP(0));	
    printf("\n\r");
#if 0      
    escreve_FiO2(SWAP(FiO2));
    FiO2++;
    sleep(1);
    escreve_max_paw(SWAP(max_paw));
    max_paw++;
    sleep(1);
#endif
  }

  return 0 ;
}
