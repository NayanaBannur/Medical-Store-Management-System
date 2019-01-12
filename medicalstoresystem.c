#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<string.h>

#define min_stock_value 0 //minimum amount of stock that a particular item can have at which an order is placed
#define lead 0 //amount of time required for the product to arrive

//STRUCTURES

struct date
{
        int day,month,year;
};

struct item
{
        char batch_no[7]; //must be 6 characters
        char manufact_code[11]; //must be 10 characters
        char prod_name[50];
        char company_name[50];
        float price;
        int curr_qty,ordered_qty;
        struct date manufact_date,exp_date,arrival_date;
};

//PROTOTYPES 

void init0(struct item *);
void set_sys_date(struct date *);
void date_init0(struct date *);
int check_date_validity(struct date);
void find_new_date(struct date *,int);
void accept_date(char *,struct date *);
void display_date(char *,struct date);
void display_string_date(struct date);
void check_file_error(FILE *,char *);
void display_details_header();
void accept_details(struct item *);
void display_details(struct item *);
void delete_records(FILE *,char *,struct item);
void add_records(FILE *,char *,struct item);
void read_records(FILE *,char *,struct item);
void check_and_order(FILE *, char *,int,struct date);
void rewrite(FILE *, char *, struct item [], int);
void check_exp(FILE *, char *,struct date);
void update_inventory(FILE *,char *, struct date);
void blank_display();
void generate_bill(FILE *, char *);
int item_scan(FILE *,char *, char *, int);
void modify_records_stock(FILE *, char *, char *, int);
void display_bill(FILE *, char *, char [][50], int [],int);

//INITIALISATION FUNCTIONS

void init0(struct item *i) 
{
	//This function initialises structure members to zero.
	
	strcpy(i->batch_no,"");
	i->curr_qty=i->ordered_qty=0;
	date_init0(&i->manufact_date);
	date_init0(&i->exp_date);
	date_init0(&i->arrival_date);
}

void set_sys_date(struct date *today) 
{
	//This function retrieves the system date and stores it in a structure 'today'.
	
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	today->day=tm.tm_mday;
	today->month=tm.tm_mon+1;
	today->year=tm.tm_year + 1900;
}

void date_init0(struct date *d) 
{
	//This function initialises members of a date structure variable to zero.
	
	d->day=d->month=d->year=0;
}

//DATE FUNCTIONS

int check_date_validity(struct date d) 
{
        //This function checks the validity of a given date.
	
		if(d.year<2000||d.year>3000||d.month<1||d.month>12||d.day<1||d.day>31)
        return 0;
        switch (d.month)
        {
                case 2: 
				if((d.day==29)&&(((d.year%4==0)&&(d.year%100!=0))||(d.year%400==0)))
					return 1;
				if(d.day>28) 
					return 0;
				break;
                case 4: case 6: case 9: case 11: if(d.day>30) return 0; break;
        }
        return 1;
}

void find_new_date(struct date *t,int lead_time)
{
	/* This function is used to find the arrival date of the product.
	It is given the date of ordering and the number of days it will take for the product to arrive. */
	
	int i;
	for(i=1;i<=lead_time;i++)
	{
		(t->day)++;
		if(check_date_validity(*t)==0)
		{
			break;
		}
	}
	if(i!=(lead_time+1))
	{
		t->month++;
		t->day=1;
		if(check_date_validity(*t)==1)
		{
			i++;
		}
		else
		{
			t->month=1;
			t->year++;
			i++;
		}
	}
	for(;i<=lead_time;i++)
	{
		t->day++;
	}
}

void accept_date(char *type,struct date *t)
{
	/* This function accepts a date.
	It calls the function check_date_validity each time a date is accepted. */
	
		char str_date[11];
        int x=0;
        do
        {
                printf("Enter %s date (DD-MM-YYYY).\n",type);
                scanf("%s",str_date);
                sscanf(str_date,"%d-%d-%d",&t->day,&t->month,&t->year);
                x=check_date_validity(*t);
                if(x==0)
                {
					printf("Invalid date!Try again.\n");
					fflush(stdin);
				}
        }while(x==0);
}

void display_date(char *type,struct date t)
{
	//This function displays a given date.
	
	printf("%s date : %d-%d-%d.\n",type,t.day,t.month,t.year);
}

void display_string_date(struct date t)
{
	//This function converts and displays a given date in string format for readability and clear output.
	
	char t1[3],t2[3],t3[5],str_date[11];

	sprintf(t1,"%d",t.day);
	sprintf(t2,"%d",t.month);
	sprintf(t3,"%d",t.year);
	
	strcpy(str_date,"");
	strcat(str_date,t1);
	strcat(str_date,"-");
	strcat(str_date,t2);
	strcat(str_date,"-");
	strcat(str_date,t3);
	str_date[10]='\0';
	printf("%-20s",str_date);
}

//INVENTORY FUNCTIONS

void check_file_error(FILE *fp,char *fname)
{
	/* This function checks for any errors in opening a given file.
	If there are any errors, the program is terminated. */
		
		if(fp==NULL)
        {
                printf("Error in opening %s.\n",fname);
                exit(0);
        }
}

void display_details_header()
{
	//This function displays the headers of the various columns of data displayed.
	
	printf("%-20s%-20s%-20s%-20s%-20s%-20s%-20s%-20s%-20s%-20s\n","Product name","Company name","Manufacturer code","Batch number","Price","Quantity","Ordered Quantity","Manufacture date","Arrival date","Expiry date");
}

void display_details(struct item *i)
{
	//This function displays all the members of a given item.
	
	printf("%-20s%-20s%-20s%-20s%-20f%-20d%-20d",i->prod_name,i->company_name,i->manufact_code,i->batch_no,i->price,i->curr_qty,i->ordered_qty);
	display_string_date(i->manufact_date);
	display_string_date(i->arrival_date);
	display_string_date(i->exp_date);
	printf("\n");
}

void blank_display()
{
	//This function is used to indicate that no items are to be displayed after some operation.
	
	printf("%-20s%-20s%-20s%-20s%-20s%-20s%-20s%-20s%-20s%-20s\n","--","--","--","--","--","--","--","--","--","--");	
}

void accept_details(struct item *i)
{
	/* This function is used to accept certain structure members.
	It calls upon the initialisation functions to initialise the remaining members. */
	
	char ch1,str_len_check;
	
	str_len_check=0;
	init0(i);
	
	printf("Enter:\n");
	
	do
	{
		printf("Manufacturer code (must be 10 characters):");
		fflush(stdin);
		scanf("%s",i->manufact_code);
		if(strlen(i->manufact_code)==10)
        {
			str_len_check=1;
			break;
        }
        printf("Invalid manufacturer code. Try again. \n");
	}while(str_len_check==0);
	printf("Product name:");
	scanf("%s",i->prod_name);
	printf("Company name:");
	scanf("%s",i->company_name);	
	printf("Enter price:");
	scanf("%f",&i->price);
}

void add_records(FILE *fp,char *fname,struct item i)
{
	/*This function is used to add a record to the inventory file.
	It calls upon the accept_details function and writes records to the file as many times as the user wants. */
		
		char ch; 
		int check,first_time;
		struct item newitem;
		ch='y';
		first_time=0;
        
        while(ch=='y'||ch=='Y')
        {
			    check=1;
				accept_details(&newitem);
				fp=fopen(fname,"rb");
				if(fp!=NULL)
				{
					fread(&i,sizeof(i),1,fp);
			        while((!feof(fp))&&(first_time==0))
			        {
							if(strcmp(i.prod_name,newitem.prod_name)==0)
							{
								printf("This item is already in the inventory!\n");
								check=0;
								break;
							}
							fread(&i,sizeof(i),1,fp);
			        }
				}

				fclose(fp);
				if(check==1)
				{
			        fp=fopen(fname,"ab");
			        check_file_error(fp,fname);
	                fwrite(&newitem,sizeof(newitem),1,fp);
					fclose(fp);
				}
		        
                printf("Enter 'y' to continue:");
                scanf(" %c",&ch);
        }
        
}

void read_records(FILE *fp,char *fname,struct item i)
{
	//This function reads records from the inventory file and displays them in a formatted manner.
		
		int loop=0;
		fp=fopen(fname,"rb");
        check_file_error(fp,fname);
		display_details_header();
		fread(&i,sizeof(i),1,fp);
        while(!feof(fp))
        {
			loop++;
			display_details(&i);
			fread(&i,sizeof(i),1,fp);
        }
		if(loop==0)
			blank_display();
			
        fclose(fp);
}

void delete_records(FILE *fp,char *fname,struct item i)
{
    //This function is used to delete a particular item from the inventory file.
	
	int count,loop;
	struct item list[100];
	char p_name[50],choice,ch;
	
	do
	{
		fp=fopen(fname,"rb");
		check_file_error(fp,fname);
		count=loop=0;
		
		printf("Enter name of the product to be deleted.\n");
		fflush(stdin);
		scanf("%s",p_name);
	
		printf("Product being deleted:\n");
	
		display_details_header();
		fseek(fp,0,0);
		fread(&list[count],sizeof(list[count]),1,fp);
	    while(!feof(fp))
	    {
			if(strcmp(p_name,list[count].prod_name)==0)
			{
				display_details(&list[count]);
				loop++;
				printf("Are you sure you want to delete this item? (Y/N)\n");
				scanf(" %c",&choice);
				if(choice=='Y'||choice=='y')
				count--;
			}
			count++;
			fread(&list[count],sizeof(list[count]),1,fp);
	    }
		if(loop==0)
		{
			blank_display();
			printf("Product not found.\n");
		}
		
		printf("Do you want to continue?(Y/N)\n");
		scanf(" %c",&ch);
		
		fclose(fp);
		rewrite(fp,fname,list,count);
		
	}while(ch=='y'||ch=='Y');
	
}

void modify_records_stock(FILE *fp, char *fname, char *p_name, int qty_sold)
{
    //During billing of items, this function is called to decrease the stock in the inventory as and when an item is sold. 
	
	int count,loop;
	struct item list[100];
	char choice;
	
	fp=fopen(fname,"rb");
	check_file_error(fp,fname);
	count=loop=0;
	
	display_details_header();
	fread(&list[count],sizeof(list[count]),1,fp);
	while(!feof(fp))
	{
		if(strcmp(p_name,list[count].prod_name)==0)
		{
			display_details(&list[count]);
			loop++;
			printf("Confirm sale of item. (Y/N) ");
			scanf(" %c",&choice);
			if(choice=='Y'||choice=='y')				
			{
				list[count].curr_qty-=qty_sold;
			}			
		}
		count++;
		fread(&list[count],sizeof(list[count]),1,fp);
	}
	if(loop==0)
	{			
		blank_display();
		printf("Product not found.\n");
	}
		
	fclose(fp);
	rewrite(fp,fname,list,count);
}

void check_and_order(FILE *fp, char *fname,int min_stock,struct date today)
{
    //This function is used to check the stock of all the items, and place orders accordingly.
	
	int count,loop;
	struct item list[100];
	count=loop=0;
	fp=fopen(fname,"rb");
	check_file_error(fp,fname);
	
	printf("List of items to be ordered:\n");
	
	display_details_header();
	fread(&list[count],sizeof(list[count]),1,fp);
    while(!feof(fp))
    {
		if((list[count].curr_qty<=min_stock)&&(list[count].ordered_qty==0))
		{
			loop++;
			list[count].arrival_date=today;
			find_new_date(&list[count].arrival_date,lead);
			list[count].ordered_qty=10;
			display_details(&list[count]);
		}
		count++;
		fread(&list[count],sizeof(list[count]),1,fp);
    }
	if(loop==0)
		blank_display();
	fclose(fp);
	
	rewrite(fp,fname,list,count);
}

void rewrite(FILE *fp, char *fname, struct item list[], int count)
{
    //This function is used to write modified data to the inventory file after an operation is completed.
	
	int loop;
	fp=fopen(fname,"wb");
    check_file_error(fp,fname);
	for(loop=0;loop<count;loop++)
	fwrite(&list[loop],sizeof(list[loop]),1,fp);
    fclose(fp);
}

void check_exp(FILE *fp, char *fname,struct date today)
{
    //This function checks whether any item is at its expiry date, and removes the details of this item.
	
	int count,loop,flag;
	struct item list[100];
	count=loop=flag=0;
	fp=fopen(fname,"rb");
	check_file_error(fp,fname);
	
	printf("List of expired items being removed from the inventory:\n");
	
	display_details_header();
	fread(&list[count],sizeof(list[count]),1,fp);
    while(!feof(fp))
    {	
		if(list[count].exp_date.year<today.year)
		{
			flag=1;
		}
		else if(list[count].exp_date.year==today.year)
		{
			if(list[count].exp_date.month<today.month)
			{
				flag=1;
			}
			else if(list[count].exp_date.month==today.month)
			{
				if(list[count].exp_date.day<=today.day)
				flag=1;
			}
		}
		
		if(flag==1)
		{
			loop++;
			display_details(&list[count]);
			init0(&list[count]);
		}
		
		count++;
		fread(&list[count],sizeof(list[count]),1,fp);
    }
	if(loop==0)
		blank_display();
	fclose(fp);
	
	rewrite(fp,fname,list,count);
}

void update_inventory(FILE *fp,char *fname, struct date today)
{
    /* This item checks the arrival date of the ordered items.
	If the arrival date and the current date match, the details of the item are updated. */
	
	int count,loop,str_len_check=0,exp_check=1,manufact_check=1;
	struct item list[100];
	count=loop=0;
	fp=fopen(fname,"rb");
	check_file_error(fp,fname);
	
	printf("The following items have arrived. Enter the necessary details:\n");
	
	display_details_header();
	fread(&list[count],sizeof(list[count]),1,fp);
    while(!feof(fp))
    {
		if((list[count].arrival_date.day==today.day)&&(list[count].arrival_date.month==today.month)&&(list[count].arrival_date.year==today.year)&&(list[count].curr_qty==0))
		{
			loop++;
			list[count].curr_qty+=list[count].ordered_qty;
			list[count].ordered_qty=0;
			
			printf("%d:\n",loop);
			display_details(&list[count]);
			
			do
			{
				printf("Batch number (must be 6 characters):");
				fflush(stdin);
				scanf("%s",list[count].batch_no);
				if(strlen(list[count].batch_no)==6)
		        {
					str_len_check=1;
					break;
		        }
		        printf("Invalid batch number. Try again. \n");
			}while(str_len_check==0);
			
			

			do
			{
				accept_date("Manufacture",&list[count].manufact_date);
				
				if(list[count].arrival_date.year<list[count].manufact_date.year)
				{
					printf("Invalid manufacture date! Try again. \n");
					manufact_check=0;
				}
				else if(list[count].arrival_date.year==list[count].manufact_date.year)
				{
					if(list[count].arrival_date.month<list[count].manufact_date.month)
					{
						printf("Invalid manufacture date! Try again. \n");
						manufact_check=0;
					}
					else if(list[count].arrival_date.month==list[count].manufact_date.month)
					{
						if(list[count].arrival_date.day<=list[count].manufact_date.day)
						{
							printf("Invalid manufacture date! Try again. \n");
							manufact_check=0;
						}
					}
					else
						manufact_check=1;			
				}
				else
					manufact_check=1;
				
			}while(manufact_check==0);
			
			do
			{
				accept_date("Expiry",&list[count].exp_date);
				
				if(list[count].exp_date.year<list[count].manufact_date.year)
				{
					printf("Invalid expiry date! Try again. \n");
					exp_check=0;
				}
				else if(list[count].exp_date.year==list[count].manufact_date.year)
				{
					if(list[count].exp_date.month<list[count].manufact_date.month)
					{
						printf("Invalid expiry date! Try again. \n");
						exp_check=0;
					}
					else if(list[count].exp_date.month==list[count].manufact_date.month)
					{
						if(list[count].exp_date.day<=list[count].manufact_date.day)
						{
							printf("Invalid expiry date! Try again. \n");
							exp_check=0;
						}
					}
					else
						exp_check=1;			
				}
				else
					exp_check=1;
				
			}while(exp_check==0);
			
		}
		count++;
		fread(&list[count],sizeof(list[count]),1,fp);
    }
	if(loop==0)
		blank_display();
	fclose(fp);
	
	rewrite(fp,fname,list,count);
}

int item_scan(FILE *fp,char *fname, char *p_name, int qty_sold)
{
	/* This function checks two things:
		- whether a given item is present in the inventory or not
		- whether the desired quantity pf the item is present in the inventory or not */
	
	int loop=0;
	struct item temp;
	fp=fopen(fname,"rb");
	check_file_error(fp,fname);
	fseek(fp,0,0);
	fread(&temp,sizeof(temp),1,fp);
    while(!feof(fp))
    {
		if(strcmp(temp.prod_name,p_name)==0)
		{
			loop++;
			if(temp.curr_qty<qty_sold)
			{
				printf("Insufficient stock. Quantity left = %d.\n",temp.curr_qty);
				fclose(fp);
				return 0;
			}
		}
		fread(&temp,sizeof(temp),1,fp);
    }
	if(loop==0)
		{
			printf("Product not found.\n");
			fclose(fp);
			return 0;
		}
	fclose(fp);
	return 1;
}

//BILLING FUNCTIONS

void generate_bill(FILE *fp, char *fname)
{
	/* This function accepts the names and quantities of the items to be billed.
	It calls upon the item_scan and modify_records_stock functions. */
	
	char p_name[50],ch,prod_list[100][50];
	int qty_sold,check,loop,qty_list[100];
	loop=0;
	do
	{
		printf("Enter:\n");
		printf("Product name:");
		fflush(stdin);
		scanf("%s",p_name);
		do
		{
			printf("Quantity:");
			fflush(stdin);
			scanf("%d",&qty_sold);
			if(qty_sold>=0)
			{
				break;
			}
			printf("Quantity cannot be negative! Try again.\n");
		}while(1);
		check=item_scan(fp,fname,p_name,qty_sold);
		if(check==1)
		{
			modify_records_stock(fp,fname,p_name,qty_sold);
			qty_list[loop]=qty_sold;
			strcpy(prod_list[loop],p_name);
			loop++;
		}
		printf("Do you want to continue?(Y/N) ");
		scanf(" %c",&ch);
	}while(ch=='y'||ch=='Y');
	if(loop>0)
	display_bill(fp,fname,prod_list,qty_list,loop);
}

void display_bill(FILE *fp, char *fname, char prod_list[][50], int qty_list[],int loop)
{
	//This function displays the bill.
	
	struct item temp;
	float tot=0;
	int j,count;count=0;
	fp=fopen(fname,"rb");
	check_file_error(fp,fname);
	fread(&temp,sizeof(temp),1,fp);
	printf("\nBILL\n");
	printf("%-5s%-20s%-20s%-20s%-20s\n","No.","Name","Quantity","Rate","Amount");
    while(!feof(fp))
    {
		for(j=0;j<loop;j++)
		if(strcmp(temp.prod_name,prod_list[j])==0)
		{
			count++;
			printf("%-5d%-20s%-20d%-20f%-20f\n",count,temp.prod_name,qty_list[j],temp.price,qty_list[j]*temp.price);
			tot+=qty_list[j]*temp.price;
		}
		fread(&temp,sizeof(temp),1,fp);
    }
	fclose(fp);
	printf("\nTotal = Rs.%f\n",tot);
}

int main()
{
        struct item i;
		struct date d1,d2,today;
        FILE *fp;
        int main_menu_choice, inventory_menu_choice, billing_menu_choice;
		char fname[20]="inventory", ch1,ch2;

		set_sys_date(&today);
		
		do
		{
			//This is the main menu of the program.
			
			printf("\nMAIN MENU\n");
			printf("1. Inventory options\n2. Billing\n3. Exit\n");
			scanf("%d",&main_menu_choice);
		
			switch(main_menu_choice)
			{
		        case 1:
			
				do
				{
					//This part of the programs has options for various inventory management related operations.
					
					printf("\n");
					printf("INVENTORY MENU\n");
					printf("1. Add new items\n2. Print inventory\n3. Check stock and place order\n4. Update inventory\n5. Remove items beyond expiry date\n6. Delete items\n7. Main menu\n");
					scanf("%d",&inventory_menu_choice);
					
					switch(inventory_menu_choice)
					{
						case 1:
						add_records(fp,fname,i);
						break;
					
						case 2:
						printf("INVENTORY\n");
						read_records(fp,fname,i);
						break;
					
						case 3:
						check_and_order(fp,fname,min_stock_value,today);
						break;
						
						case 4:
						update_inventory(fp,fname,today);
						break;
						
						case 5:
						check_exp(fp,fname,today);
						break;
						
						case 6:
						delete_records(fp,fname,i);
						break;
						
						case 7:
						ch1='e';
						break;
						
						default:
						printf("Wrong choice.\n");
						break;
						
					}
					
				}while(ch1!='e');
				break;
				
				case 2:
				
				do
				{
					//This part of the programs manages the billing of items.
					
					printf("\n");
					printf("BILLING MENU\n");
					printf("Enter:\n1. Bill items\n2. Main menu\n");
					scanf("%d",&billing_menu_choice);
				
					switch(billing_menu_choice)
					{
						case 1:
						generate_bill(fp,fname);
						break;
					
						case 2:
						ch2='e';
						break;
					
						default:
						printf("Wrong choice.\n");
						break;
					}					
				}while(ch2!='e');
				break;
				
				case 3:
				exit(0);
				break;
			
				default:
				printf("Wrong choice.\n");
			};
			
		}while(1);

        return 0;
}
