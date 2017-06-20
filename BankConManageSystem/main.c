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

User *login_client = NULL;	//	사용자가 로그인을 하면 동적메모리할당하여 이 변수에 주소값 저장
							//User userArray = NULL; // 고객명단 배열을 가리킬 포인터 변수


int id_overlapcheck(char * id);		//Id 중복체크**************************************
void user_add(User * client);	//회원정보 추가 및 업데이트
int excep_birth(int year, int mon, int day);		//생년월일 예외처리
int excep_hangul(char * a);	//이름 예외처리
int excep_pwd(char *a);		//회원 비밀번호 예외처리
int excep_id(char * src_id);			//아이디 예외처리
int excep_account_pwd(char * pwOfacc);  //계좌비밀번호 예외처리
int create_account(User* client);	//계좌생성
void memberlist();
void transfer_process(char * a);	//이체프로세스 *
void deposit();				//입금 *
void transfer();			//이체초기화면
int loging_password(char * a);	//회원비밀번호 입력
void establish_account();		//계좌개설
void withdrawal();		//출금 *
void mainscreen();		//메인 화면
void join_process();	    //은행가입
int DB_modify();	//DB 수정,업데이트
void inquiry();		//조회 *
void login_screen();
int login_system(char* id, char* pw);	// 아이디와 비번이 저장된 주소를 넘겨주면 간접적으로 참조하여 로그인


char input[4096];
char input2[4096];
char buf[50];
User super_client = { 0 };			//DB_modify시에 이 구조체의 내용 DB에 수정함
int clientNum;	// DB_modify에서 db 수정을 할때 몇번째 데이터를 수정할지를 저장

int join_processCount; // 총 가입 횟수
int i;

char *commify(double val, char *buf, int round) { /*오픈소스 활용*/
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
		else if (ch >= 33 && ch <= 126) {	//숫자입력
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
	int clientNum_des = 0;	// 이체 받을 사용자가 몇번째 바이트에 저장되있는지 저장할 변수
	int clientNum_tmp = clientNum;
	User client;
	User tmp_client;
	FILE * fp;
	fopen_s(&fp, "client.txt", "r");
	fseek(fp, 0, SEEK_END);
	fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	for (int i = 0; i < (fsize / sizeof(User)); i++) {
		fread((void*)&client, sizeof(User), 1, fp);		//회원정보 순차적으로 불러오기
		for (int j = 0; j < client.numOfAcc; j++)	// 그 회원의 계좌수만큼 반복하기
		{
			if (!strcmp(client.acc[j].accountNo, a))	// 지금 불어온 계좌와 사용자가 입력한 계좌와 같다면
			{
				fclose(fp);
				while (1)
				{

					printf("%s 이 분 맞습니까?(y/n)\n", client.name);
					tmp = getch();
					if (tmp == 'y')
					{

						while(1)
						{
							system("cls");
							puts("[이체서비스]");
							puts("[안내 1]보낼 계좌를 선택해 주신 후, 해당 계좌의 비밀번호 입력 후");
							puts("[안내 2]보낼 금액을 입력해 주십시오.");
							puts("[안내 3]100만원 이하로 제한되어있습니다.");
							puts("[안내 4]취소를 선택할 경우 메인화면으로 이동합니다.");
							for (int k = 0; k < super_client.numOfAcc; k++)
							{
								printf("%d.%s 계좌 선택\n", (k + 1), super_client.acc[k].accountNo);
							}
							printf("%d.취소\n", (super_client.numOfAcc + 1));
							printf("입력 : ");
							scanf_s("%d", &i);
							rewind(stdin);

							if (i == super_client.numOfAcc + 1)	// 최소일경우
							{
								return;
							}
							else    // 특정계좌를 선택하였을 경우
							{
								system("cls");
								puts("[이체서비스]");
								puts("[안내 1]보낼 계좌를 선택해 주신 후, 해당 계좌의 비밀번호 입력 후");
								puts("[안내 2]보낼 금액을 입력해 주십시오.");
								puts("[안내 3]100만원 이하로 제한되어있습니다.");
								puts("[안내 4]취소를 선택할 경우 메인화면으로 이동합니다.");
								printf("%s 계좌의 비밀번호 입력 : " , super_client.acc[i-1].accountNo);
								excep_account_pwd(pwd);
								if (!strcmp(pwd, super_client.acc[i - 1].password))
								{
									accNum = i - 1;
									system("cls");
									puts("[이체서비스]");
									puts("[안내 1]보낼 계좌를 선택해 주신 후, 해당 계좌의 비밀번호 입력 후");
									puts("[안내 2]보낼 금액을 입력해 주십시오.");
									puts("[안내 3]100만원 이하로 제한되어있습니다.");
									puts("[안내 4]-1를 입력하면 메인화면으로 이동됩니다.");
									commify(super_client.acc[accNum].money, buf, 0);
									buf[strlen(buf) - 1] = '\0';
									printf("[%s]계좌의 잔액 [ %s 원]\n", super_client.acc[accNum].accountNo,
										buf);
									printf("보낼 금액 : ");
									scanf_s("%d", &i);
									rewind(stdin);
									if (i >= 0 && i <= 1000000)
									{
										if (i <= super_client.acc[accNum].money)	// 잔액에 초과하지않는 금액을 입력했을경우
										{
											
											if (!strcmp(client.acc[j].accountNo, super_client.acc[accNum].accountNo)) // 같은계좌로 이체할 경우
											{

												puts("\a같은 계좌로 이체하는 것은 불가능합니다.");
												puts("[엔터]를 누르면 메인화면으로 이동합니다.");

											}
											else if (!strcmp(client.user_id, super_client.user_id))	//자신의 계좌로 이체할 경우
											{
												super_client.acc[j].money += i;
												super_client.acc[accNum].money -= i;
												DB_modify();
												puts("===[이체완료]===");

											}
											else    // 다른 회원의 계좌로 계좌이체를 할 경우
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
												puts("===[이체완료]===");

											}
											getchar();
											rewind(stdin);
											return;
										}
										else //잔액보다 높은 금액을 입력했을경우
										{
											puts("\a잔액을 초과하여 입력하셨습니다.");
											puts("계속 진행을 원하시면 [엔터]를 눌러주세요");
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
										puts("\a1원이상 100만원 이하로 입력해주십시오.");
										puts("계속 진행을 원하시면 [엔터]를 눌러주세요");
										getchar();
										rewind(stdin);
										continue;
									}
								}
								else
								{
									puts("\n\a비밀번호가 일치하지 않습니다.");
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
						puts("\a올바른 값을 입력해주십시오.");
						puts("계속 진행을 원하시면 [엔터]를 눌러주세요");
						getchar();
						rewind(stdin);
						continue;
					}
				}
				
			}
		}
		clientNum_des++;
		//printf("비교값 : %s\n입력값:%s\n\n", client.user_id, id);
	}
	puts("존재하지 않는 계좌번호입니다.\n[엔터]를 입력하면 메인화면으로 이동합니다.");
	getchar();
	rewind(stdin);
	fclose(fp);
	return PERFECT;
}
void transfer()         //이체
{
	char accNum[12];
	system("cls");
	puts("[이체서비스]");
	printf("받을 계좌 입력(-포함) : ");
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
		puts("[계좌개설서비스]");
		puts("정말 계좌를 개설하시겠습니까?(y,n) ");
		temp = getch();
		rewind(stdin);
		if (temp == 'y')
		{
			if (super_client.numOfAcc >= 3)
			{
				printf("\a");
				puts("계좌를 4개이상 만들수 없습니다.");
				puts("[엔터]를 누르면 메인화면으로 이동합니다.");
				getchar();
				rewind(stdin);
				return;
			}
			else
			{
				
				system("cls");
				puts("[계좌개설서비스]");
				puts("[안내 1]계좌를 개설하기 위해선 비밀번호가 필요합니다. 비밀번호를 입력해주십시오.");
				puts("[안내 2]뒤로가길 원하실 경우 비밀번호재입력을 틀리게 입력하시면 됩니다.");
				printf("비밀번호 입력(4자리) : ");	excep_account_pwd(pwd);
				printf("\n비밀번호 재입력 : ");	 excep_account_pwd(re_pwd);
				rewind(stdin);
				if (!strcmp(pwd, re_pwd))
				{
					create_account(&super_client);
					strcpy(super_client.acc[super_client.numOfAcc-1].password,pwd);
					DB_modify();
					system("cls");
					puts("[계좌개설서비스]");
					puts("계좌개설이 완료되었습니다.");
					printf("계좌번호 : %s\n", super_client.acc[super_client.numOfAcc - 1].accountNo);
					puts("[엔터]를 누르면 메인화면으로 이동합니다.");
					getchar();
					rewind(stdin);
					return;
				}
				else
				{
					system("cls");
					printf("\a");
					puts("[계좌개설서비스]");
					puts("비밀번호가 일치하지않습니다.");
					puts("[엔터]를 누르면 초기화면으로갑니다.");
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
			puts("올바른값을 입력해주십시오.");
			puts("계속 진행하기 위해선 [엔터]를 입력하십시오.");
			getchar();
			rewind(stdin);
			continue;
		}
	}
	

}
void DropOut()			//은행탈퇴
{}
void withdrawal()	    //출금
{
	int money;
	while (1)
	{
		system("cls");
		puts("[출금서비스]");
		for (int j = 0; j < super_client.numOfAcc; j++)
		{
			printf("%d.%s 계좌 선택\n", (j + 1), super_client.acc[j].accountNo);
		}
		printf("%d.뒤로가기\n", (super_client.numOfAcc + 1));
		printf("입력 : ");
		scanf_s("%d", &i);
		rewind(stdin);

		if (i >= 1 && i <= (super_client.numOfAcc + 1))	// 유효한 값일 경우
		{
			if (i == (super_client.numOfAcc + 1))	// 뒤로가기일경우
			{
				system("cls");
				return;
			}
			else             // 특정 계좌를 선택했을 경우; i-1은 선택한 계좌를 뜻함
			{
				char pwd[5];
				system("cls");
				puts("[출금서비스]");
				printf("해당 계좌의 비밀번호 입력 : ");
				excep_account_pwd(pwd);
				if (!strcmp(pwd, super_client.acc[i - 1].password))
				{
					commify(super_client.acc[i - 1].money, buf, 0);
					buf[strlen(buf) - 1] = '\0';
					while (1)
					{
						
						system("cls");
						puts("[출금서비스]");
						puts("(제한: 100만원 이하, 취소 : -1입력)");
						printf("잔액 [ %s 원] \n\n", buf);
						printf("출금할 금액 : ");
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
									puts("[출금완료]");
									printf("남은 잔액 [ %s 원 ]\n", buf);
									puts("다시 메뉴를 선택하기 위해선 [엔터]를 입력하십시오.");
									getchar();
									rewind(stdin);
								}
								else
								{
									printf("\a");
									system("cls");
									super_client.acc[i - 1].money += money;
									puts("[출금실패]");
									puts("다시 메뉴를 선택하기 위해선 [엔터]를 입력하십시오.");
									getchar();
									rewind(stdin);
								}
								break;
							}
							else
							{
								printf("\a");
								puts("[잔액보다 많은 출금액을 입력하였습니다.]");
								puts("다시 메뉴를 선택하기 위해선 [엔터]를 입력하십시오.");
								getchar();
								rewind(stdin);
							}
							//입금진행
						}
						else if (money == -1)
						{
							//입금취소
							puts("[출금취소]");
							puts("다시 메뉴를 선택하기 위해선 [엔터]를 입력하십시오.");
							getchar();
							rewind(stdin);
							break;
						}
						else
						{
							printf("\a");
							puts("올바른 값을 입력해 주십시오.");
							puts("다시 메뉴를 선택하기 위해선 [엔터]를 입력하십시오.");
							getchar();
							rewind(stdin);
						}
					}
				}
				else
				{
					printf("\a");
					puts("\n비밀번호가 일치하지않습니다.");
					puts("다시 메뉴를 선택하기 위해선 [엔터]를 입력하십시오.");
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
			puts("올바른 값을 입력해 주십시오. [엔터]를 입력하십시오.");
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
			if (feof(src) != 0)		// DB 수정 완료
			{
				fwrite((void*)&user, 1, readCnt, tmp);
				fclose(tmp);
				fclose(src);
				err = remove("client.txt");
				if (err != 0)
					puts("[DB]임시DB 삭제 오류!!");
				err = rename("client.tmp", "client.txt");
				if (err != 0)
					puts("[DB]임시DB 이름 변경 오류!!");
				return PERFECT;
			}
			else      // DB 수정 오류
			{
				err = remove("client.tmp");
				if (err != 0)
					puts("[DB]임시DB 삭제 오류!!");
				puts("[오류]DB작업중 일부 오류가 발생하였습니다.");
				puts("다시 메뉴를 선택하기 위해선 [엔터]를 입력하십시오.");
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
void deposit()			//입금
{
	int money;
	while (1)
	{
		system("cls");
		puts("[입금서비스]");
		for (int j = 0; j < super_client.numOfAcc; j++)
		{
			printf("%d.%s 계좌 선택\n", (j + 1), super_client.acc[j].accountNo);
		}
		printf("%d.뒤로가기\n", (super_client.numOfAcc + 1));
		printf("입력 : ");
		scanf_s("%d", &i);
		rewind(stdin);

		if (i >= 1 && i <= (super_client.numOfAcc + 1))	// 유효한 값일 경우
		{
			if (i == (super_client.numOfAcc + 1))	// 뒤로가기일경우
			{
				system("cls");
				return;
			}
			else             // 특정 계좌를 선택했을 경우
			{
				while (1)
				{
					system("cls");
					puts("[입금서비스]");
					puts("(제한: 100만원 이하, 취소 : -1입력)");
					printf("입금할 금액 : ");
					scanf_s("%d", &money);
					rewind(stdin);
					if (money >= 0 && money <= 1000000)
					{
						//입금진행
						super_client.acc[i - 1].money += money;
						DB_modify();
						commify(super_client.acc[i - 1].money, buf, 0);
						buf[strlen(buf) - 1] = '\0';
						puts("[입금완료]");
						printf("입금 완료 후 잔액 : %s 원\n", buf);
						puts("다시 메뉴를 선택하기 위해선 [엔터]를 입력하십시오.");
						getchar();
						rewind(stdin);
						break;
						
					}
					else if (money == -1)
					{
						//입금취소
						puts("[입금취소]");
						puts("다시 메뉴를 선택하기 위해선 [엔터]를 입력하십시오.");
						getchar();
						rewind(stdin);
						break;
					}
					else
					{
						puts("\a올바른 값을 입력해 주십시오.");
						puts("다시 메뉴를 선택하기 위해선 [엔터]를 입력하십시오.");
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
			puts("\a올바른 값을 입력해 주십시오. [엔터]를 입력하십시오.");
			getchar();
			rewind(stdin);
			continue;
		}
	}
	
}

void inquiry()          //조회
{
	while (1)
	{
		system("cls");

		puts("[계좌잔액조회서비스]\n");
		for (int j = 0; j < super_client.numOfAcc; j++)
		{
			printf("%d.%s 계좌 조회\n", (j + 1), super_client.acc[j].accountNo);
		}
		printf("%d.모든계좌조회\n", (super_client.numOfAcc + 1));
		printf("%d.뒤로가기\n", (super_client.numOfAcc + 2));
		printf("입력 : ");
		scanf_s("%d", &i);
		rewind(stdin);
		if (i >= 1 && i <= (super_client.numOfAcc + 2))
		{
			if (i == (super_client.numOfAcc + 1))	// 모든계좌조회일경우
			{
				puts("=========================");
				for (int j = 0; j < super_client.numOfAcc; j++)
				{
					commify(super_client.acc[j].money, buf, 0);
					buf[strlen(buf) - 1] = '\0';
					printf("계좌번호 : %s\n", super_client.acc[j].accountNo);
					printf("잔액     : %s 원\n", buf);
					puts("=========================");

				}
				printf("다시 메뉴를 선택하기 위해선 [엔터]를 입력하십시오.\n");
				getchar();
				rewind(stdin);
				continue;
			}
			if (i == (super_client.numOfAcc + 2))	// 뒤로가기일경우
			{
				system("cls");
				break;
			}
			else             // 특정 계좌를 선택했을 경우
			{
				for (int j = 0; j < super_client.numOfAcc; j++)
				{
					
					if ((i - 1) == j)	//i:사용자가 선택한 계좌 (1,2,3), j: User구조체의 acc 배열값
					{
						commify(super_client.acc[j].money, buf, 0);
						buf[strlen(buf) - 1] = '\0';

						puts("=========================");
						printf("계좌번호 : %s\n", super_client.acc[j].accountNo);
						printf("잔액     : %s 원\n",buf);
						puts("=========================");
					}
				}
				printf("다시 메뉴를 선택하기 위해선 [엔터]를 입력하십시오.\n");
				getchar();
				rewind(stdin);
				continue;
			}

		}
		else
		{
			puts("\a올바른 값을 입력해 주십시오. [엔터]를 입력하십시오.");
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
			//printf("%s 는 중복된 아이디입니다.\n", id);
			if (!strcmp(client.password, pw))
			{
				//로그인 성공
				super_client = client;
				clientNum = i;	// 고객번호 저장
				fclose(fp);
				return PERFECT;
			}
			
		}
		//printf("비교값 : %s\n입력값:%s\n\n", client.user_id, id);
	}
	fclose(fp);

	puts("\a\n아이디 또는 비밀번호가 잘못되었습니다.[엔터]를 입력하십시오.");
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
		puts("계속 로그인 작업을 진행하시겠습니까?(1:진행, 2:메인화면)");
		printf("입력 : ");
		scanf_s("%d",&i);
		rewind(stdin);
		if (i == 1)
		{
			system("cls");
			puts("[로그인화면]");
			printf("아이디 : ");
			gets(input);
			rewind(stdin);
			printf("비번   : ");
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
			puts("올바른 값을 입력해주십시오.");
			puts("[엔터]를 입력하십시오.");
			getchar();
			rewind(stdin);
			continue;
		}
		
		
	}
	while (1)
	{
		system("cls");
		puts("[KGP은행서비스]");
		printf("%s 님 안녕하세요? \n저희 은행에서는 아래의 서비스들을 지원합니다.\n", super_client.name);
		puts("원하는 서비스에 해당하는 숫자를 입력하여 이용해주세요.\n");
		puts("1.계좌개설[구현]\n2.계좌이체[구현]\n3.계좌잔액조회[구현]\n4.입금[구현]\n5.출금[구현]\n6.계좌관리\n7.로그아웃[구현]");
		printf("입력 : ");
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
			super_client = client_clear;		// 로그인 정보 초기화
			mainscreen();
			break;
		default:
			puts("오바른 값을 입력하여 주십시오.");
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
	printf("안녕하세요 KGP은행입니다.\n아래 입력항목을 모두 입력하시면 가입이 완료됩니다.\n");
	do {
		do {	// 이름 입력
			printf("\n이름 : ");
			rewind(stdin);
			temp = excep_hangul(client.name);
		} while (temp != PERFECT);

		do {	// 생년월일 입력
			temp = 0;
			printf("(출생)년도(ex 1995) : ");
			temp = scanf_s("%hd", &client.year, sizeof(short));
			rewind(stdin);
			printf("(출생)달(ex 01~12)  : ");
			temp = temp + scanf_s("%hd", &client.month, sizeof(short));
			rewind(stdin);
			printf("(출생)일(ex 01~31)  : ");
			temp = temp + scanf_s("%hd", &client.day, sizeof(short));
			rewind(stdin);
		} while (excep_birth(client.year, client.month, client.day) != PERFECT || temp != 3);

		do {	// id 입력
			printf("ID : ");
			rewind(stdin);
			temp = excep_id(client.user_id);
			if (temp == PERFECT)
				temp = id_overlapcheck(client.user_id);
		} while (temp != PERFECT);

		temp = 0;

		do {	// 비밀번호 입력
			if (temp != 0)
				printf("\n일치하지않습니다. 다시 입력해주십시오.\n\n");

			do {
				printf("비밀번호(6자 이상,영문,숫자,특수문자) : ");
				rewind(stdin);
			} while (excep_pwd(client.password) != PERFECT);

			printf("\n비밀번호 재입력 : ");
			rewind(stdin);
			excep_pwd(password_check);

		} while (temp = strcmp(client.password, password_check));

		system("cls");
		printf("==========[회원정보]===============\n");
		printf("이름 : %s\n", client.name);
		printf("(출생)년도 : %d (년)\n", client.year);
		printf("(출생)달   : %d (월)\n", client.month);
		printf("(출생)일   : %d (일)\n", client.day);
		printf("ID : %s\n", client.user_id);
		printf("===================================\n");

		do {
			printf("\n입력하신 정보가 맞습니까?(y/n) ");
			temp = getch();
		} while (temp != 'y' && temp != 'n');

	} while (temp == 'n');

	// 회원가입 완료 후 프로세스

	printf("\n[축하드립니다. 회원가입이 완료 되었습니다.]\n");
	printf("지금 바로 계좌를 개설 하시겠습니까?(y/n)");
	temp = getch();
	while (temp != 'y' && temp != 'n') {
		temp = getch();
	}
	if (temp == 'y') {

		create_account(&client);
		char pwd_check[5];
		printf("\n[계좌 비밀번호를 입력해주십시오.]\n");
		temp = 0;

		do {
			if (temp != 0)
				printf("\n일치하지않습니다. 다시 입력해주십시오.\n\n");

			do {
				printf("비밀번호(숫자4자리) : ");
				rewind(stdin);
				temp = excep_account_pwd(client.acc[0].password);
			} while (temp != PERFECT);

			do {
				printf("\n재입력 : ");
				rewind(stdin);
				temp = excep_account_pwd(pwd_check);
			} while (temp != PERFECT);


		} while (temp = strcmp(client.acc[0].password, pwd_check));



		user_add(&client);		//파일에 회원추가
		printf("\n==========[회원정보]=============\n");
		printf("이름 : %s\n", client.name);
		printf("(출생)년도 : %d (년)\n", client.year);
		printf("(출생)월   : %d (월)\n", client.month);
		printf("(출생)일   : %d (일)\n", client.day);
		printf("ID : %s\n", client.user_id);
		printf("==========[계좌정보]=============\n");
		printf("계좌번호 : %s\n", client.acc[0].accountNo);
		printf("계좌개수 : %d\n", client.numOfAcc);
		printf("=================================\n");
		printf("메인화면 이동을 원하시면 엔터키를 누르십시오.");
		getchar();


		mainscreen();

	}
	else {
		user_add(&client);
		printf("\n메인화면 이동을 원하시면 엔터키를 누르십시오.");
		getchar();
		rewind(stdin);
		mainscreen();
		// 계좌를 바로 만들지 않을 경우
	}

}

int excep_pwd(char * src_pw) {	//회원 비밀번호 예외처리****************************************
	char des_pw[CLIENT_PW_SIZE];
	int ch;
	for (int i = 0; i < CLIENT_PW_SIZE; i++) {

		ch = getch();
		if (i >(CLIENT_PW_SIZE - 5)) {	// 20자까지 문장의 끝이 나오지 않았다면
			printf("\n[에러]비밀번호가 너무 길어요!\n");
			return ERROR;
		}
		else if (ch > 128) {	//유니코드 입력발생
			printf("\n[에러]한글입력 발생\n");
			return ERROR;
		}
		else if (i < 6 && ch == '\r') {	// 문장의 끝이면서 6자 미만일 때
			printf("\n[에러]6자 이상입력하시오.\n");
			return ERROR;
		}
		else if (ch == '\r') {	// 위 예외를 모두 통과했다면
			des_pw[i] = '\0';
			strcpy_s(src_pw, sizeof(des_pw), des_pw);
			return PERFECT;
		}
		else if (ch == '\b') {
			if (i <= 0) { // 지울 값이 없을때
				i = i - 1;
				continue;
			}
			i = i - 1;
			des_pw[i] = '\0';
		}
		else if (ch >= 33 && ch <= 126) {	// 성공적인 문자입력
			des_pw[i] = ch;
		}
		else { // 그 외 오류들
			printf("\n영문,숫자,특수키만 입력이 됩니다.\n");
			return ERROR;
		}
	}
	return ERROR;
}

int excep_hangul(char * a) {	//이름 예외처리************************************
	char name[MAX_NAME_SIZE];
	int ch;
	for (int i = 0; i < MAX_NAME_SIZE; i++) {
		ch = getchar();

		if (ch < 128 && ch != '\n') {
			printf("\a[에러]한글만 입력해주세요.\n");
			return ERROR;
		}
		else if (i>(MAX_NAME_SIZE / 2)) {
			printf("\a[에러]5자 이하로 작성해주십시오.\n");
			return ERROR;
		}
		else if (ch == '\n' && i < 3) {
			printf("\a[에러]2자 이상으로 작성해주십시오.\n");
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



int excep_id(char * src_id) {			//아이디 예외처리*************************
	char des_id[MAX_CLIENT_ID_SIZE];
	int ch;
	for (int i = 0; i <= (MAX_CLIENT_ID_SIZE+1); i++) {
		ch = getchar();

		if (i>MAX_CLIENT_ID_SIZE) {
			printf("\a[에러]길이 30이 최대입니다.\n");
			return ERROR;
		}
		else if (ch == '\n' && i < 3) {
			printf("\a[에러]2자 이상으로 작성해주십시오.\n");
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
			printf("\a[에러]영어,숫자로 작성해주세요.\n");
			return ERROR;
		}


	}
	return ERROR;
}




int  excep_account_pwd(char * pwOfaccount) {		//계좌비밀번호 예외처리**************************
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
		else if (ch >= 48 && ch <= 57) {	//숫자입력
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



void user_add(User * client)		//회원정보 추가 및 업데이트**************************
{
	FILE *fp;
	fopen_s(&fp, "client.txt", "a");
	fwrite((void*)client, sizeof((*client)), 1, fp);
	fflush(fp);
	fclose(fp);
}

int id_overlapcheck(char * id)	//Id 중복체크**************************************
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
			printf("\a%s 는 중복된 아이디입니다.\n", id);
			fclose(fp);
			return ERROR;
		}
		//printf("비교값 : %s\n입력값:%s\n\n", client.user_id, id);
	}
	fclose(fp);
	return PERFECT;

}

void mainscreen()
{
	int n;
	while (1) {


		while (1) {	// 적절한 번호 입력 전까지 메인화면 출력
			system("cls");
			puts("Copyright ⓒ by KGP 2016. All right reserved\n");
			puts("[은행서비스이용프로그램]");
			puts("1.로그인");
			puts("2.회원가입");
			puts("3.회원목록\n");
			printf("입력 : ");
			scanf_s("%d", &n, sizeof(n));
			rewind(stdin);
			if (1 <= n && n <= 3)	// 적절한 번호 검사
				break;
		}

		switch (n) {	// 입력 번호에 따른 함수 호출
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
		printf("올바른 출생년도(year)를 입력해주십시오.\n");
		return ERROR;
	}
	else if (!(mon >= 1 && mon <= 12))
	{
		printf("올바른 달(month)을 입력해주십시오.\n");
		return ERROR;
	}
	else if (!(day >= 1 && day <= 31))
	{
		printf("올바른 일자(day)를 입력해주십시오.\n");
		return ERROR;
	}
	else
	{
		while (year < 1970)	// time.h에서는 1970년도 이후부터 다룸
			year += 4;

		time_t now;
		struct tm tm_now, tm_next;
		long int now_sec, next_sec, last_day;

		time(&now);
		localtime_s(&tm_now, &now);

		tm_now.tm_mday = 1;	//날짜를 1일로 수정
		tm_now.tm_year = year - 1900;
		tm_next = tm_now;


		tm_now.tm_mon = (mon - 1);		//현재 달
		tm_next.tm_mon = mon;			//현재 달 + 1

		now_sec = mktime(&tm_now);		//초로 변환
		next_sec = mktime(&tm_next);	//초로 변환


		time(&now);
		localtime_s(&tm_now, &now);
		if (tm_now.tm_year + 1900 < year)
		{
			printf("미래에서 오신분인가요!!?\n");
			return ERROR;
		}
		else if (tm_now.tm_year + 1900 == year && (tm_now.tm_mon+1) < mon)
		{
			printf("미래에서 오신분인가요!!?\n");
			return ERROR;
		}
		else if (tm_now.tm_year + 1900 == year && (tm_now.tm_mon+1) == mon && tm_now.tm_mday < day)
		{
			printf("미래에서 오신분인가요!!?\n");
			return ERROR;
		}

		last_day = (next_sec - now_sec) / DAYSEC;

		if (day <= last_day)
		{
			return PERFECT;
		}
		else
		{
			printf("해당 달에 없는 일자(day)입니다.\n");
			return ERROR;
		}
		
	}

}



int create_account(User* client) {	//계좌 생성*********************************
	int temp = 1; // 계좌중복값이 없을경우 0을 대입하여 반복문 빠져나올때 사용(1:중복/0:ok)
	int value[6], accNum;
	char acc_number1[7];
	char acc_number2[5];
	char ACCOUNT[12];
	int fsize;
	User exit_client;
	FILE * fp;
	srand((int)time(NULL));
	if (client->numOfAcc >= 3)	//	계좌가 3개 이상이라면 오류 메세지를 리턴하라
	{
		printf("계좌는 4개이상 만들수 없습니다. %d\n",client->numOfAcc);
		return ERROR;
	}
	fopen_s(&fp, "client.txt", "r");
	fseek(fp, 0, SEEK_END);
	fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	do
	{
		temp = 0;		// temp 초기화

		for (int i = 0; i < 6; i++) {	//계좌번호 2번째 부분 생성
			value[i] = (rand() % 10 + 0);
		}
		sprintf(acc_number1, "%d%d%d%d%d%d", value[0], value[1], value[2], value[3], value[4], value[5]);

		for (int i = 0; i < 4; i++) {	//계좌번호 3번째 부분 생성
			value[i] = (rand() % 10 + 0);
		}
		sprintf(acc_number2, "%d%d%d%d", value[0], value[1], value[2], value[3]);

		sprintf(ACCOUNT, "%s-%s", acc_number1, acc_number2);	//계좌 2번째 번호 3번째 번호 병합

		
		for (int i = 0; i < (fsize / sizeof(User)); i++) {	//회원수만큼 반복
			
			fread((void*)&exit_client, sizeof(User), 1, fp);
			
			for (int j = 0; j < exit_client.numOfAcc; j++)	//해당 회원의 계좌 수 만큼 반복
			{
				if (!strcmp(exit_client.acc[j].accountNo, ACCOUNT))	//중복되는 계좌번호가 있다면
				{
					temp = 1; // 중복되는 계좌라는 것을 temp에 1로 표시;
				}
			}

		}

	} while (temp == 1);	// 중복된값이 있었을 경우 반복

	
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
	puts("[회원목록]");
	puts("===========================");
	for (int i = 0; i < (fsize / sizeof(User)); i++) {
		fread((void*)&client, sizeof(User), 1, fp);
		printf("이름: %s\n아이디: %s\n비번: %s\n", client.name, client.user_id,client.password);
		printf("계좌번호 : %s\n계좌개수 : %d\n===========================\n", 
			client.acc[0].accountNo, client.numOfAcc);
		//printf("비교값 : %s\n입력값:%s\n\n", client.user_id, id);
	}
	fclose(fp);
	if (fsize == 0)
		puts("현재 회원이 없습니다.");
	puts("메인화면으로 이동을 원하시면 아무키나 눌러주십시오.");
	rewind(stdin);
	getchar();
	mainscreen();
}