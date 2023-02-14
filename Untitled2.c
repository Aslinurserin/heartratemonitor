#include <intrinsics.h>
#include <io430.h>
int gelendeger = 0;
void uart_init(void);
void adc_init(void);
int main(void)
{
WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer
DCOCTL = 0;
BCSCTL1 = CALBC1_1MHZ; // Clock saatini 1 MHZ ayarlamak
DCOCTL = CALDCO_1MHZ;
__delay_cycles(1000000);
P1SEL = BIT1 + BIT2; // P1.1/P1.2 rx/tx olarak kullanmak i�in yapilan ayar g�revleri se�iyor 
ikisinide aktif edince haberlesme pinlerini kullaniyoruz
P1SEL2 = BIT1 + BIT2;
adc_init();
uart_init();
__bis_SR_register(LPM1_bits+GIE); // lmpx modundayken receivden bir sey gelirse uyaniyor
while(1);
}
void uart_init(void)
{
UCA0CTL1 |= UCSSEL_2; //SMCLK islemciden gelen clocku aldi
UCA0BR0 = 104; // 1000000/9600 BAUDRATE AYARLAMAK baund rate msw bit
UCA0BR1 = 0; // lsw biti baund hizi ayarlamak i�in
UCA0MCTL = UCBRS2 + UCBRS1; // tam �ikmayan sonu�larda hata payi d�zeltmek i�in hata 
payini gidermek icin modulasyon katsayisi se�ildi
UCA0CTL1 &= ~UCSWRST; //uca0ctl1 de ucswrst resetleme biti bulunmaktadir yazilimsal
olarak temizlenmezse uart �alismayacaktir
}
9
void adc_init(void)
{
ADC10CTL0 &= ~ENC; //Can be modified only when ENC = 0
ADC10CTL0 = ADC10SHT_3 + ADC10ON + REF0N + ADC10IE; // ADC10ON, interrupt 
enabled , X64 �rnekleme ka� adc clock darbesi alnacagini s�yler
ADC10CTL1 = INCH_4 ; // input p1.4 analog giris ayari
ADC10AE0 |= BIT4; // Analog (input) enable control register satlayicisina bit 4 girise se�tigimiz
pini aktif etmek i�in
ADC10CTL0 |= ENC + ADC10SC; // Sampling and conversion start
}
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
gelendeger = ADC10MEM; // sens�rden alinan veri adc memory ye kaydoluyor biz de bu veriyi
gelen deger degiskenine atiyoruz
while (!(IFG2&UCA0TXIFG)); //TX m�sait olana kadar bekle
UCA0TXBUF = gelendeger; // bu degiskende tutulan degeri tx bufferina aktararak uartla
g�nerilecek veriyi s�yelmis oluyoruz.
ADC10CTL0 |= ENC + ADC10SC; // enc biti adc10sc biti ile kullanilir adc �evrsm tek 
�evrim modundayken adc10sc biti ile bu bitte logic bir yapildiginda baslamaktadir
}
