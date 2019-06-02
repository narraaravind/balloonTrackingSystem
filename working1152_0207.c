#include <stdio.h>
#include <dos.h>
#include <conio.h>
#include<string.h>

// #define PORT1 0x3F8  /* Port Address Goes Here Statically */
		     // if you are changing port no be sure to check interrupt nos too
#define INTVECT 0x0C /* Com Port's IRQ here (Must also change PIC setting) */

  /* Defines Serial Ports Base Address */
  /* COM1 0x3F8                        */
  /* COM2 0x2F8			       */
  /* COM3 0x3E8			       */
  /* COM4 0x2E8			       */

int temp1=0,temp3=0,temp=0,tempo2=0,bufferin = 0,replace,replace1,var1=0,modcheck_counter=3;
long int PORT1;
char send[160],SMSTEXT[160],ch;
char command;
FILE *fp;
FILE *filecontacts;
FILE *filegprmc;
char google[160]="https://www.google.co.in/maps/@17.48147,78.583721,15z";
char gprmc[160]="$GPRMC,104802.50,A,5957.03641,N,01037.73641,E,4.336,246.70,080306,,,A*60";
char ok[3]="OK";
char contact[30]="AT+CMGS=\"+911234567890\"";
char contact2[30]="AT+CMGS=\"+918464920999\"";
char contact3[30]="AT+CMGS=\"+918464920999\"";
char commandString[160];
char *checkok;
int charCounter,charCounter2;
char check[10]="AT";
unsigned char buffer[100];
int ackLength=0;
char contact_array[20][20];
int no_of_contacts=0;
char cont1[20],cont2[20],cont3[20];
int tempo=0,temp4=0;

void interrupt (*oldport1isr)();
void initCOMport(void);
void sendsms(void);
void googlelink(void);
int contactconfirmation(void);
void nmea(void);

void interrupt PORT1INT()  /* Interrupt Service Routine (ISR) for PORT1 */
{
 int c;
 do {
 c = inportb(PORT1 + 5);
      if (c & 1) {
		  buffer[bufferin] = inportb(PORT1);
		  bufferin++;
		  //ackLength=bufferin;

		  }
   }while (c & 1);
 outportb(0x20,0x20);
}

void restoreISR(void)
{

outportb(PORT1 + 1 , 0);       /* Turn off interrupts - Port1 */
outportb(0x21,(inportb(0x21) | 0x10));  /* MASK IRQ using PIC */
		 /* COM1 (IRQ4) - 0x10  */
		 /* COM2 (IRQ3) - 0x08  */
		 /* COM3 (IRQ4) - 0x10  */
		 /* COM4 (IRQ3) - 0x08  */
setvect(INTVECT, oldport1isr); /* Restore old interrupt vector */
}

void clearbuffer(void)
{
	memset(buffer,NULL,sizeof(buffer));
    ackLength=0;
    bufferin=0;

}

void changenumbers(void)
{
		char command1;
		char read[100];
		
		printf("\nEnter the index of the contact which u want to change:");
		scanf("%d",&temp4);
		
retype:	printf("\nThe contact which u want to change is:%s\nPress 'y' to continue change or 'n' to go back\nEnter your choice:",contact_array[temp4-1]);
		scanf("%s",&command1);
		if(command1!='y' && command1!='Y' && command1!='n' && command1!='N' )
		{
			printf("\n\nSorry wrong choice try again");
			command1=NULL;
			goto retype;
		}
		
		if(command1=='n' || command1=='N')
		{
			return;
		}
			
reenter:printf("\nEnter the 10 digit mobile number +91-");
       	scanf("%s",&read);
		//printf("The temp4 after is :%d",temp4);
		
		//printf("The string to be replaced is :%d",temp4-1);
		strcpy(contact_array[temp4-1],read);
		if(strlen(contact_array[temp4-1])!=10)
		{
		printf("\nSorry not a valid number\n");
		goto reenter;	
		}
	
			
}



void intializingcontact(void)				//READING THE RECEPIENT CONTACT NUMBER AND OVERWRITING THE CONTACT STRING
{
	
		int temp=0,delete=0;
		char end_of_file='r';
		
		printf("Existing contacts %d are:\n",no_of_contacts);
		
	
		filecontacts=fopen("contacts.txt","r");	
		for(temp=0;end_of_file!=NULL;temp++)
		{
			fscanf(filecontacts,"%s",&contact_array[temp]);
			end_of_file=contact_array[temp][0];
			if(end_of_file==NULL)
				break;
		//	printf("\n%d:%s",temp+1,contact_array[temp]);
		}
		fclose(filecontacts);
		//printf("temp=%d",temp);
		no_of_contacts=temp;
changes:
		clrscr();
		printf("\n\t\t     ****GSM Modem interfacing program****\n\n\n");
		printf("\nNew contact list is:\n");
		for(temp=0;temp<no_of_contacts;temp++)
		{
		printf("%d:%s\n",temp+1,contact_array[temp]);
		}		
		printf("\nPress 'y' to continue with these numbers\nPress 'a' to add  new number(s)\nPress 'd' to delete a number(s)\nPress 'm' modify number(s)\n\nEnter your choice:");
		scanf("%s",&command);
		if(command!='y' && command!='Y' && command!='a' && command!='A' && command!='m' && command!='M' && command!='d' && command!='D' )
		{
			printf("\n\nSorry wrong choice try again");
			command=NULL;
			goto changes;
		}
		if(command=='a' || command=='A')
		{
			command=NULL;
rereada:	printf("\nEnter the 10 digit mobile number +91-");
			scanf("%s",&contact_array[temp]);
			no_of_contacts=temp+1;
			//printf("NO of conts %d",no_of_contacts);
			if(strlen(contact_array[temp])!=10)
			{
			printf("\nSorry not a valid number\n");
			goto rereada;	
			}
		
			goto changes;
	
		}
		else if(command=='m' || command=='M')
		{
			temp4=0;
rereadm:	printf("\nEnter the index of the contact which u wish to Modify:");
			scanf("%d",&temp4);
			if(temp4>no_of_contacts)
			{
				printf("\nWrong choice TRY AGAIN");
				goto rereadm;
			}
			printf("\nEnter the 10 digit mobile number +91-");
			scanf("%s",&contact_array[temp4-1]);
			goto changes;
		
		}
		else if(command=='d' || command=='D')
		{
			temp4=0;
rereadd:	printf("\nEnter the index of the contact which u wish to modify:");
			scanf("%d",&temp4);
			if(temp4>no_of_contacts)
			{
				printf("\nWrong choice TRY AGAIN");
				goto rereadd;
			}
			for(delete=temp4-1;delete<no_of_contacts;delete++)
			{
				strcpy(contact_array[delete],contact_array[delete+1]);
			}
			no_of_contacts--;
			goto changes;
				
		}
		else if(command=='y' || command=='Y')
		{
			fclose(filecontacts);
			filecontacts=fopen("contacts.txt","w");
			for(temp=0;temp<no_of_contacts;temp++)
			{
			fprintf(filecontacts,"%s\n",contact_array[temp]);
			}
			fclose(filecontacts);
			
		}

		
}

void initCOMport(void)
{

	int COM;

	printf("Select the COM port\n1:COM1\n2:COM2\n3:COM3\n4:COM4\nEnter the selection:");
	scanf("%d",&COM);

	switch(COM)
	{
		case 1:PORT1=0x3f8;break;
		case 2:PORT1=0x2f8;break;
		case 3:PORT1=0x3e8;break;
		case 4:PORT1=0x2e8;break;
		default:printf("Selected COM port does not exist");
	}


	outportb(PORT1 + 1 , 0);        /* Turn off interrupts - Port1 */
	
	oldport1isr = getvect(INTVECT); /* Save old Interrupt Vector of later
						recovery */
	
	setvect(INTVECT, PORT1INT);     /* Set Interrupt Vector Entry */
					/* COM1 - 0x0C */
					/* COM2 - 0x0B */
					/* COM3 - 0x0C */
					/* COM4 - 0x0B */
	
	/*         PORT 1 - Communication Settings         */
	
	outportb(PORT1 + 3 , 0x80);  /* SET DLAB ON */
	outportb(PORT1 + 0 , 0x0C);  /* Set Baud rate - Divisor Latch Low Byte */
					/* Default 0x03 =  38,400 BPS */
					/*         0x01 = 115,200 BPS */
					/*         0x02 =  57,600 BPS */
					/*         0x06 =  19,200 BPS */
					/*         0x0C =   9,600 BPS */
					/*         0x18 =   4,800 BPS */
					/*         0x30 =   2,400 BPS */
	outportb(PORT1 + 1 , 0x00);  /* Set Baud rate - Divisor Latch High Byte */
	outportb(PORT1 + 3 , 0x03);  /* 8 Bits, No Parity, 1 Stop Bit */
	outportb(PORT1 + 2 , 0xC7);  /* FIFO Control Register */
	outportb(PORT1 + 4 , 0x0B);  /* Turn on DTR, RTS, and OUT2 */
	
	outportb(0x21,(inportb(0x21) & 0xEF));  /* Set Programmable Interrupt Controller */
						/* COM1 (IRQ4) - 0xEF  */
						/* COM2 (IRQ3) - 0xF7  */
						/* COM3 (IRQ4) - 0xEF  */
						/* COM4 (IRQ3) - 0xF7  */
	
	outportb(PORT1 + 1 , 0x01);  /* Interrupt when data received */
	
	
}


void googlelink(void)
{
	memset(commandString,NULL,sizeof(commandString));
	memset(buffer,NULL,sizeof(buffer));
	memset(send,NULL,sizeof(send));

	strcpy(commandString,google);
	fp=fopen("gps.txt","r");

				while(1)
				{
				fseek(fp,-(temp1+1),SEEK_END);

				temp3=fgetc(fp);
					if(temp3=='\n')
					break;
				send[temp1]=temp3;
				temp1++;
				}
				strrev(send);


		/*lattitude replacement*/
				replace1=0;
				for(replace=31;replace<=38;replace++)
				{
				commandString[replace]=send[replace1];
				replace1++;
				}

		/*longitude replacement*/
				replace1=9;
				for(replace=40;replace<=48;replace++)
				{
				commandString[replace]=send[replace1];
				replace1++;
				}
		/* sending location to module*/

				for(temp=0;temp<=strlen(commandString);temp++)
				{
				outportb(PORT1, commandString[temp]);
				}
			memset(commandString,NULL,sizeof(commandString));
			temp1=0;
			printf("\n");
			//sleep(2);
		fclose(fp);


}

void nmea(void)
{
	temp1=0;
	
	memset(commandString,NULL,sizeof(commandString));
	memset(buffer,NULL,sizeof(buffer));
	memset(send,NULL,sizeof(send));

	
	filegprmc=fopen("gprmc.txt","r");

				while(1)
				{
				fseek(filegprmc,-(temp1+3),SEEK_END);

				temp3=fgetc(filegprmc);
					if(temp3=='\n')
					break;
				send[temp1]=temp3;
				temp1++;
				}
				strrev(send);
			printf("send%s",send);
			strcpy(gprmc,send);
			strcpy(commandString,gprmc);
			for(temp=0;temp<=strlen(commandString);temp++)
				{
				outportb(PORT1, commandString[temp]);
				}
			memset(commandString,NULL,sizeof(commandString));
			temp1=0;
			printf("\n");
	
	fclose(filegprmc);
	
}

void fullControl(void)
{

    memset(commandString,NULL,sizeof(commandString));   //make string NULL to hold new data
    clearbuffer();
	printf("Enter GSM command :");
    scanf("%s",commandString);
    for(charCounter=0;charCounter<sizeof(commandString);charCounter++){
	outportb(PORT1,commandString[charCounter]);
	}
	outportb(PORT1,'\r');   // command terminating with \r
	clearbuffer();
	sleep(1);

    printf("\n after at reply is:%s",buffer);
	memset(commandString,NULL,sizeof(commandString));   //make string NULL to hold new data
    printf("\nEnter SMS Text\n");
    scanf("%s",commandString);
    for(charCounter=0;charCounter<sizeof(commandString);charCounter++){
	outportb(PORT1,commandString[charCounter]);
	}
    outportb(PORT1,26);

    printf("\nackLength=%d\n\nReceived:",ackLength);
    for(charCounter2=0;charCounter2<ackLength;charCounter2++){
	printf("%c",buffer[charCounter2]);
    }
    printf("\n");

}

int checkmodule(void)
{
	int charCounter;
	memset(commandString,NULL,sizeof(commandString));   //make string NULL to hold new data
    clearbuffer();
    strcpy(commandString,check);
    for(charCounter=0;commandString[charCounter]!=NULL;charCounter++)
	{
		outportb(PORT1,commandString[charCounter]);
    }
    outportb(PORT1,'\r');
	clearbuffer();

	sleep(2);
//	printf("\nThe lenth of input string is:%d",strlen(buffer));
//	 printf("\nThe lenth of input string is:%s",buffer);
	checkok=strstr(buffer,ok);
	if(checkok!=NULL)
	{
		printf("\n\nModule is good to go(OK Received for AT)\n\n");
		return 1;
	}
	else
	{

		return 0;
	}
}


void sendsms(void)
{
	int var;
	var=no_of_contacts;
	var1=0;
back:memset(commandString,NULL,sizeof(commandString));   //make string NULL to hold new data
    clearbuffer();
	tempo2=0;
	for(tempo=12;tempo<=21;tempo++,tempo2++)
	{
		contact[tempo]=contact_array[var1][tempo2];
	}
	strcpy(commandString,contact);
	printf("\nSending SMS to %s!!!\n",contact_array[var1]);
//	for(charCounter=0;charCounter<sizeof(commandString);charCounter++)
	for(charCounter=0;commandString[charCounter]!=NULL;charCounter++)
	{
	outportb(PORT1,commandString[charCounter]);
    }
    outportb(PORT1,'\r');
	clearbuffer();
	sleep(2);
   // googlelink();
	nmea();
    outportb(PORT1,26);
    outportb(PORT1,'\r');
	clearbuffer();
	var--;
	var1=var1+1;
	sleep(5);
	if(var>0)
		goto back;
}


int main(void)
{
	int keyin,checkmod=5,_delay=2;
	char esc;
	unsigned char commandString[160];
	clrscr();
	printf("\n\t\t     ****GSM Modem interfacing program****\n\n\n");
	initCOMport();
	intializingcontact();
	printf("\nEnter the time delay to send New Position(In seconds):");
	scanf("%d",&_delay);
   	checkmod=checkmodule();
	printf("\nPress ESCAPE to exit\n");
	if(checkmod)
	{
		do
		{
		printf("\nSending SMS !!!\n");
		sendsms();
		//getch();
		esc=kbhit();
		if(modcheck_counter%2==0)
		{
			checkmod=checkmodule();
			if(checkmod==0)
				goto error;
		}
		printf("Waiting for %d seconds to send Next SMS",_delay);
		sleep(_delay);
		//fullControl();
		modcheck_counter++;
		}while(esc!=27);
	}
	else
	{
error:	printf("Error with initializing module(AT Command)");
		getch();
	}
	restoreISR();

	return 0;
}