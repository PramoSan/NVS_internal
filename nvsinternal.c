#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

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

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
   FILE *fp;
     //  char fileBuffer[250];
     //  char signature[ARRAY_SIZE(buffer)];
       size_t nread;

       fp = fopen("/home/pramodh/rdnvs.txt", "r");
       fseek(fp, 0, SEEK_END); //read file size
       int totalbytes = ftell(fp);
       fseek(fp, 0L, SEEK_SET);//reset pointer

//       fgets(fileBuffer, ARRAY_SIZE(buffer), (FILE*)fp);
//       memcpy(signature, fileBuffer,strlen(fileBuffer)+1);
//       memset(fileBuffer, ARRAY_SIZE(buffer), ARRAY_SIZE(fileBuffer) );   //Clear the contents of fileBuffer

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

    int segment = totalbytes/sizeof(buffer);
    int start=0;
    int remain;

  do{

    NVS_getAttrs(nvsHandle, &regionAttrs);

    remain = (totalbytes - start);
    start = ftell(fp);

  if(remain > 0)
      {
        Display_printf(displayHandle, 0, 0, FOOTER);
        Display_printf(displayHandle, 0, 0, "Total bytes: %ibytes", totalbytes);
        Display_printf(displayHandle, 0, 0, "Region Base Address: 0x%x",regionAttrs.regionBase);
        Display_printf(displayHandle, 0, 0, "Sector Size: 0x%x",regionAttrs.sectorSize);
        Display_printf(displayHandle, 0, 0, "Region Size: 0x%x",regionAttrs.regionSize);
        Display_printf(displayHandle, 0, 0, "Writing signature to flash...\n");
        Display_printf(displayHandle, 0, 0, "starting at: %i byte \t", (start+1));
        Display_printf(displayHandle, 0, 0, "remains: %i bytes", remain);

        while ((nread = fread(buffer, (start+1), sizeof buffer, fp)) > 0)
        Display_printf(displayHandle, 0, 0, "fread %i byte \t", (start+1));

        NVS_write(nvsHandle, start, (void *) buffer, sizeof(buffer),NVS_WRITE_ERASE | NVS_WRITE_POST_VERIFY);
        NVS_read(nvsHandle, start, (void *) buffer, sizeof(buffer));

        start = start + sizeof(buffer);
        fseek(fp,(start),SEEK_SET);

       /* Write signature directly from the NVS region to the UART console. */
        Display_printf(displayHandle, 0, 0, "%s", regionAttrs.regionBase);
        Display_printf(displayHandle, 0, 0, "\n===== Elements =====\n");
        //Display_printf(displayHandle, 0, 0, "Index\tFile\tNVS");
        Display_printf(displayHandle, 0, 0, "Index\tNVS");

        int i;
        for(i=0; i < ARRAY_SIZE(buffer); i++)
           {
     // Display_printf(displayHandle, 0, 0, "%d\t%c\t%c", i, signature[i], buffer[i]);
        Display_printf(displayHandle, 0, 0, "%d\t%c", i, buffer[i]);
           }

    // Display_printf(displayHandle, 0, 0, "Erasing flash sector...");
    // NVS_erase(nvsHandle, 0, regionAttrs.sectorSize);
    // NVS_close(nvsHandle);
    // Display_printf(displayHandle, 0, 0, "Reset the device.");

        Display_printf(displayHandle, 0, 0, FOOTER);

       } //if end here from line 82

} while(remain > 0);

    fclose(fp);
    return (NULL);
}
