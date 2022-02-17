
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

/* Driver Header files */
#include <ti/display/Display.h>
#include <ti/drivers/NVS.h>

/* Driver configuration */
#include "ti_drivers_config.h"

#define FOOTER "=================================================="
#define MAXBUFLEN 50

char buffer[MAXBUFLEN];
char rbuffer[MAXBUFLEN];

int i = 0;

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    uint_fast16_t status;
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
    nvsHandle = NVS_open(CONFIG_NVSEXTERNAL, &nvsParams);
    if (nvsHandle == NULL)
    {
        Display_printf(displayHandle, 0, 0, "NVS_open() failed.");
        return (NULL);
    }

    uint16_t newLen = 0;
    uint16_t totalRead = 0;
    uint8_t readRound = 0;
    uint8_t round = 0;
    uint8_t roundn = 0; //for final display @line 117
    int fileSize = 0;

    NVS_getAttrs(nvsHandle, &regionAttrs);
    size_t round_offset = 4;


    FILE *fp = fopen("/home/pramodh/rdnvs.txt", "r");
    if (fp != NULL)
    {
        fseek(fp, 0, SEEK_END);
        fileSize = ftell(fp);
        fseek(fp, 0L, SEEK_SET);
        Display_printf(displayHandle, 0, 0, "========================================");
        Display_printf(displayHandle, 0, 0, "Total file size = %d ", fileSize);

        readRound = ((fileSize % MAXBUFLEN) == 0)? 0 :1 ;
        readRound += (fileSize/MAXBUFLEN);
        Display_printf(displayHandle, 0, 0, "Need to read %d times \n", readRound);

        //erase everything at first
        NVS_erase(nvsHandle, 0, regionAttrs.sectorSize);
        NVS_write(nvsHandle, 0, 0, sizeof(MAXBUFLEN+1), NVS_WRITE_ERASE | NVS_WRITE_POST_VERIFY);
        memset(rbuffer, 0, (MAXBUFLEN));
        memset(buffer, 0, (MAXBUFLEN));

    while (round < (readRound+1))
    {
        Display_printf(displayHandle, 0, 0, "Writing round %d", round);
        //Read from file and store on buffer here
        fseek(fp, 0 + round * MAXBUFLEN, SEEK_CUR);
        newLen = fread(buffer, sizeof(char), MAXBUFLEN, fp);

        //Write to NVS
        Display_printf(displayHandle, 0, 0, "%s ", buffer);
        Display_printf(displayHandle, 0, 0, "Writing to regionBase: 0x%x", regionAttrs.regionBase);
        //NVS_write(nvsHandle, regionAttrs.regionBase, (void *) buffer, sizeof(buffer), NVS_WRITE_POST_VERIFY);
        status = NVS_write(nvsHandle, round_offset, (void *) buffer, sizeof(buffer), NVS_WRITE_POST_VERIFY);
        Display_printf(displayHandle, 0, 0, "status >>>>>>>>>>>>>>>>> %d",  status);

        totalRead += newLen;

        Display_printf(displayHandle, 0, 0, "reading from %d offset ===== regionBase: 0x%x", round_offset, regionAttrs.regionBase);
        NVS_read(nvsHandle, round_offset, (void *) rbuffer, sizeof(buffer));
        Display_printf(displayHandle, 0, 0, "%s \n", rbuffer);

        memset(rbuffer, 0, (MAXBUFLEN));
        memset(buffer, 0, (MAXBUFLEN));

        round_offset = (MAXBUFLEN + 1)+ round_offset;
        round++;
        fseek(fp, 0L, SEEK_SET);
    }

 sleep(1);

    //display all chunks at once after writing
    if(totalRead == fileSize)
    {
        int displayRound=0;
        displayRound = ((fileSize % MAXBUFLEN) == 0)? 0 :1 ;
        displayRound += (fileSize/MAXBUFLEN);
        size_t round_offset = 4;


        while (roundn < (displayRound + 1))
        {
        Display_printf(displayHandle, 0, 0, "Reading round %d ", roundn);
        NVS_read(nvsHandle, round_offset, rbuffer, sizeof(buffer));
      //  Display_printf(displayHandle, 0, 0, "Data at offset: 0x%x", round_offset);
        Display_printf(displayHandle, 0, 0, "Data at %d offset ==== regionBase: 0x%x", round_offset, regionAttrs.regionBase);
        Display_printf(displayHandle, 0, 0, "%s \n", rbuffer);
        round_offset = (MAXBUFLEN + 1) + round_offset;
        memset(rbuffer, 0, (MAXBUFLEN));
        roundn++;
        }
        Display_printf(displayHandle, 0, 0, "END of elements Reading");
       sleep(1);

    }

    if (ferror(fp) != 0)
        {
            Display_printf(displayHandle, 0, 0, "Error reading file");
        }
        else
        {
            buffer[newLen++] = '\0';
        }
        readRound += MAXBUFLEN;
    }

        fclose(fp);

        //erase everything at end
        NVS_erase(nvsHandle, 0, regionAttrs.sectorSize);
        NVS_write(nvsHandle, 0, 0, sizeof(MAXBUFLEN+1), NVS_WRITE_ERASE | NVS_WRITE_POST_VERIFY);
        memset(rbuffer, 0, (MAXBUFLEN));
        memset(buffer, 0, (MAXBUFLEN));
        sleep(1);
    Display_printf(displayHandle, 0, 0, "=============All Erased===============");

      NVS_close(nvsHandle);
      return (NULL);
}
