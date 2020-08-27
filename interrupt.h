#include "xscugic.h"
#include "xil_exception.h"
#include "xgpio.h"

// Define platform macro
#define  INTC_DEVICE_ID	  XPAR_SCUGIC_SINGLE_DEVICE_ID
#define  BUTTON_DEVICE_ID       XPAR_GPIO_0_DEVICE_ID
#define  BUTTONS_IP2INTC      XPAR_FABRIC_PERIPHERAL_SECTION_BUTTON_IP2INTC_IRPT_INTR

// Define Variables 
XScuGic  Intc;
static XScuGic_Config *IntcConfig;
XGpio butn;


// Function decalreation
// interrupt
void Interrupt_Initialize();
void Interrupt_Handler_Connect();//XGpio * InstancePtr

// peripheral
void Button_Initialize(XGpio * InstancePtr, u16 DeviceId);

// callback handler
void button_press_handler();

// function body
void Interrupt_Initialize(){

	u32 Status;
	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 */
	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	Status = XScuGic_CfgInitialize(&Intc, IntcConfig,IntcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return ;
	}

	Xil_ExceptionInit();
	/*
	 * Connect the interrupt controller interrupt handler to the hardware
	 * interrupt handling logic in the ARM processor.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			(Xil_ExceptionHandler) XScuGic_InterruptHandler,
			&Intc);

	/*
	 * Enable interrupts in the ARM
	 */
	Xil_ExceptionEnable();
}

void Interrupt_Handler_Connect(){
	u32 Status;

	// if uartlite used
	#ifdef UART_INTR
		XScuGic_SetPriorityTriggerType(&Intc, UARTLITE_IP2INTC, 0xA0, 0x3);
		Status = XScuGic_Connect(&Intc, UARTLITE_IP2INTC,(Xil_ExceptionHandler)uartlite_rec_handler, NULL);
		if (Status != XST_SUCCESS) {
			return ;
		}
		XScuGic_Enable(&Intc, UARTLITE_IP2INTC);
	#endif

	// if button is used
	#ifdef BUTTON_INTR
		XScuGic_SetPriorityTriggerType(&Intc, BUTTONS_IP2INTC, 0xA0, 0x3);
		Status = XScuGic_Connect(&Intc, BUTTONS_IP2INTC,(Xil_ExceptionHandler)button_press_handler, &butn);
		if (Status != XST_SUCCESS) {
			return ;
		}
		XScuGic_Enable(&Intc, BUTTONS_IP2INTC);
	#endif
}

void UartLite_Initialize(XUartLite * InstancePtr, u16 DeviceId){
	u32 Status;

	// Uart initialization
	Status = XUartLite_Initialize(InstancePtr, UARTLITE_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return;
	}
	XUartLite_EnableInterrupt(InstancePtr);
}

void Button_Initialize(XGpio * InstancePtr, u16 DeviceId){
	u32 Status;
	// Button initialization
	Status = XGpio_Initialize(InstancePtr, DeviceId);
		if (Status != XST_SUCCESS) {
			return ;
		}
	XGpio_InterruptEnable(InstancePtr, 0xFF);
	XGpio_InterruptGlobalEnable(InstancePtr);
}


	void button_press_handler(void *CallbackRef){
		XGpio *GpioPtr = (XGpio *)CallbackRef;
		u32 temp;
			Delayus(100);
			temp = Xil_In32(BUTTON_BASEADDR + XGPIO_DATA_OFFSET);
			switch(temp){
			case 1: //
				// Add BTNU process coding here
				led_off_all();
				led_on(0);
				break;
			case 2:
				// Add BTNR process coding here
				led_off_all();
				led_on(1);
				break;
			case 4:
				// Add BTND process coding here
				led_off_all();
				led_on(2);
				break;
			case 8:
				// Add BTNC process coding here
				led_off_all();
				led_on(3);
				break;
			case 16:
				// Add BTNL process coding here
				led_off_all();
				led_on(4);
				break;
			default:
				break;
			}
		/* Clear the Interrupt */
		XGpio_InterruptClear(GpioPtr, 1);
	}
  
  
int main()
{
	// configure button
	Button_Initialize(&butn, BUTTON_DEVICE_ID);
  
  #ifdef ENABLE_BASIC_INTERRUPT
    //enable interrupt
    Interrupt_Initialize();
    Interrupt_Handler_Connect();
  #endif  
  
  while(1){
		;
	}
}
