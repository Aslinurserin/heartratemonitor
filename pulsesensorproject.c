#include <intrinsics.h>
#include <io430.h>
int gelendeger = 0;
void uart_init(void);
void adc_init(void);
int main(void)
{
WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer 
DCOCTL = 0;
BCSCTL1 = CALBC1_1MHZ; 
DCOCTL = CALDCO_1MHZ;
__delay_cycles(1000000);
P1SEL = BIT1 + BIT2; 
P1SEL2 = BIT1 + BIT2;
adc_init();
uart_init();
__bis_SR_register(LPM1_bits+GIE); 
while(1);
}
void uart_init(void)
{
UCA0CTL1 |= UCSSEL_2; //SMCLK islemciden gelen clocku aldi
UCA0BR0 = 104; // 1000000/9600 BAUDRATE AYARLAMAK baund rate msw bit
UCA0BR1 = 0; // lsw biti baund hizi ayarlamak için
UCA0MCTL = UCBRS2 + UCBRS1; // tam çikmayan sonuçlarda hata payi düzeltmek için hata 
payini gidermek icin modulasyon katsayisi seçildi
UCA0CTL1 &= ~UCSWRST; //uca0ctl1 de ucswrst resetleme biti bulunmaktadir yazilimsal
olarak temizlenmezse uart çalismayacaktir
}
9
void adc_init(void)
{
ADC10CTL0 &= ~ENC; //Can be modified only when ENC = 0
ADC10CTL0 = ADC10SHT_3 + ADC10ON + REF0N + ADC10IE; // ADC10ON, interrupt 
enabled , X64 örnekleme kaç adc clock darbesi alnacagini söyler
ADC10CTL1 = INCH_4 ; // input p1.4 analog giris ayari
ADC10AE0 |= BIT4; // Analog (input) enable control register satlayicisina bit 4 girise seçtigimiz
pini aktif etmek için
ADC10CTL0 |= ENC + ADC10SC; // Sampling and conversion start
}
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
gelendeger = ADC10MEM; // sensörden alinan veri adc memory ye kaydoluyor biz de bu veriyi
gelen deger degiskenine atiyoruz
while (!(IFG2&UCA0TXIFG)); //TX müsait olana kadar bekle
UCA0TXBUF = gelendeger; // bu degiskende tutulan degeri tx bufferina aktararak uartla
gönerilecek veriyi söyelmis oluyoruz.
ADC10CTL0 |= ENC + ADC10SC; // enc biti adc10sc biti ile kullanilir adc çevrsm tek 
çevrim modundayken adc10sc biti ile bu bitte logic bir yapildiginda baslamaktadir.
}
