
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
//#include <getopt.h>
#include <time.h>

//#include "inc/deviceMap.h"
#include "inc/pthreadClass.h"
#include "inc/linux_Queue.h"   
#include "inc/linux_spi.h"
#include "inc/FastCRC32.h"


#define TEST_MAJOR_VERSION  (1)
#define TEST_MINOR_VERSION  (0)
#define TEST_PATCH_VERSION  (0)
// #define SPI_RAW_RX_TX_BUFFER_SIZE      2060U    // outer frame Rx
#define MAX_NUM_OF_INNER_PACKETS_SPI_Rx         77U      // INNER frame Rx
#define TOTAL_NUMBER_OF_SPI_DEVICES    (1U)
#define PSU_COMMAND 0xA0U
#define PC_POWER_DOWN_EXECUTE 0x88U
//! Number of Bytes to the beginning of the CRC block, in the raw packet
#define SPI_RAW_CRC_POSITION 2034U
#define STACKSIZE					(0x00010000)
#define SPI_RAW_RX_TX_BUFFER_SIZE       2060U
#define SLE_THREAD_DATA_SIZE            100U
#define SLE_THREAD_NUMBER               3
#define SLE_TOTAL_THREAD_DATA_NUMBER    ((SLE_THREAD_NUMBER) * (SLE_THREAD_DATA_SIZE))


void GenerateCRCandAddToOutboundPacket(CommandAndResponse::SPI_A_Outer_struct_Ascii_type*  g_SPI_UI_To_MMS_Packet);

enum { SLE_TOTAL_THREAD_DATA_NUMBER__ = SLE_TOTAL_THREAD_DATA_NUMBER };
enum { SLE_THREAD_DATA_SIZE__ = SLE_THREAD_DATA_SIZE };


unsigned char Tx_buff[SPI_RAW_RX_TX_BUFFER_SIZE];
typedef union SPI_Rx_OuterFrame {
    unsigned char outerRx_buff[SPI_RAW_RX_TX_BUFFER_SIZE]; 
    CommandAndResponse::SPI_A_Outer_struct_Ascii_type spi_Rx_outerFrame;
} SPI_Rx_outerFrame_t;
static SPI_Rx_outerFrame_t g_SPI_Rx_outerFrame;

typedef union SPI_Rx_InnerFrame {
    unsigned char InnerRx_buff[MAX_NUM_OF_INNER_PACKETS_SPI_Rx]; 
    CommandAndResponse::Raw_UI_To_MMS_Data_struct_type       spi_Rx_InnerFrame;  // inner frame
} SPI_Rx_InnerFrame_t;
static SPI_Rx_InnerFrame_t g_SPI_Rx_InnerFrame;


// extern C_DeviceMap_SLE          sle7k_dev_map;  // No extern allowed. Search for without extern 
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; /* locking mechanism for Sle7k_dev, Device_Map */

	
	//Define the raw SPI buffer for incoming traffic. This has a margin to allow for slippage from the MMS.
	static CommandAndResponse::BYTE									g_SPI_MMS_To_UI_Packet_Byte_Array[SPI_RAW_RX_TX_BUFFER_SIZE]; // 12 byte margin  
	 CommandAndResponse::SPI_A_Outer_struct_Ascii_type		g_SPI_UI_To_MMS_Packet; //The outgoing Command UI->MMS TX packet.
	static CommandAndResponse::SPI_A_Outer_struct_Ascii_type		g_SPI_UI_To_MMS_RetryCommandPacket; //The outgoing Retry UI->MMS TX packet.
	static CommandAndResponse::SPI_A_Outer_struct_Ascii_type *  	g_SPI_MMS_To_UI_Packet; //The incoming MMS->UI SPI packet.  Outer frame
    
    CommandAndResponse::CFastCRC32 m_CFastCRC; //!< The fast CRC object
	
    //CommandAndResponse::Raw_UI_To_MMS_Data_struct_type m_atCANBinDataFrameBufferUI[MAX_NUM_OF_CAN_PACKETS]; //!< The CAN Packet buffer array 
	CommandAndResponse::Raw_UI_To_MMS_Data_struct_type g_spiTx_innerFrame[MAX_NUM_OF_CAN_PACKETS]; // inner Tx spi !< The CAN Packet buffer array 

	static uint16_t g_add_cmd_ptr = NULL;
    enum { BEGIN_TX = 0 };
   //const Raw_UI_To_MMS_Data_struct_type* paCANBinMessage; //The actual CAN Binary message buffer array,
		// to be encoded to an SPI frame. Refers t the fact that it is binary, as oppossed to ascii encoded HEX.
        CommandAndResponse::BYTE g_NumOf_Tx_cmd = BEGIN_TX; //The number of entrys within the SPI frame that is populated with CAN messages.
    
    

void spi_Rx(C_Linux_SPI *spi_dev)   
{        
    int ret = 0;
    ret = spi_dev->read(&g_SPI_Rx_outerFrame.outerRx_buff, sizeof(g_SPI_Rx_outerFrame.outerRx_buff));
    //printf("\t sle read ret == %d;  g_SPI_Rx_outerFrame.outerRx_buff[] == %s\n", ret, g_SPI_Rx_outerFrame.outerRx_buff);
}


void spi_Tx(C_Linux_SPI *spi_dev)   
{        
    int ret = 0;
    ret = spi_dev->write(&g_SPI_UI_To_MMS_Packet, sizeof(Tx_buff));
    // printf("\t sle send %s;   ret == %d\n", Tx_buff, ret);
}

void SPI_run_Rx_Tx(C_Linux_SPI *spi_dev)     /* both read & write */
{
    enum { SPI_ENABLE = 0, SPI_DISABLE = 1 };
    struct spi_ioc_transfer spi_config_msg[TOTAL_NUMBER_OF_SPI_DEVICES];
    int ret, i;

    spi_config_msg[0].tx_buf        = (uintptr_t)Tx_buff;
    spi_config_msg[0].rx_buf        = (uintptr_t)g_SPI_Rx_outerFrame.outerRx_buff;
    spi_config_msg[0].len           = SPI_RAW_RX_TX_BUFFER_SIZE;
    spi_config_msg[0].cs_change     = SPI_ENABLE;
    spi_config_msg[0].delay_usecs   = 100;

/*
    spi_config_msg[1].bits_per_word = 8;
    spi_config_msg[1].rx_buf        = (uintptr_t)buf;
    spi_config_msg[1].tx_buf        = (uintptr_t)NULL;
    spi_config_msg[1].len           = 1056;
    spi_config_msg[1].cs_change     = 0;
*/


    ret = spi_dev->send_tr(spi_config_msg, TOTAL_NUMBER_OF_SPI_DEVICES);


    printf("send_tr ret == %d\n", ret);


    for(uint16_t i = 0; i < SPI_RAW_RX_TX_BUFFER_SIZE; i++)     {
        printf("buf[%d] == %d\n", i, g_SPI_Rx_outerFrame.outerRx_buff[i]);   }
 
 
}
  


void init_spi_dev(C_Linux_SPI *spi_dev)
{


    if( spi_dev->dev_open("/dev/spidev0.2") != 0 )
    {
        printf("Error: dev_open %s\n", spi_dev->strerror(spi_dev->get_errno()));
        exit(-1);
    }


    if( spi_dev->set_mode(SPI_MODE_1) != 0 )
    {
        printf("Error: %s\n", spi_dev->strerror(spi_dev->get_errno()));
        exit(-1);
    }


    if( spi_dev->set_bits_per_word(8) != 0 )
    {
        printf("Error: %s\n", spi_dev->strerror(spi_dev->get_errno()));
        exit(-1);
    }


    if( spi_dev->set_max_speed_hz(4000000) != 0 )
    {
        printf("Error: %s\n", spi_dev->strerror(spi_dev->get_errno()));
        exit(-1);
    }


}

	/*!********************************************************************
	* \brief Prepares a CAN packet containing the shut down request to PSU
	***********************************************************************/
	//void CPowerDownPanel::RequestPSUToShutDown() const
    void RequestPSUToShutDown(void)
	{				
		g_spiTx_innerFrame[g_add_cmd_ptr].Dest_Subsystem_ID_u8 = SPI_SUBS_ID_MONITOR;
		g_spiTx_innerFrame[g_add_cmd_ptr].CAN_ID_u8 = PSU_COMMAND;
		g_spiTx_innerFrame[g_add_cmd_ptr].CAN_DLC_u8 = 3U;
		g_spiTx_innerFrame[g_add_cmd_ptr].Payload_u8[g_add_cmd_ptr] = PC_POWER_DOWN_EXECUTE;
		g_spiTx_innerFrame[g_add_cmd_ptr].Payload_u8[1] = 0U;
		g_spiTx_innerFrame[g_add_cmd_ptr].Payload_u8[2] = 1U;

        ++g_add_cmd_ptr;
	}

    /*!********************************************************************
    * \brief Initialise the the Backup Alarm Sounder on the MMS.
    * \details When the MMS turns on, it sounds the alarm controller at full
    * volume as a Power On, Pre Test. We need to tell the Alarm Sounder to
    * stop.
    * Also set the Volume to Maximum.
    ***********************************************************************/
    void Test_Tx_commands()
    {
        //Update the volume to the MMS Alarm Controller, this should always be at maximum volume
		g_spiTx_innerFrame[g_add_cmd_ptr].Dest_Subsystem_ID_u8 = SPI_SUBS_ID_MMS_LOCAL;
		g_spiTx_innerFrame[g_add_cmd_ptr].CAN_ID_u8 = MMS_CAN_ID_ALARM;
		g_spiTx_innerFrame[g_add_cmd_ptr].CAN_DLC_u8 = 2U;
		g_spiTx_innerFrame[g_add_cmd_ptr].Payload_u8[0] = (CommandAndResponse::BYTE) MMS_LOCAL_CMD_SET_ALARM;
		g_spiTx_innerFrame[g_add_cmd_ptr].Payload_u8[1] = 2U;        // AlarmVolume max = 7U;;
		

        g_add_cmd_ptr++;

        //Construct High Priority Alarm Request
        g_spiTx_innerFrame[g_add_cmd_ptr].Dest_Subsystem_ID_u8 = SPI_SUBS_ID_MMS_LOCAL;
		g_spiTx_innerFrame[g_add_cmd_ptr].CAN_ID_u8 = MMS_CAN_ID_ALARM;
		g_spiTx_innerFrame[g_add_cmd_ptr].CAN_DLC_u8 = 2U;
		g_spiTx_innerFrame[g_add_cmd_ptr].Payload_u8[0] = (CommandAndResponse::BYTE) MMS_LOCAL_CMD_SET_ALARM;
		g_spiTx_innerFrame[g_add_cmd_ptr].Payload_u8[1] = MMS_CE_START_HIGH_PRIORITY_ALARM;

        g_add_cmd_ptr++;

        //Update the volume to the MMS Alarm Controller, this should always be at maximum volume
		g_spiTx_innerFrame[g_add_cmd_ptr].Dest_Subsystem_ID_u8 = SPI_SUBS_ID_MMS_LOCAL;
		g_spiTx_innerFrame[g_add_cmd_ptr].CAN_ID_u8 = MMS_CAN_ID_ALARM;
		g_spiTx_innerFrame[g_add_cmd_ptr].CAN_DLC_u8 = 2U;
		g_spiTx_innerFrame[g_add_cmd_ptr].Payload_u8[0] = (CommandAndResponse::BYTE) MMS_LOCAL_CMD_SET_ALARM;
		g_spiTx_innerFrame[g_add_cmd_ptr].Payload_u8[1] = 5U;        // AlarmVolume max = 7U;		

        g_add_cmd_ptr++;        

        //Update the volume to the MMS Alarm Controller, this should always be at maximum volume
		g_spiTx_innerFrame[g_add_cmd_ptr].Dest_Subsystem_ID_u8 = SPI_SUBS_ID_MMS_LOCAL;
		g_spiTx_innerFrame[g_add_cmd_ptr].CAN_ID_u8 = MMS_CAN_ID_ALARM;
		g_spiTx_innerFrame[g_add_cmd_ptr].CAN_DLC_u8 = 2U;
		g_spiTx_innerFrame[g_add_cmd_ptr].Payload_u8[0] = (CommandAndResponse::BYTE) MMS_LOCAL_CMD_SET_ALARM;
		g_spiTx_innerFrame[g_add_cmd_ptr].Payload_u8[1] = 1U;        // AlarmVolume max = 7U;;		

        g_add_cmd_ptr++;

        //Tell the alarm sounder to stop
        g_spiTx_innerFrame[g_add_cmd_ptr].Dest_Subsystem_ID_u8 = SPI_SUBS_ID_MMS_LOCAL;
		g_spiTx_innerFrame[g_add_cmd_ptr].CAN_ID_u8 = MMS_CAN_ID_ALARM;
		g_spiTx_innerFrame[g_add_cmd_ptr].CAN_DLC_u8 = 2U;
		g_spiTx_innerFrame[g_add_cmd_ptr].Payload_u8[0] = (CommandAndResponse::BYTE) MMS_LOCAL_CMD_SET_ALARM;
		g_spiTx_innerFrame[g_add_cmd_ptr].Payload_u8[1] = MMS_CE_STOP_ALARM_IMMEDIATELY;	

        ++g_add_cmd_ptr;
    }

    
    /*!********************************************************************
    * \brief Generate the CRC for the constructed packet.
    * \details Generates a CRC for the packet, and places the CRC is the
    * dedicated slot within the packet
    * \param g_SPI_UI_To_MMS_Packet The Packet to be CRC'ed.
    ***********************************************************************/
    void GenerateCRCandAddToOutboundPacket(CommandAndResponse::SPI_A_Outer_struct_Ascii_type*  g_SPI_UI_To_MMS_Packet) 
    {
        //Compute CRC
        //Fails 5-2-7, so wrap with exclusion.
        //lint -e927
        CommandAndResponse::DWORD* pdwRawPacket = (CommandAndResponse::DWORD*)(&g_SPI_UI_To_MMS_Packet->O_Start_Delimiter);
        //lint +e927
        CommandAndResponse::DWORD dwSCM_CRC_u32 = m_CFastCRC.Crc32FastBlock_fn(0xFFFFFFFFUL, (SPI_RAW_CRC_POSITION / 4U), pdwRawPacket);

        CommandAndResponse::WORD wHighWord = (CommandAndResponse::WORD)((dwSCM_CRC_u32) >> 16U);
        CommandAndResponse::WORD wLowWord = (CommandAndResponse::WORD)dwSCM_CRC_u32;

        //Place CRC in outward packet
        (void)CommandAndResponse::SPI_Bin_u16_To_ASCII_Quad_fn(wHighWord, &g_SPI_UI_To_MMS_Packet->O_CRC[0]); 
        (void)CommandAndResponse::SPI_Bin_u16_To_ASCII_Quad_fn(wLowWord, &g_SPI_UI_To_MMS_Packet->O_CRC[4]); 
    }
    

// The function to be executed by  thread1 -------------------------------------------------------------------------------
/*-----------------------------------------------------------------------------------------------------------------------*/
__attribute__((noinline)) int C_SPI_Rx_Tx_thread::exec()
{
    //C_DeviceMap_SLE *       ptr_sle7k_dev_map;
   // C_Queue *               ptrQueueSPI;
    C_Linux_SPI             *ptr_SPIdev;
    uint16_t                max_len;
    
    FILE *fd = freopen("Logfile_SPI.txt","w",stdout);
    if (fd == nullptr)   {
        printf("\n\t Error = Logfile_SPI.txt was not created !!! \n");
    }

    ptr_SPIdev = getSPI_dev1();
     // use arg1_, arg2_....sponse::Ra
    Test_Tx_commands();
    RequestPSUToShutDown();
    g_NumOf_Tx_cmd = g_add_cmd_ptr;

    /* log the inner Tx frame */
    // pthread_mutex_lock(&mutex);
    // unsigned char line = 0;
    // for (u_int16_t i = 0; i < g_NumOf_Tx_cmd * sizeof(CommandAndResponse::Raw_UI_To_MMS_Data_struct_type); ++i)   {
    //    //printf("\t g_spiTx_innerFrame_buff[%d] = %d; ", i, g_spiTx_innerFrame[i]);
    //     ++line;
    //     if (line > sizeof(CommandAndResponse::Raw_UI_To_MMS_Data_struct_type))   {     
    //         line = 0;
    //         printf("\n");
    //     }
    // }   
    // pthread_mutex_unlock(&mutex);


                
    //Populate ASCII Hex Buffer for Tx.
    //unsigned char byNumOfPopulatedEntrys = pCallingClass->GetuiNumOfPopulatedEntrysUI();  // max = 77 commands / messages

    /* convert inner frame into outer Tx frame g_SPI_UI_To_MMS_Packet in order to Tx over spi , CAN*/
    (void) CommandAndResponse::SPI_Construct_UI_To_MMS_Packet(g_spiTx_innerFrame, g_NumOf_Tx_cmd, g_SPI_UI_To_MMS_Packet);

    //Compute CRC
    GenerateCRCandAddToOutboundPacket(&g_SPI_UI_To_MMS_Packet);

    //ioData.Buffer = &g_SPI_UI_To_MMS_Packet;


    /* log the Outer frame Tx */
    // pthread_mutex_lock(&mutex);
    // line = 0;
    // for (uint16_t i = 0; i < sizeof(CommandAndResponse::SPI_A_Outer_struct_Ascii_type); ++i)   {
    //    // printf("\t g_SPI_UI_To_MMS_Packet_buff[%d] = %d; ", i, g_SPI_UI_To_MMS_Packet);
    //     ++line;
    //     /*
    //     if (line > sizeof(CommandAndResponse::SPI_A_Outer_struct_Ascii_type))   {     
    //         line = 0;
    //         printf("\n");
    //     }*/
    // }   
    // pthread_mutex_unlock(&mutex);

    //    test_read_write(ptrSPIdev);
    spi_Tx(ptr_SPIdev);      /* run the spi dart Tx*/
    // SPI_run_Rx_Tx(ptrSPIdev);           
    /* read and write SPI with g_SPI_Rx_outerFrame.outerRx_buff[] and Tx_buff */
    

    /* store / update data in the shared memory */
    // pthread_mutex_lock(&mutex);
    // for (uint16_t i = 0; i < SPI_RAW_RX_TX_BUFFER_SIZE; ++i)   {
    //     //ptr_sle7k_dev_map->m_thread1[i] = g_SPI_Rx_outerFrame.outerRx_buff[i];          // (unsigned char) i + 'A';
    //    printf("\tThread1 runing. g_SPI_Rx_outerFrame.g_SPI_Rx_outerFrame.outerRx_buff[%l] = %c; \n", i, ptr_sle7k_dev_map->m_thread1[i]);
    // }   
    // pthread_mutex_unlock(&mutex);


    /* decode from outer frame into the inner frame of Rx  */
    //SPI_Decode_MMS_To_UI_Packet_fn(g_SPI_Rx_outerFrame.g_SPI_Rx_outerFrame.outerRx_buff, innerRxBuffMsg, SPI_RAW_RX_TX_BUFFER_SIZE);
 /*   max_len = SPI_RAW_RX_TX_BUFFER_SIZE;
    // g_SPI_MMS_To_UI_Packet = &g_SPI_Rx_outerFrame.spi_Rx_outerFrame;
    bool bSPIOK = SPI_Decode_MMS_To_UI_Packet_fn(g_SPI_Rx_outerFrame.spi_Rx_outerFrame, &g_SPI_Rx_InnerFrame.spi_Rx_InnerFrame, max_len);
*/
    /* add the new frame, innerRxBuffMsg, in the queue */
  /*  bool bSPI_innerAdded2Queue = ptrQueueSPI->addArray2queue(g_SPI_Rx_InnerFrame.InnerRx_buff, sizeof(g_SPI_Rx_InnerFrame.InnerRx_buff));        

    pthread_mutex_lock(&mutex);
    for (int i = 0; i < MAX_NUM_OF_INNER_PACKETS_SPI_Rx; ++i)   {
        printf("\t QueueSPI Rx_buff[%d] = %c; \n", i, ptrQueueSPI->queue[i] );
    }   
    pthread_mutex_unlock(&mutex);
   */
    fclose (stdout);
    return EXIT_SUCCESS;
}

