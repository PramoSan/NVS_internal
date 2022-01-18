#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Clock.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>

/* Application Header files */
#include <ti_radio_config.h>

/* EasyLink API Header files */
#include "easylink/EasyLink.h"

/* Driver Header files */
#include <ti/display/Display.h>
#include <ti/drivers/NVS.h>

/* Driver configuration */
#include "ti_drivers_config.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

#define FOOTER "=================================================="

#define CHUNK 250
static char buffer[CHUNK]; //Buffer placed in RAM to hold bytes read from non-volatile storage.
//static const char signature[1024] = {"SimpleLink 1234567890 Non-Volatile Storage"};


/* Undefine to not use async mode */
#define RFEASYLINKECHO_ASYNC

#define RFEASYLINKECHO_TASK_STACK_SIZE    1024
#define RFEASYLINKECHO_TASK_PRIORITY      2
#define RFEASYLINKECHO_PAYLOAD_LENGTH     3

Task_Struct echoTask;    /* not static so you can see in ROV */
static Task_Params echoTaskParams;
static uint8_t echoTaskStack[RFEASYLINKECHO_TASK_STACK_SIZE];

#ifdef RFEASYLINKECHO_ASYNC
static Semaphore_Handle echoDoneSem;
#endif //RFEASYLINKECHO_ASYNC

static bool bBlockTransmit = false;

EasyLink_TxPacket txPacket = {{0}, 0, 0, {0}};

#ifdef RFEASYLINKECHO_ASYNC
void echoTxDoneCb(EasyLink_Status status)
{
    if (status == EasyLink_Status_Success)
    {
        /* Toggle RLED to indicate Echo TX, clear GLED */
        GPIO_toggle(CONFIG_GPIO_RLED);
        GPIO_write(CONFIG_GPIO_GLED, CONFIG_GPIO_LED_OFF);
    }
    else
    {
        /* Set GLED and clear RLED to indicate error */
        GPIO_write(CONFIG_GPIO_GLED, CONFIG_GPIO_LED_ON);
        GPIO_write(CONFIG_GPIO_RLED, CONFIG_GPIO_LED_OFF);
    }

    Semaphore_post(echoDoneSem);
}

void echoRxDoneCb(EasyLink_RxPacket * rxPacket, EasyLink_Status status)
{
    int i;
    FILE * fptr;
//    int8_t rssi;
//    uint32_t absTime;

    if (status == EasyLink_Status_Success)
    {
    /* Toggle RLED to indicate RX, clear GLED */
    GPIO_toggle(CONFIG_GPIO_RLED);
    GPIO_write(CONFIG_GPIO_GLED, CONFIG_GPIO_LED_OFF);
    memcpy(&txPacket.payload, rxPacket->payload, rxPacket->len);
    /* Permit echo transmission */
    bBlockTransmit = false;

////////////////////////////////////////////////
//    fptr = fopen("/home/pramodh/Rangetest_data.txt", "a");
//    fprintf(fptr, "\n new RX session \n", rxPacket->payload[0], i);
//    fclose(fptr);
/////////////////////////////////////////////////
      for(i=0; i< rxPacket->len; i++)
      {
//      fptr = fopen("/home/pramodh/Rangetest_data.txt", "a");

    NVS_Handle nvsHandle;
    NVS_Attrs regionAttrs;
    NVS_Params nvsParams;
    Display_Handle displayHandle;
    Display_init();
    NVS_init();

    displayHandle = Display_open(Display_Type_UART, NULL);
    if (displayHandle == NULL)
    {
        /* Display_open() failed */
        while (1);
    }

    NVS_Params_init(&nvsParams);
    nvsHandle = NVS_open(CONFIG_NVSINTERNAL, &nvsParams);
    if (nvsHandle == NULL)
    {
        Display_printf(displayHandle, 0, 0, "NVS_open() failed.");
        return (NULL);
    }

//////////////////////Loop Start Here////////////////////////////////

        Display_printf(displayHandle, 0, 0, "\n");
        NVS_getAttrs(nvsHandle, &regionAttrs);

        Display_printf(displayHandle, 0, 0, FOOTER);
        Display_printf(displayHandle, 0, 0, "Total bytes: %ibytes", rxPacket->len);
        Display_printf(displayHandle, 0, 0, "Region Base Address: 0x%x",regionAttrs.regionBase);
        Display_printf(displayHandle, 0, 0, "Sector Size: 0x%x",regionAttrs.sectorSize);
        Display_printf(displayHandle, 0, 0, "Region Size: 0x%x",regionAttrs.regionSize);
        Display_printf(displayHandle, 0, 0, "Writing signature to flash...\n");

            NVS_write(nvsHandle, rxPacket->payload[i], (void *) buffer, sizeof(buffer),NVS_WRITE_ERASE | NVS_WRITE_POST_VERIFY);
            NVS_read(nvsHandle, rxPacket->payload[i], (void *) buffer, sizeof(buffer));


           /* Write signature directly from the NVS region to the UART console. */
            Display_printf(displayHandle, 0, 0, "%s", regionAttrs.regionBase);
            Display_printf(displayHandle, 0, 0, "\n===== Elements =====\n");
            //Display_printf(displayHandle, 0, 0, "Index\tFile\tNVS");
            Display_printf(displayHandle, 0, 0, "Index\tNVS");

            int i;
            for(i=0; i < ARRAY_SIZE(buffer); i++)
               {
         // Display_printf(displayHandle, 0, 0, "%d\t%c\t%c", i, signature[i], buffer[i]);
            Display_printf(displayHandle, 0, 0, "%d\t%c", i, rxPacket->payload[i]);
               }

        // Display_printf(displayHandle, 0, 0, "Erasing flash sector...");
        // NVS_erase(nvsHandle, 0, regionAttrs.sectorSize);
        // NVS_close(nvsHandle);
        // Display_printf(displayHandle, 0, 0, "Reset the device.");

            Display_printf(displayHandle, 0, 0, FOOTER);

         }

        fclose(fptr);

    }
    else
    {
        /* Set GLED and clear RLED to indicate error */
        GPIO_write(CONFIG_GPIO_GLED, CONFIG_GPIO_LED_ON);
        GPIO_write(CONFIG_GPIO_RLED, CONFIG_GPIO_LED_OFF);
        /* Block echo transmission */
        bBlockTransmit = true;
        printf("blocked rx \n");

    }

    Semaphore_post(echoDoneSem);
}
#endif //RFEASYLINKECHO_ASYNC

static void rfEasyLinkEchoRxFnx(UArg arg0, UArg arg1)
{
    uint32_t absTime;

#ifdef RFEASYLINKECHO_ASYNC
    /* Create a semaphore for Async */
    Semaphore_Params params;
    Error_Block      eb;

    /* Init params */
    Semaphore_Params_init(&params);
    Error_init(&eb);

    /* Create semaphore instance */
    echoDoneSem = Semaphore_create(0, &params, &eb);
    if(echoDoneSem == NULL)
    {
        System_abort("Semaphore creation failed");
        printf("Semaphore creation failed \n");

    }

#else
    EasyLink_RxPacket rxPacket = {{0}, 0, 0, 0, 0, {0}};
#endif //RFEASYLINKECHO_ASYNC

    // Initialize the EasyLink parameters to their default values
    EasyLink_Params easyLink_params;
    EasyLink_Params_init(&easyLink_params);

    /*
     * Initialize EasyLink with the settings found in ti_easylink_config.h
     * Modify EASYLINK_PARAM_CONFIG in ti_easylink_config.h to change the default
     * PHY
     */
    if(EasyLink_init(&easyLink_params) != EasyLink_Status_Success)
    {
        System_abort("EasyLink_init failed");
        printf("EasyLink_init failed \n");
    }

    /*
     * If you wish to use a frequency other than the default, use
     * the following API:
     * EasyLink_setFrequency(868000000);
     */

    while(1) {
#ifdef RFEASYLINKECHO_ASYNC
        // Wait to receive a packet
        EasyLink_receiveAsync(echoRxDoneCb, 0);

        /* Wait indefinitely for Rx */
        Semaphore_pend(echoDoneSem, BIOS_WAIT_FOREVER);
#else
        rxPacket.absTime = 0;
        EasyLink_Status result = EasyLink_receive(&rxPacket);

        if (result == EasyLink_Status_Success)
        {
            /* Toggle RLED to indicate RX, clear GLED */
            GPIO_toggle(CONFIG_GPIO_RLED);
            GPIO_write(CONFIG_GPIO_GLED, CONFIG_GPIO_LED_OFF);
            /* Copy contents of RX packet to TX packet */
            memcpy(&txPacket.payload, &rxPacket.payload, rxPacket.len);
            /* Permit echo transmission */
            bBlockTransmit = false;
        }
        else
        {
            /* Set GLED and clear RLED to indicate error */
            GPIO_write(CONFIG_GPIO_GLED, CONFIG_GPIO_LED_ON);
            GPIO_write(CONFIG_GPIO_RLED, CONFIG_GPIO_LED_OFF);
            /* Block echo transmission */
            bBlockTransmit = true;
        }
#endif // RFEASYLINKECHO_ASYNC

        if(bBlockTransmit == false)
        {
            /* Switch to Transmitter and echo the packet if transmission
             * is not blocked
             */
            txPacket.len = RFEASYLINKECHO_PAYLOAD_LENGTH;

            /*
             * Address filtering is enabled by default on the Rx device with the
             * an address of 0xAA. This device must set the dstAddr accordingly.
             */
            txPacket.dstAddr[0] = 0xaa;

            /* Set Tx absolute time to current time + 100ms*/
            if(EasyLink_getAbsTime(&absTime) != EasyLink_Status_Success)
            {
                // Problem getting absolute time
            }
            txPacket.absTime = absTime + EasyLink_ms_To_RadioTime(100);

#ifdef RFEASYLINKECHO_ASYNC
            EasyLink_transmitAsync(&txPacket, echoTxDoneCb);

            /* Wait for Tx to complete. A Successful TX will cause the echoTxDoneCb
             * to be called and the echoDoneSem to be released, so we must
             * consume the echoDoneSem
             */
            Semaphore_pend(echoDoneSem, BIOS_WAIT_FOREVER);

#else
            EasyLink_Status result = EasyLink_transmit(&txPacket);

            if (result == EasyLink_Status_Success)
            {
                /* Toggle RLED to indicate Echo TX, clear GLED */
                GPIO_toggle(CONFIG_GPIO_RLED);
                GPIO_write(CONFIG_GPIO_GLED, CONFIG_GPIO_LED_OFF);
            }
            else
            {
                /* Set GLED and clear RLED to indicate error */
                GPIO_write(CONFIG_GPIO_GLED, CONFIG_GPIO_LED_ON);
                GPIO_write(CONFIG_GPIO_RLED, CONFIG_GPIO_LED_OFF);
            }
#endif //RFEASYLINKECHO_ASYNC
        }
    }
}

void echoTask_init() {

    Task_Params_init(&echoTaskParams);
    echoTaskParams.stackSize = RFEASYLINKECHO_TASK_STACK_SIZE;
    echoTaskParams.priority = RFEASYLINKECHO_TASK_PRIORITY;
    echoTaskParams.stack = &echoTaskStack;
    echoTaskParams.arg0 = (UInt)1000000;

    Task_construct(&echoTask, rfEasyLinkEchoRxFnx, &echoTaskParams, NULL);
}

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    Board_initGeneral();
        GPIO_init();

        GPIO_setConfig(CONFIG_GPIO_RLED, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
        GPIO_setConfig(CONFIG_GPIO_GLED, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

        GPIO_write(CONFIG_GPIO_RLED, CONFIG_GPIO_LED_OFF);
        GPIO_write(CONFIG_GPIO_GLED, CONFIG_GPIO_LED_OFF);

        echoTask_init();

        /* Start BIOS */
        BIOS_start();


    return (NULL);
}
