#include "esp_heap_caps.h"
#include "anim.h"
#include "val2pwm.h"
#include "i2s_parallel.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_spi_flash.h"
#include "esp_partition.h"

int brightness=15; //Change to set the global brightness of the display, range 1-63
                  //Warning when set too high: Do not look into LEDs with remaining eye.
i2s_parallel_buffer_desc_t bufdesc[2][1<<BITPLANE_CNT];
i2s_parallel_config_t cfg={
    .gpio_bus={2, 15, 4, 16, 27, 17, -1, -1, 5, 18, 19, 21, 26, 25, -1, -1},
    .gpio_clk=22,
    .bits=I2S_PARALLEL_BITS_16,
    .clkspeed_hz=20*1000*1000,
    .bufa=bufdesc[0],
    .bufb=bufdesc[1],
};

uint16_t *bitplane[2][BITPLANE_CNT];
				  
static uint32_t getpixel(unsigned char *pix, int x, int y) {
    unsigned char *p=pix+((x+y*64)*3);
    return (p[0]<<16)|(p[1]<<8)|(p[2]);
}

void LED32x64_refresh(int apos,int backbuf_id){
	//Fill bitplanes with the data for the current image
    const uint8_t *pix=&anim[apos*64*32*3];     //pixel data for this animation frame
    for (int pl=0; pl<BITPLANE_CNT; pl++) {
        int mask=(1<<(8-BITPLANE_CNT+pl)); //bitmask for pixel data in input for this bitplane
        uint16_t *p=bitplane[backbuf_id][pl]; //bitplane location to write to
        for (unsigned int y=0; y<16; y++) {
            int lbits=0;                //Precalculate line bits of the *previous* line, which is the one we're displaying now
            if ((y-1)&1) lbits|=BIT_A;
            if ((y-1)&2) lbits|=BIT_B;
            if ((y-1)&4) lbits|=BIT_C;
            if ((y-1)&8) lbits|=BIT_D;
            for (int fx=0; fx<64; fx++) {
#if DISPLAY_ROWS_SWAPPED
                int x=fx^1; //to correct for the fact that the stupid LED screen I have has each row swapped...
#else
                int x=fx;
#endif

                int v=lbits;
                //Do not show image while the line bits are changing
                if (fx<1 || fx>=brightness) v|=BIT_OE;
                if (fx==62) v|=BIT_LAT; //latch on second-to-last bit... why not last bit? Dunno, probably a timing thing.

                int c1, c2;
                c1=getpixel(pix, x, y);
                c2=getpixel(pix, x, y+16);
                if (c1 & (mask<<16)) v|=BIT_R1;
                if (c1 & (mask<<8)) v|=BIT_G1;
                if (c1 & (mask<<0)) v|=BIT_B1;
                if (c2 & (mask<<16)) v|=BIT_R2;
                if (c2 & (mask<<8)) v|=BIT_G2;
                if (c2 & (mask<<0)) v|=BIT_B2;

                    //Save the calculated value to the bitplane memory
                *p++=v;
            }
        }
    }
    //Show our work!
    i2s_parallel_flip_to_buffer(&I2S1, backbuf_id);	
}

void initImagePartition(){
	spi_flash_mmap_handle_t hrom;  
	esp_err_t err;  
	esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
	if (it==0) printf("Couldn't find storage partition - update with partition_espruino.bin\n");
	else {
		const esp_partition_t *p = esp_partition_get(it);
		err=esp_partition_mmap(p, 0, p->size, SPI_FLASH_MMAP_DATA, (const void**)&romdata_images, &hrom);
		anim = romdata_images;
		//err = esp_partition_mmap(p, 0, p->size, SPI_FLASH_MMAP_DATA, (const void**)anim, &hrom);
		if (err!=ESP_OK) printf("Couldn't map images!\n");
		// The mapping in hrom is never released - as images can be called at anytime      
	}
	esp_partition_iterator_release(it);
}

void initI2S(){
	i2sTaskName = I2STASKNAME;
    for (int i=0; i<BITPLANE_CNT; i++) {
        for (int j=0; j<2; j++) {
			bitplane[j][i]=heap_caps_malloc(BITPLANE_SZ, MALLOC_CAP_DMA);
            assert(bitplane[j][i] && "Can't allocate bitplane memory");
        }
    }

    //Do binary time division setup. Essentially, we need n of plane 0, 2n of plane 1, 4n of plane 2 etc, but that
    //needs to be divided evenly over time to stop flicker from happening. This little bit of code tries to do that
    //more-or-less elegantly.
    int times[BITPLANE_CNT]={0};
    for (int i=0; i<((1<<BITPLANE_CNT)-1); i++) {
        int ch=0;
        //Find plane that needs insertion the most
        for (int j=0; j<BITPLANE_CNT; j++) {
            if (times[j]<=times[ch]) ch=j;
        }
        //Insert the plane
        for (int j=0; j<2; j++) {
            bufdesc[j][i].memory=bitplane[j][ch];
            bufdesc[j][i].size=BITPLANE_SZ*2;
        }
        //Magic to make sure we choose this bitplane an appropriate time later next time
        times[ch]+=(1<<(BITPLANE_CNT-ch));
    }

    //End markers
    bufdesc[0][((1<<BITPLANE_CNT)-1)].memory=NULL;
    bufdesc[1][((1<<BITPLANE_CNT)-1)].memory=NULL;

    //Setup I2S
    i2s_parallel_setup(&I2S1, &cfg);
	
}
