#define _CRT_SECURE_NO_WARNINGS
#define ERROR 1
#define PERFECT 0
#define MAX_CLIENT_ID_SIZE 30 + 1
#define MAX_NAME_SIZE 20 + 1
#define CLIENT_PW_SIZE 20 + 1
#define DAYSEC (24*60*60)

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <string.h>
#include <direct.h>

typedef struct account {
	char accountNo[12];
	unsigned int money;
	char password[5];
} Account;

typedef struct user {
	char name[MAX_NAME_SIZE];
	char user_id[MAX_CLIENT_ID_SIZE];
	short year;
	short month;
	short day;
	char password[CLIENT_PW_SIZE];
	int numOfAcc;
	Account acc[3];
} User;

User *login_client = NULL;	//	����ڰ� �α����� �ϸ� �����޸��Ҵ��Ͽ� �� ������ �ּҰ� ����
							//User userArray = NULL; // ����� �迭�� ����ų ������ ����


int id_overlapcheck(char * id);		//Id �ߺ�üũ**************************************
void user_add(User * client);	//ȸ������ �߰� �� ������Ʈ
int excep_birth(int year, int mon, int day);		//������� ����ó��
int excep_hangul(char * a);	//�̸� ����ó��
int excep_pwd(char *a);		//ȸ�� ��й�ȣ ����ó��
int excep_id(char * src_id);			//���̵� ����ó��
int excep_account_pwd(char * pwOfacc);  //���º�й�ȣ ����ó��
int create_account(User* client);	//���»���
void memberlist();
void transfer_process(char * a);	//��ü���μ��� *
void deposit();				//�Ա� *
void transfer();			//��ü�ʱ�ȭ��
int loging_password(char * a);	//ȸ����й�ȣ �Է�
void establish_account();		//���°���
void withdrawal();		//��� *
void mainscreen();		//���� ȭ��
void join_process();	    //���డ��
int DB_modify();	//DB ����,������Ʈ
void inquiry();		//��ȸ *
void login_screen();
int login_system(char* id, char* pw);	// ���̵�� ����� ����� �ּҸ� �Ѱ��ָ� ���������� �����Ͽ� �α���


char input[4096];
char input2[4096];
char buf[50];
User super_client = { 0 };			//DB_modify�ÿ� �� ����ü�� ���� DB�� ������
int clientNum;	// DB_modify���� db ������ �Ҷ� ���° �����͸� ���������� ����

int join_processCount; // �� ���� Ƚ��
int i;

char *commify(double val, char *buf, int round) { /*���¼ҽ� Ȱ��*/
	static char *result;
	char *nmr;
	int dp, sign;


	result = buf;

	if (round < 0)                        /*  Be sure round-off is positive  */
		round = -round;

	nmr = _fcvt(val, round, &dp, &sign);   /*  Convert number to a string     */

	if (sign)                             /*  Prefix minus sign if negative  */
		*buf++ = '-';

	if (dp <= 0) {                         /*  Check if number is less than 1 */
		if (dp < -round)                    /*  Set dp to max(dp, -round)      */
			dp = -round;
		*buf++ = '0';                       /*  Prefix with "0."               */
		*buf++ = '.';
		while (dp++)                        /*  Write zeros following decimal  */
			*buf++ = '0';                     /*     point                       */
	}
	else {                                /*  Number is >= 1, commify it     */
		while (dp--) {
			*buf++ = *nmr++;
			if (dp % 3 == 0)
				*buf++ = dp ? ',' : '.';
		}
	}

	strcpy(buf, nmr);                     /*  Append rest of digits         */
	return result;                        /*  following dec pt              */
}


int loging_password(char * a)
{
	char pwd[CLIENT_PW_SIZE];
	int ch;
	for (int i = 0; i < CLIENT_PW_SIZE; i++) {
		ch = getch();

		if (ch == '\b')
		{
			if (i <= 0)
			{
				i--;
				continue;
			}
			else
				i -= 2;
			putchar('\b');
			putchar(' ');
			putchar('\b');
		}
		else if ((ch != '\r') && i >= CLIENT_PW_SIZE-1)
		{
			i--;
			continue;
		}
		else if (ch == '\r')
		{
			pwd[i] = '\0';
			strcpy_s(a, sizeof(pwd), pwd);
			return PERFECT;
		}
		else if (ch >= 33 && ch <= 126) {	//�����Է�
			pwd[i] = ch;
			putchar('*');
		}
		else {
			i--;
			continue;
		}
	}
	return ERROR;
}




void transfer_process(char * a)
{
	int fsize , accNum;
	char tmp;
	char pwd[5];
	int clientNum_des = 0;	// ��ü ���� ����ڰ� ���° ����Ʈ�� ������ִ��� ������ ����
	int clientNum_tmp = clientNum;
	User client;
	User tmp_client;
	FILE * fp;
	fopen_s(&fp, "client.txt", "r");
	fseek(fp, 0, SEEK_END);
	fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	for (int i = 0; i < (fsize / sizeof(User)); i++) {
		fread((void*)&client, sizeof(User), 1, fp);		//ȸ������ ���������� �ҷ�����
		for (int j = 0; j < client.numOfAcc; j++)	// �� ȸ���� ���¼���ŭ �ݺ��ϱ�
		{
			if (!strcmp(client.acc[j].accountNo, a))	// ���� �Ҿ�� ���¿� ����ڰ� �Է��� ���¿� ���ٸ�
			{
				fclose(fp);
				while (1)
				{

					printf("%s �� �� �½��ϱ�?(y/n)\n", client.name);
					tmp = getch();
					if (tmp == 'y')
					{

						while(1)
						{
							system("cls");
							puts("[��ü����]");
							puts("[�ȳ� 1]���� ���¸� ������ �ֽ� ��, �ش� ������ ��й�ȣ �Է� ��");
							puts("[�ȳ� 2]���� �ݾ��� �Է��� �ֽʽÿ�.");
							puts("[�ȳ� 3]100���� ���Ϸ� ���ѵǾ��ֽ��ϴ�.");
							puts("[�ȳ� 4]��Ҹ� ������ ��� ����ȭ������ �̵��մϴ�.");
							for (int k = 0; k < super_client.numOfAcc; k++)
							{
								printf("%d.%s ���� ����\n", (k + 1), super_client.acc[k].accountNo);
							}
							printf("%d.���\n", (super_client.numOfAcc + 1));
							printf("�Է� : ");
							scanf_s("%d", &i);
							rewind(stdin);

							if (i == super_client.numOfAcc + 1)	// �ּ��ϰ��
							{
								return;
							}
							else    // Ư�����¸� �����Ͽ��� ���
							{
								system("cls");
								puts("[��ü����]");
								puts("[�ȳ� 1]���� ���¸� ������ �ֽ� ��, �ش� ������ ��й�ȣ �Է� ��");
								puts("[�ȳ� 2]���� �ݾ��� �Է��� �ֽʽÿ�.");
								puts("[�ȳ� 3]100���� ���Ϸ� ���ѵǾ��ֽ��ϴ�.");
								puts("[�ȳ� 4]��Ҹ� ������ ��� ����ȭ������ �̵��մϴ�.");
								printf("%s ������ ��й�ȣ �Է� : " , super_client.acc[i-1].accountNo);
								excep_account_pwd(pwd);
								if (!strcmp(pwd, super_client.acc[i - 1].password))
								{
									accNum = i - 1;
									system("cls");
									puts("[��ü����]");
									puts("[�ȳ� 1]���� ���¸� ������ �ֽ� ��, �ش� ������ ��й�ȣ �Է� ��");
									puts("[�ȳ� 2]���� �ݾ��� �Է��� �ֽʽÿ�.");
									puts("[�ȳ� 3]100���� ���Ϸ� ���ѵǾ��ֽ��ϴ�.");
									puts("[�ȳ� 4]-1�� �Է��ϸ� ����ȭ������ �̵��˴ϴ�.");
									commify(super_client.acc[accNum].money, buf, 0);
									buf[strlen(buf) - 1] = '\0';
									printf("[%s]������ �ܾ� [ %s ��]\n", super_client.acc[accNum].accountNo,
										buf);
									printf("���� �ݾ� : ");
									scanf_s("%d", &i);
									rewind(stdin);
									if (i >= 0 && i <= 1000000)
									{
										if (i <= super_client.acc[accNum].money)	// �ܾ׿� �ʰ������ʴ� �ݾ��� �Է��������
										{
											
											if (!strcmp(client.acc[j].accountNo, super_client.acc[accNum].accountNo)) // �������·� ��ü�� ���
											{

												puts("\a���� ���·� ��ü�ϴ� ���� �Ұ����մϴ�.");
												puts("[����]�� ������ ����ȭ������ �̵��մϴ�.");

											}
											else if (!strcmp(client.user_id, super_client.user_id))	//�ڽ��� ���·� ��ü�� ���
											{
												super_client.acc[j].money += i;
												super_client.acc[accNum].money -= i;
												DB_modify();
												puts("===[��ü�Ϸ�]===");

											}
											else    // �ٸ� ȸ���� ���·� ������ü�� �� ���
											{

												client.acc[j].money += i;
												super_client.acc[accNum].money -= i;
												tmp_client = super_client;

												clientNum = clientNum_des;
												super_client = client;
												DB_modify();

												clientNum = clientNum_tmp;
												super_client = tmp_client;
												DB_modify();
												puts("===[��ü�Ϸ�]===");

											}
											getchar();
											rewind(stdin);
											return;
										}
										else //�ܾ׺��� ���� �ݾ��� �Է��������
										{
											puts("\a�ܾ��� �ʰ��Ͽ� �Է��ϼ̽��ϴ�.");
											puts("��� ������ ���Ͻø� [����]�� �����ּ���");
											getchar();
											rewind(stdin);
											continue;
										}
									}
									else if (i == -1)
									{
										return;
									}
									else
									{
										puts("\a1���̻� 100���� ���Ϸ� �Է����ֽʽÿ�.");
										puts("��� ������ ���Ͻø� [����]�� �����ּ���");
										getchar();
										rewind(stdin);
										continue;
									}
								}
								else
								{
									puts("\n\a��й�ȣ�� ��ġ���� �ʽ��ϴ�.");
									getchar();
									rewind(stdin);
									continue;
								}
								
							}
						}
					
					}
					else if (tmp == 'n')
					{
						return;
					}
					else
					{
						puts("\a�ùٸ� ���� �Է����ֽʽÿ�.");
						puts("��� ������ ���Ͻø� [����]�� �����ּ���");
						getchar();
						rewind(stdin);
						continue;
					}
				}
				
			}
		}
		clientNum_des++;
		//printf("�񱳰� : %s\n�Է°�:%s\n\n", client.user_id, id);
	}
	puts("�������� �ʴ� ���¹�ȣ�Դϴ�.\n[����]�� �Է��ϸ� ����ȭ������ �̵��մϴ�.");
	getchar();
	rewind(stdin);
	fclose(fp);
	return PERFECT;
}
void transfer()         //��ü
{
	char accNum[12];
	system("cls");
	puts("[��ü����]");
	printf("���� ���� �Է�(-����) : ");
	scanf_s("%s", accNum);
	transfer_process(accNum);


}

void establish_account()
{
	char temp;
	char pwd[5];
	char re_pwd[5];
	while (1)
	{
		system("cls");
		puts("[���°�������]");
		puts("���� ���¸� �����Ͻðڽ��ϱ�?(y,n) ");
		temp = getch();
		rewind(stdin);
		if (temp == 'y')
		{
			if (super_client.numOfAcc >= 3)
			{
				printf("\a");
				puts("���¸� 4���̻� ����� �����ϴ�.");
				puts("[����]�� ������ ����ȭ������ �̵��մϴ�.");
				getchar();
				rewind(stdin);
				return;
			}
			else
			{
				
				system("cls");
				puts("[���°�������]");
				puts("[�ȳ� 1]���¸� �����ϱ� ���ؼ� ��й�ȣ�� �ʿ��մϴ�. ��й�ȣ�� �Է����ֽʽÿ�.");
				puts("[�ȳ� 2]�ڷΰ��� ���Ͻ� ��� ��й�ȣ���Է��� Ʋ���� �Է��Ͻø� �˴ϴ�.");
				printf("��й�ȣ �Է�(4�ڸ�) : ");	excep_account_pwd(pwd);
				printf("\n��й�ȣ ���Է� : ");	 excep_account_pwd(re_pwd);
				rewind(stdin);
				if (!strcmp(pwd, re_pwd))
				{
					create_account(&super_client);
					strcpy(super_client.acc[super_client.numOfAcc-1].password,pwd);
					DB_modify();
					system("cls");
					puts("[���°�������]");
					puts("���°����� �Ϸ�Ǿ����ϴ�.");
					printf("���¹�ȣ : %s\n", super_client.acc[super_client.numOfAcc - 1].accountNo);
					puts("[����]�� ������ ����ȭ������ �̵��մϴ�.");
					getchar();
					rewind(stdin);
					return;
				}
				else
				{
					system("cls");
					printf("\a");
					puts("[���°�������]");
					puts("��й�ȣ�� ��ġ�����ʽ��ϴ�.");
					puts("[����]�� ������ �ʱ�ȭ�����ΰ��ϴ�.");
					getchar();
					rewind(stdin);
					continue;
				}
			}
		}
		else if (temp == 'n')
		{
			return;
		}
		else
		{
			printf("\a");
			puts("�ùٸ����� �Է����ֽʽÿ�.");
			puts("��� �����ϱ� ���ؼ� [����]�� �Է��Ͻʽÿ�.");
			getchar();
			rewind(stdin);
			continue;
		}
	}
	

}
void DropOut()			//����Ż��
{}
void withdrawal()	    //���
{
	int money;
	while (1)
	{
		system("cls");
		puts("[��ݼ���]");
		for (int j = 0; j < super_client.numOfAcc; j++)
		{
			printf("%d.%s ���� ����\n", (j + 1), super_client.acc[j].accountNo);
		}
		printf("%d.�ڷΰ���\n", (super_client.numOfAcc + 1));
		printf("�Է� : ");
		scanf_s("%d", &i);
		rewind(stdin);

		if (i >= 1 && i <= (super_client.numOfAcc + 1))	// ��ȿ�� ���� ���
		{
			if (i == (super_client.numOfAcc + 1))	// �ڷΰ����ϰ��
			{
				system("cls");
				return;
			}
			else             // Ư�� ���¸� �������� ���; i-1�� ������ ���¸� ����
			{
				char pwd[5];
				system("cls");
				puts("[��ݼ���]");
				printf("�ش� ������ ��й�ȣ �Է� : ");
				excep_account_pwd(pwd);
				if (!strcmp(pwd, super_client.acc[i - 1].password))
				{
					commify(super_client.acc[i - 1].money, buf, 0);
					buf[strlen(buf) - 1] = '\0';
					while (1)
					{
						
						system("cls");
						puts("[��ݼ���]");
						puts("(����: 100���� ����, ��� : -1�Է�)");
						printf("�ܾ� [ %s ��] \n\n", buf);
						printf("����� �ݾ� : ");
						scanf_s("%d", &money);
						rewind(stdin);
						if (money >= 0 && money <= 1000000)
						{
							if (super_client.acc[i - 1].money >= money)
							{
								super_client.acc[i - 1].money -= money;
								if (DB_modify() == PERFECT)
								{
									commify(super_client.acc[i - 1].money, buf, 0);
									buf[strlen(buf) - 1] = '\0';
									system("cls");
									puts("[��ݿϷ�]");
									printf("���� �ܾ� [ %s �� ]\n", buf);
									puts("�ٽ� �޴��� �����ϱ� ���ؼ� [����]�� �Է��Ͻʽÿ�.");
									getchar();
									rewind(stdin);
								}
								else
								{
									printf("\a");
									system("cls");
									super_client.acc[i - 1].money += money;
									puts("[��ݽ���]");
									puts("�ٽ� �޴��� �����ϱ� ���ؼ� [����]�� �Է��Ͻʽÿ�.");
									getchar();
									rewind(stdin);
								}
								break;
							}
							else
							{
								printf("\a");
								puts("[�ܾ׺��� ���� ��ݾ��� �Է��Ͽ����ϴ�.]");
								puts("�ٽ� �޴��� �����ϱ� ���ؼ� [����]�� �Է��Ͻʽÿ�.");
								getchar();
								rewind(stdin);
							}
							//�Ա�����
						}
						else if (money == -1)
						{
							//�Ա����
							puts("[������]");
							puts("�ٽ� �޴��� �����ϱ� ���ؼ� [����]�� �Է��Ͻʽÿ�.");
							getchar();
							rewind(stdin);
							break;
						}
						else
						{
							printf("\a");
							puts("�ùٸ� ���� �Է��� �ֽʽÿ�.");
							puts("�ٽ� �޴��� �����ϱ� ���ؼ� [����]�� �Է��Ͻʽÿ�.");
							getchar();
							rewind(stdin);
						}
					}
				}
				else
				{
					printf("\a");
					puts("\n��й�ȣ�� ��ġ�����ʽ��ϴ�.");
					puts("�ٽ� �޴��� �����ϱ� ���ؼ� [����]�� �Է��Ͻʽÿ�.");
					getchar();
					rewind(stdin);
				}

				rewind(stdin);
				continue;
			}

		}
		else
		{
			printf("\a");
			puts("�ùٸ� ���� �Է��� �ֽʽÿ�. [����]�� �Է��Ͻʽÿ�.");
			getchar();
			rewind(stdin);
		}
	}
}


int DB_modify()
{
	int readCnt, cur = 0 ,err;
	FILE *tmp, *src;
	User user;
	fopen_s(&src, "client.txt", "r");
	fopen_s(&tmp, "client.tmp", "a");
	while (1)
	{
		
		readCnt = fread((void*)&user, 1, sizeof(User), src);

		if (readCnt < sizeof(User))
		{
			if (feof(src) != 0)		// DB ���� �Ϸ�
			{
				fwrite((void*)&user, 1, readCnt, tmp);
				fclose(tmp);
				fclose(src);
				err = remove("client.txt");
				if (err != 0)
					puts("[DB]�ӽ�DB ���� ����!!");
				err = rename("client.tmp", "client.txt");
				if (err != 0)
					puts("[DB]�ӽ�DB �̸� ���� ����!!");
				return PERFECT;
			}
			else      // DB ���� ����
			{
				err = remove("client.tmp");
				if (err != 0)
					puts("[DB]�ӽ�DB ���� ����!!");
				puts("[����]DB�۾��� �Ϻ� ������ �߻��Ͽ����ϴ�.");
				puts("�ٽ� �޴��� �����ϱ� ���ؼ� [����]�� �Է��Ͻʽÿ�.");
				getchar();
				rewind(stdin);
				return ERROR;
			}
			
		}

		if(clientNum == cur)
			fwrite((void*)&super_client, 1, sizeof(User), tmp);
		else
			fwrite((void*)&user, 1, sizeof(User), tmp);
		cur++;
	}
	return ERROR;
	
}
void deposit()			//�Ա�
{
	int money;
	while (1)
	{
		system("cls");
		puts("[�Աݼ���]");
		for (int j = 0; j < super_client.numOfAcc; j++)
		{
			printf("%d.%s ���� ����\n", (j + 1), super_client.acc[j].accountNo);
		}
		printf("%d.�ڷΰ���\n", (super_client.numOfAcc + 1));
		printf("�Է� : ");
		scanf_s("%d", &i);
		rewind(stdin);

		if (i >= 1 && i <= (super_client.numOfAcc + 1))	// ��ȿ�� ���� ���
		{
			if (i == (super_client.numOfAcc + 1))	// �ڷΰ����ϰ��
			{
				system("cls");
				return;
			}
			else             // Ư�� ���¸� �������� ���
			{
				while (1)
				{
					system("cls");
					puts("[�Աݼ���]");
					puts("(����: 100���� ����, ��� : -1�Է�)");
					printf("�Ա��� �ݾ� : ");
					scanf_s("%d", &money);
					rewind(stdin);
					if (money >= 0 && money <= 1000000)
					{
						//�Ա�����
						super_client.acc[i - 1].money += money;
						DB_modify();
						commify(super_client.acc[i - 1].money, buf, 0);
						buf[strlen(buf) - 1] = '\0';
						puts("[�ԱݿϷ�]");
						printf("�Ա� �Ϸ� �� �ܾ� : %s ��\n", buf);
						puts("�ٽ� �޴��� �����ϱ� ���ؼ� [����]�� �Է��Ͻʽÿ�.");
						getchar();
						rewind(stdin);
						break;
						
					}
					else if (money == -1)
					{
						//�Ա����
						puts("[�Ա����]");
						puts("�ٽ� �޴��� �����ϱ� ���ؼ� [����]�� �Է��Ͻʽÿ�.");
						getchar();
						rewind(stdin);
						break;
					}
					else
					{
						puts("\a�ùٸ� ���� �Է��� �ֽʽÿ�.");
						puts("�ٽ� �޴��� �����ϱ� ���ؼ� [����]�� �Է��Ͻʽÿ�.");
						getchar();
						rewind(stdin);
					}
				}
				
				rewind(stdin);
				continue;
			}

		}
		else
		{
			puts("\a�ùٸ� ���� �Է��� �ֽʽÿ�. [����]�� �Է��Ͻʽÿ�.");
			getchar();
			rewind(stdin);
			continue;
		}
	}
	
}

void inquiry()          //��ȸ
{
	while (1)
	{
		system("cls");

		puts("[�����ܾ���ȸ����]\n");
		for (int j = 0; j < super_client.numOfAcc; j++)
		{
			printf("%d.%s ���� ��ȸ\n", (j + 1), super_client.acc[j].accountNo);
		}
		printf("%d.��������ȸ\n", (super_client.numOfAcc + 1));
		printf("%d.�ڷΰ���\n", (super_client.numOfAcc + 2));
		printf("�Է� : ");
		scanf_s("%d", &i);
		rewind(stdin);
		if (i >= 1 && i <= (super_client.numOfAcc + 2))
		{
			if (i == (super_client.numOfAcc + 1))	// ��������ȸ�ϰ��
			{
				puts("=========================");
				for (int j = 0; j < super_client.numOfAcc; j++)
				{
					commify(super_client.acc[j].money, buf, 0);
					buf[strlen(buf) - 1] = '\0';
					printf("���¹�ȣ : %s\n", super_client.acc[j].accountNo);
					printf("�ܾ�     : %s ��\n", buf);
					puts("=========================");

				}
				printf("�ٽ� �޴��� �����ϱ� ���ؼ� [����]�� �Է��Ͻʽÿ�.\n");
				getchar();
				rewind(stdin);
				continue;
			}
			if (i == (super_client.numOfAcc + 2))	// �ڷΰ����ϰ��
			{
				system("cls");
				break;
			}
			else             // Ư�� ���¸� �������� ���
			{
				for (int j = 0; j < super_client.numOfAcc; j++)
				{
					
					if ((i - 1) == j)	//i:����ڰ� ������ ���� (1,2,3), j: User����ü�� acc �迭��
					{
						commify(super_client.acc[j].money, buf, 0);
						buf[strlen(buf) - 1] = '\0';

						puts("=========================");
						printf("���¹�ȣ : %s\n", super_client.acc[j].accountNo);
						printf("�ܾ�     : %s ��\n",buf);
						puts("=========================");
					}
				}
				printf("�ٽ� �޴��� �����ϱ� ���ؼ� [����]�� �Է��Ͻʽÿ�.\n");
				getchar();
				rewind(stdin);
				continue;
			}

		}
		else
		{
			puts("\a�ùٸ� ���� �Է��� �ֽʽÿ�. [����]�� �Է��Ͻʽÿ�.");
			getchar();
			rewind(stdin);
			continue;
		}
	}

}
int login_system(char* id, char* pw)
{
	int fsize;
	User client;
	FILE * fp;
	fopen_s(&fp, "client.txt", "r");
	fseek(fp, 0, SEEK_END);
	fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	for (int i = 0; i < (fsize / sizeof(User)); i++) {
		fread((void*)&client, sizeof(User), 1, fp);
		if (!strcmp(client.user_id, id))
		{
			//printf("%s �� �ߺ��� ���̵��Դϴ�.\n", id);
			if (!strcmp(client.password, pw))
			{
				//�α��� ����
				super_client = client;
				clientNum = i;	// ����ȣ ����
				fclose(fp);
				return PERFECT;
			}
			
		}
		//printf("�񱳰� : %s\n�Է°�:%s\n\n", client.user_id, id);
	}
	fclose(fp);

	puts("\a\n���̵� �Ǵ� ��й�ȣ�� �߸��Ǿ����ϴ�.[����]�� �Է��Ͻʽÿ�.");
	getchar();
	rewind(stdin);
	return ERROR;
}


void login_screen()
{
	User client_clear = { 0, };
	
	while(1)
	{
		system("cls");
		puts("��� �α��� �۾��� �����Ͻðڽ��ϱ�?(1:����, 2:����ȭ��)");
		printf("�Է� : ");
		scanf_s("%d",&i);
		rewind(stdin);
		if (i == 1)
		{
			system("cls");
			puts("[�α���ȭ��]");
			printf("���̵� : ");
			gets(input);
			rewind(stdin);
			printf("���   : ");
			loging_password(input2);
			rewind(stdin);
			if (login_system(input, input2) == PERFECT)
				break;
		}
		else if (i == 2) 
		{
			return;
		}
		else
		{
			puts("�ùٸ� ���� �Է����ֽʽÿ�.");
			puts("[����]�� �Է��Ͻʽÿ�.");
			getchar();
			rewind(stdin);
			continue;
		}
		
		
	}
	while (1)
	{
		system("cls");
		puts("[KGP���༭��]");
		printf("%s �� �ȳ��ϼ���? \n���� ���࿡���� �Ʒ��� ���񽺵��� �����մϴ�.\n", super_client.name);
		puts("���ϴ� ���񽺿� �ش��ϴ� ���ڸ� �Է��Ͽ� �̿����ּ���.\n");
		puts("1.���°���[����]\n2.������ü[����]\n3.�����ܾ���ȸ[����]\n4.�Ա�[����]\n5.���[����]\n6.���°���\n7.�α׾ƿ�[����]");
		printf("�Է� : ");
		scanf_s("%d", &i);
		switch (i)
		{
		case 1:
			establish_account();
			break;
		case 2:
			transfer();
			break;
		case 3:
			inquiry();
			break;
		case 4:
			deposit();
			break;
		case 5:
			withdrawal();
			break;
		case 6:
			break;
		case 7:
			super_client = client_clear;		// �α��� ���� �ʱ�ȭ
			mainscreen();
			break;
		default:
			puts("���ٸ� ���� �Է��Ͽ� �ֽʽÿ�.");
			break;
		}
	}
	
	

}
int main(void)
{

	mainscreen();

	return 0;
}



void join_process()
{
	char password_check[CLIENT_PW_SIZE];
	int temp;
	User client = { 0 ,};
	system("cls");
	printf("�ȳ��ϼ��� KGP�����Դϴ�.\n�Ʒ� �Է��׸��� ��� �Է��Ͻø� ������ �Ϸ�˴ϴ�.\n");
	do {
		do {	// �̸� �Է�
			printf("\n�̸� : ");
			rewind(stdin);
			temp = excep_hangul(client.name);
		} while (temp != PERFECT);

		do {	// ������� �Է�
			temp = 0;
			printf("(���)�⵵(ex 1995) : ");
			temp = scanf_s("%hd", &client.year, sizeof(short));
			rewind(stdin);
			printf("(���)��(ex 01~12)  : ");
			temp = temp + scanf_s("%hd", &client.month, sizeof(short));
			rewind(stdin);
			printf("(���)��(ex 01~31)  : ");
			temp = temp + scanf_s("%hd", &client.day, sizeof(short));
			rewind(stdin);
		} while (excep_birth(client.year, client.month, client.day) != PERFECT || temp != 3);

		do {	// id �Է�
			printf("ID : ");
			rewind(stdin);
			temp = excep_id(client.user_id);
			if (temp == PERFECT)
				temp = id_overlapcheck(client.user_id);
		} while (temp != PERFECT);

		temp = 0;

		do {	// ��й�ȣ �Է�
			if (temp != 0)
				printf("\n��ġ�����ʽ��ϴ�. �ٽ� �Է����ֽʽÿ�.\n\n");

			do {
				printf("��й�ȣ(6�� �̻�,����,����,Ư������) : ");
				rewind(stdin);
			} while (excep_pwd(client.password) != PERFECT);

			printf("\n��й�ȣ ���Է� : ");
			rewind(stdin);
			excep_pwd(password_check);

		} while (temp = strcmp(client.password, password_check));

		system("cls");
		printf("==========[ȸ������]===============\n");
		printf("�̸� : %s\n", client.name);
		printf("(���)�⵵ : %d (��)\n", client.year);
		printf("(���)��   : %d (��)\n", client.month);
		printf("(���)��   : %d (��)\n", client.day);
		printf("ID : %s\n", client.user_id);
		printf("===================================\n");

		do {
			printf("\n�Է��Ͻ� ������ �½��ϱ�?(y/n) ");
			temp = getch();
		} while (temp != 'y' && temp != 'n');

	} while (temp == 'n');

	// ȸ������ �Ϸ� �� ���μ���

	printf("\n[���ϵ帳�ϴ�. ȸ�������� �Ϸ� �Ǿ����ϴ�.]\n");
	printf("���� �ٷ� ���¸� ���� �Ͻðڽ��ϱ�?(y/n)");
	temp = getch();
	while (temp != 'y' && temp != 'n') {
		temp = getch();
	}
	if (temp == 'y') {

		create_account(&client);
		char pwd_check[5];
		printf("\n[���� ��й�ȣ�� �Է����ֽʽÿ�.]\n");
		temp = 0;

		do {
			if (temp != 0)
				printf("\n��ġ�����ʽ��ϴ�. �ٽ� �Է����ֽʽÿ�.\n\n");

			do {
				printf("��й�ȣ(����4�ڸ�) : ");
				rewind(stdin);
				temp = excep_account_pwd(client.acc[0].password);
			} while (temp != PERFECT);

			do {
				printf("\n���Է� : ");
				rewind(stdin);
				temp = excep_account_pwd(pwd_check);
			} while (temp != PERFECT);


		} while (temp = strcmp(client.acc[0].password, pwd_check));



		user_add(&client);		//���Ͽ� ȸ���߰�
		printf("\n==========[ȸ������]=============\n");
		printf("�̸� : %s\n", client.name);
		printf("(���)�⵵ : %d (��)\n", client.year);
		printf("(���)��   : %d (��)\n", client.month);
		printf("(���)��   : %d (��)\n", client.day);
		printf("ID : %s\n", client.user_id);
		printf("==========[��������]=============\n");
		printf("���¹�ȣ : %s\n", client.acc[0].accountNo);
		printf("���°��� : %d\n", client.numOfAcc);
		printf("=================================\n");
		printf("����ȭ�� �̵��� ���Ͻø� ����Ű�� �����ʽÿ�.");
		getchar();


		mainscreen();

	}
	else {
		user_add(&client);
		printf("\n����ȭ�� �̵��� ���Ͻø� ����Ű�� �����ʽÿ�.");
		getchar();
		rewind(stdin);
		mainscreen();
		// ���¸� �ٷ� ������ ���� ���
	}

}

int excep_pwd(char * src_pw) {	//ȸ�� ��й�ȣ ����ó��****************************************
	char des_pw[CLIENT_PW_SIZE];
	int ch;
	for (int i = 0; i < CLIENT_PW_SIZE; i++) {

		ch = getch();
		if (i >(CLIENT_PW_SIZE - 5)) {	// 20�ڱ��� ������ ���� ������ �ʾҴٸ�
			printf("\n[����]��й�ȣ�� �ʹ� ����!\n");
			return ERROR;
		}
		else if (ch > 128) {	//�����ڵ� �Է¹߻�
			printf("\n[����]�ѱ��Է� �߻�\n");
			return ERROR;
		}
		else if (i < 6 && ch == '\r') {	// ������ ���̸鼭 6�� �̸��� ��
			printf("\n[����]6�� �̻��Է��Ͻÿ�.\n");
			return ERROR;
		}
		else if (ch == '\r') {	// �� ���ܸ� ��� ����ߴٸ�
			des_pw[i] = '\0';
			strcpy_s(src_pw, sizeof(des_pw), des_pw);
			return PERFECT;
		}
		else if (ch == '\b') {
			if (i <= 0) { // ���� ���� ������
				i = i - 1;
				continue;
			}
			i = i - 1;
			des_pw[i] = '\0';
		}
		else if (ch >= 33 && ch <= 126) {	// �������� �����Է�
			des_pw[i] = ch;
		}
		else { // �� �� ������
			printf("\n����,����,Ư��Ű�� �Է��� �˴ϴ�.\n");
			return ERROR;
		}
	}
	return ERROR;
}

int excep_hangul(char * a) {	//�̸� ����ó��************************************
	char name[MAX_NAME_SIZE];
	int ch;
	for (int i = 0; i < MAX_NAME_SIZE; i++) {
		ch = getchar();

		if (ch < 128 && ch != '\n') {
			printf("\a[����]�ѱ۸� �Է����ּ���.\n");
			return ERROR;
		}
		else if (i>(MAX_NAME_SIZE / 2)) {
			printf("\a[����]5�� ���Ϸ� �ۼ����ֽʽÿ�.\n");
			return ERROR;
		}
		else if (ch == '\n' && i < 3) {
			printf("\a[����]2�� �̻����� �ۼ����ֽʽÿ�.\n");
			return ERROR;
		}
		else if (ch == '\n') {
			name[i] = '\0';
			strcpy_s(a, sizeof(name), name);
			return PERFECT;
		}
		else {
			name[i] = ch;
		}

	}
	return ERROR;
}



int excep_id(char * src_id) {			//���̵� ����ó��*************************
	char des_id[MAX_CLIENT_ID_SIZE];
	int ch;
	for (int i = 0; i <= (MAX_CLIENT_ID_SIZE+1); i++) {
		ch = getchar();

		if (i>MAX_CLIENT_ID_SIZE) {
			printf("\a[����]���� 30�� �ִ��Դϴ�.\n");
			return ERROR;
		}
		else if (ch == '\n' && i < 3) {
			printf("\a[����]2�� �̻����� �ۼ����ֽʽÿ�.\n");
			return ERROR;
		}
		else if (ch == '\n') {
			des_id[i] = '\0';
			strcpy_s(src_id, sizeof(des_id), des_id);
			return PERFECT;
		}
		else if ((ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122) || (ch >= 48 && ch <= 57)) {
			des_id[i] = ch;
		}
		else {
			printf("\a[����]����,���ڷ� �ۼ����ּ���.\n");
			return ERROR;
		}


	}
	return ERROR;
}




int  excep_account_pwd(char * pwOfaccount) {		//���º�й�ȣ ����ó��**************************
	char pwd[5];
	int ch;
	for (int i = 0; i < 5; i++) {
		ch = getch();

		if (ch == '\b')
		{
			if (i <= 0)
			{
				i--;
				continue;
			}
			else
				i -= 2;
			putchar('\b');
			putchar(' ');
			putchar('\b');
		}
		else if ((ch != '\r') && i >= 4)
		{
			i--;
			continue;
		}
		else if ((ch == '\r') && (i >= 4))
		{
			pwd[i] = '\0';
			strcpy_s(pwOfaccount, sizeof(pwd), pwd);
			return PERFECT;
		}
		else if (ch >= 48 && ch <= 57) {	//�����Է�
			pwd[i] = ch;
			putchar('*');
		}
		else {
			i--;
			continue;
		}
	}
	return ERROR;
}



void user_add(User * client)		//ȸ������ �߰� �� ������Ʈ**************************
{
	FILE *fp;
	fopen_s(&fp, "client.txt", "a");
	fwrite((void*)client, sizeof((*client)), 1, fp);
	fflush(fp);
	fclose(fp);
}

int id_overlapcheck(char * id)	//Id �ߺ�üũ**************************************
{
	int fsize;
	User client;
	FILE * fp;
	fopen_s(&fp, "client.txt", "r");
	fseek(fp, 0, SEEK_END);
	fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	for (int i = 0; i < (fsize / sizeof(User)); i++) {
		fread((void*)&client, sizeof(User), 1, fp);
		if (!strcmp(client.user_id, id))
		{
			printf("\a%s �� �ߺ��� ���̵��Դϴ�.\n", id);
			fclose(fp);
			return ERROR;
		}
		//printf("�񱳰� : %s\n�Է°�:%s\n\n", client.user_id, id);
	}
	fclose(fp);
	return PERFECT;

}

void mainscreen()
{
	int n;
	while (1) {


		while (1) {	// ������ ��ȣ �Է� ������ ����ȭ�� ���
			system("cls");
			puts("Copyright �� by KGP 2016. All right reserved\n");
			puts("[���༭���̿����α׷�]");
			puts("1.�α���");
			puts("2.ȸ������");
			puts("3.ȸ�����\n");
			printf("�Է� : ");
			scanf_s("%d", &n, sizeof(n));
			rewind(stdin);
			if (1 <= n && n <= 3)	// ������ ��ȣ �˻�
				break;
		}

		switch (n) {	// �Է� ��ȣ�� ���� �Լ� ȣ��
		case 1: login_screen(); break;
		case 2: join_process(); break;
		case 3: memberlist(); break;
		default: break;
		}


	}

}



int excep_birth(int year, int mon, int day)
{
	if (!(year >= 1900 && year <= 9999))
	{
		printf("�ùٸ� ����⵵(year)�� �Է����ֽʽÿ�.\n");
		return ERROR;
	}
	else if (!(mon >= 1 && mon <= 12))
	{
		printf("�ùٸ� ��(month)�� �Է����ֽʽÿ�.\n");
		return ERROR;
	}
	else if (!(day >= 1 && day <= 31))
	{
		printf("�ùٸ� ����(day)�� �Է����ֽʽÿ�.\n");
		return ERROR;
	}
	else
	{
		while (year < 1970)	// time.h������ 1970�⵵ ���ĺ��� �ٷ�
			year += 4;

		time_t now;
		struct tm tm_now, tm_next;
		long int now_sec, next_sec, last_day;

		time(&now);
		localtime_s(&tm_now, &now);

		tm_now.tm_mday = 1;	//��¥�� 1�Ϸ� ����
		tm_now.tm_year = year - 1900;
		tm_next = tm_now;


		tm_now.tm_mon = (mon - 1);		//���� ��
		tm_next.tm_mon = mon;			//���� �� + 1

		now_sec = mktime(&tm_now);		//�ʷ� ��ȯ
		next_sec = mktime(&tm_next);	//�ʷ� ��ȯ


		time(&now);
		localtime_s(&tm_now, &now);
		if (tm_now.tm_year + 1900 < year)
		{
			printf("�̷����� ���ź��ΰ���!!?\n");
			return ERROR;
		}
		else if (tm_now.tm_year + 1900 == year && (tm_now.tm_mon+1) < mon)
		{
			printf("�̷����� ���ź��ΰ���!!?\n");
			return ERROR;
		}
		else if (tm_now.tm_year + 1900 == year && (tm_now.tm_mon+1) == mon && tm_now.tm_mday < day)
		{
			printf("�̷����� ���ź��ΰ���!!?\n");
			return ERROR;
		}

		last_day = (next_sec - now_sec) / DAYSEC;

		if (day <= last_day)
		{
			return PERFECT;
		}
		else
		{
			printf("�ش� �޿� ���� ����(day)�Դϴ�.\n");
			return ERROR;
		}
		
	}

}



int create_account(User* client) {	//���� ����*********************************
	int temp = 1; // �����ߺ����� ������� 0�� �����Ͽ� �ݺ��� �������ö� ���(1:�ߺ�/0:ok)
	int value[6], accNum;
	char acc_number1[7];
	char acc_number2[5];
	char ACCOUNT[12];
	int fsize;
	User exit_client;
	FILE * fp;
	srand((int)time(NULL));
	if (client->numOfAcc >= 3)	//	���°� 3�� �̻��̶�� ���� �޼����� �����϶�
	{
		printf("���´� 4���̻� ����� �����ϴ�. %d\n",client->numOfAcc);
		return ERROR;
	}
	fopen_s(&fp, "client.txt", "r");
	fseek(fp, 0, SEEK_END);
	fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	do
	{
		temp = 0;		// temp �ʱ�ȭ

		for (int i = 0; i < 6; i++) {	//���¹�ȣ 2��° �κ� ����
			value[i] = (rand() % 10 + 0);
		}
		sprintf(acc_number1, "%d%d%d%d%d%d", value[0], value[1], value[2], value[3], value[4], value[5]);

		for (int i = 0; i < 4; i++) {	//���¹�ȣ 3��° �κ� ����
			value[i] = (rand() % 10 + 0);
		}
		sprintf(acc_number2, "%d%d%d%d", value[0], value[1], value[2], value[3]);

		sprintf(ACCOUNT, "%s-%s", acc_number1, acc_number2);	//���� 2��° ��ȣ 3��° ��ȣ ����

		
		for (int i = 0; i < (fsize / sizeof(User)); i++) {	//ȸ������ŭ �ݺ�
			
			fread((void*)&exit_client, sizeof(User), 1, fp);
			
			for (int j = 0; j < exit_client.numOfAcc; j++)	//�ش� ȸ���� ���� �� ��ŭ �ݺ�
			{
				if (!strcmp(exit_client.acc[j].accountNo, ACCOUNT))	//�ߺ��Ǵ� ���¹�ȣ�� �ִٸ�
				{
					temp = 1; // �ߺ��Ǵ� ���¶�� ���� temp�� 1�� ǥ��;
				}
			}

		}

	} while (temp == 1);	// �ߺ��Ȱ��� �־��� ��� �ݺ�

	
	fclose(fp);
	accNum = client->numOfAcc;
	strcpy(client->acc[accNum].accountNo, ACCOUNT);
	client->numOfAcc++;
	return PERFECT;


}

void memberlist()
{
	int fsize;
	User client;
	FILE * fp;
	fopen_s(&fp, "client.txt", "r");
	fseek(fp, 0, SEEK_END);
	fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	system("cls");
	puts("[ȸ�����]");
	puts("===========================");
	for (int i = 0; i < (fsize / sizeof(User)); i++) {
		fread((void*)&client, sizeof(User), 1, fp);
		printf("�̸�: %s\n���̵�: %s\n���: %s\n", client.name, client.user_id,client.password);
		printf("���¹�ȣ : %s\n���°��� : %d\n===========================\n", 
			client.acc[0].accountNo, client.numOfAcc);
		//printf("�񱳰� : %s\n�Է°�:%s\n\n", client.user_id, id);
	}
	fclose(fp);
	if (fsize == 0)
		puts("���� ȸ���� �����ϴ�.");
	puts("����ȭ������ �̵��� ���Ͻø� �ƹ�Ű�� �����ֽʽÿ�.");
	rewind(stdin);
	getchar();
	mainscreen();
}