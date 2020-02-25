#include "msp430.h"
#include "lcd_msp.h"
#include <stdio.h>
#include <stdint.h>

static uint16_t temp=0;
void wait();
void yazdir( int, int,int);

void main( void )
{
  WDTCTL = WDTPW + WDTHOLD;
  
     BCSCTL1 = CALBC1_1MHZ;// 1 MHZ KRISTAL SEÇILDI..
     DCOCTL  = CALDCO_1MHZ;// 1 MHZ KRISTAL, DAHILI (DCO) OLANI SEÇILDI..
 
  __delay_cycles(100000);//KALIBRASYON IÇIN BEKLEME

static int R=0,G=0,B=0;

lcd_init();
P1DIR =(~(BIT1));// P1.0,1 GIRIS,DIGERLERI ÇIKIS
P1SEL =BIT1;//P1.1 Timer_A CCIxA SET EDILDI 
P1SEL2 =0x00;//P1.1 Timer_A CCIxA SET EDILDI 
P2DIR =0xFF;
P2OUT|=0x01;//S0--1     // ÇIKIS FREAKANS ÖLÇEGI
P2OUT&=0xFD;//S1--0     // %20 OLARAK AYARLANDI
  //LCD YE YAZI YAZMA KISMI
lcd_goto(1,2);                     //LCD 1. SATIR
lcd_puts("MICROPROCESSORS");
lcd_goto(2,2);                        //LCD 2.SATIR
lcd_puts("COLOR DETECTION");
delay_ms(2000);                       //LCD YE YAZILDIKTAN SONRA 2 SN BEKLEYIP
lcd_temizle();                        // SONRA TEMIZLEME ISLEMI
 //CAPTURE MODE VE TIMERA AYARI
 //INEN VE YUKSELEN KENAR,CCIxA SEÇIMI,CAPTURE\COMPARE  SENKRON,CAPTURE\COMPARE KESMESI AKTIF
TACCTL0 = CM_3| CCIS_0 | SCS| CCIE| CAP ;
// TIMER BASLAMA:SMCLK,FREKANS BÖLME YOK,CONTINOUS UP MODE AKTIF,SAYICIYI TEMIZLEME AKTIF
TACTL = TASSEL_2 | ID_0 | MC_2 | TACLR;
//KESMELER AKTIF
//__enable_interrupt();
while(1){
  
    P2OUT&=0xF3;//S2,S3 --00 //KIRMIZI FILTRE AKTIF
    wait();                // BEKLEME
    R=temp*1;              // TEMP DEGERI ATAMA 
    wait();               // BEKLEME                          
   temp=0;               // TEMP I SIFIRLAMAK 
 __low_power_mode_0();    // BURADA ISLEMCI KESME GELENE KADAR UYKUYA GEÇER       
 
 P2OUT|=0x0C; //S2,S3 --11 YESIL FILTRE AKTIF
  wait();                 // BEKLEME
  G=temp*1;               // TEMP DEGERI ATAMA 
  wait();                // BEKLEME   
  temp=0;                // TEMP I SIFIRLAMAK 
 __low_power_mode_0();   // BURADA ISLEMCI KESME GELENE KADAR UYKUYA GEÇER       
 
 P2OUT&=0xFB;//S2,S3 --11 MAVI FILTRE AKTIF
 wait();              // BEKLEME
 B=temp*1;            //  TEMP DEGERI ATAMA 
 wait();              //BEKLEME
 temp=0;               // BEKLEME    
 yazdir(R,G,B);        // TEMP I SIFIRLAMAK 
 __low_power_mode_0(); // BURADA ISLEMCI KESME GELENE KADAR UYKUYA GEÇER       
    }

   
  
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMERA0_ISR ( void ) // Flag cleared automatically
{
static uint16_t LastTime = 0;//GELECEK KESME IÇIN BIR ÖNCEKI TACCRO DEGERINI BUNA ATMA
 
//GELECEK KESME IÇIN BIR ÖNCEKI TACCRO DEGERINI SIMDIKI TACCRO DEGERINDEN ÇIKARIP GEÇEN SÜREYI HESAPLAMA
temp=(TACCR0 - LastTime); // TEMP E ARADAKI SÜREYI ATAMA ISLEMI
LastTime = TACCR0; //GELECEK KESME IÇIN BIR ÖNCEKI TACCRO DEGERINI BUNA ATMA
__low_power_mode_off_on_exit() ;//UYKU MODUNUNDAN ÇIKMASI IÇIN

}

void yazdir(int kir, int yes,int mav)
{
  
char kirmizi[8];
char yesil[8];
char mavi[8];
   //HER ALINAN R G B  DDEGERLERI LCD DE 1. SATIRA BASMA ISLEMI
lcd_goto(1,1);
lcd_puts("R=");   
sprintf(kirmizi,"%d",kir);
lcd_puts(kirmizi);

lcd_puts("G=");
sprintf(yesil,"%d",yes);
lcd_puts(yesil);

lcd_puts("B=");
sprintf(mavi,"%d",mav);
lcd_puts(mavi);

lcd_goto(2,2);
     
     if(kir>59& kir<80& mav>83 & mav<100 & yes>115 & yes<135){ 
         lcd_puts("kirmizi     ");//kirmizi
     
     }
      
     if(kir>70& kir<85 & mav>72 & mav<85 & yes>109 & yes<123){ 
         lcd_puts("magenta     ");//magenta
     
     }
      
     if(kir>95 & kir<125 & mav>68 & mav<95& yes>75 & yes<99){ 
         lcd_puts("yesil       ");//yesil
     
     }
      if(kir>95& kir<135 & mav>63 & mav<80 & yes>105 & yes<125){ 
         lcd_puts("lacivert    ");//lacivert
     
     } 
     if(kir>51& kir<61 & mav>49 & mav<59 & yes>73 & yes<87){ 
         lcd_puts("pembe       ");//pempe
     
     }
     
     if(kir>44& kir<54 & mav>60 & mav<70 & yes>51 & yes<61){ 
         lcd_puts("sari        ");//sari
     
     }
      if(kir>46 & kir<56 & mav>68 & mav<85 & yes>65 & yes<90){ 
         lcd_puts("turuncu     ");//TURUNCU
     
     }
      if(kir>109 & kir<129 & mav>38 & mav<58 & yes>71 & yes<90){ 
         lcd_puts("cyan    ");//cyan
     
     }
      
     if(kir>40 & kir<60 & mav>30 & mav<50 & yes>40 & yes<60){ 
         lcd_puts("beyaz      ");//beyaz
     
     }

}
void wait(){
 
int i=0;
 for(i=0;i<3000;i++){
}
}


