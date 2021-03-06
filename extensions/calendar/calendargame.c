#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>

#include "historyeasylevel.h"
#include "historymediumlevel.h"
#include "historyhardlevel.h"
#include "calendargame.h"
#include "hangman.h"

#define TRUE    1
#define FALSE   0

int determineleapyear(int);

/* Use to generate random date */
int randomGen(int);
int randomGenYear();

int calculateDay(int, int, int);
int dayToInt(char*);
char* intToDay(int);

/* Date game */
void Dateprogram();
void Holidayrogram();

/* helper functions for holiday-date game */
char* int_to_holiday(int);
int get_month(int);
int get_day(int);

/* history game */
void Historyprogram();
void Hangmanprogram();

int days_in_month[]={0,31,28,31,30,31,30,31,31,30,31,30,31};
int score = 0;
int holiday = 0;

/* check if it is a leap year */
int determineleapyear(int year)
{
	if((year%4 == FALSE && year%100 != FALSE) || year%400 == FALSE)
	{
		days_in_month[2] = 29;
		return TRUE;
	}
	else
	{
		days_in_month[2] = 28;
		return FALSE;
	}
}

/* genarate a random number in the given range */
int randomGen(int range) {
   int random = 0;

   while(random > range || random <= 0)
	 {
     random = rand();
   }
   return random;
}

/* genarate a random year */
int randomGenYear() {

  int random = 0;

  while(random < 1000 || random > 3000)
	{
    random = rand();
  }
  return random;

}

/* calculate the day of week of a given date */
int calculateDay(int day, int month, int year) {
  /* Use Zeller's Rule here */
	int last2year = year%100;  // last 2 digits of year
	int first2year = (year - last2year)/100;  // first 2 digits of year

	/* use a special number for each month */
	/* March = 1, April = 2, ..., February = 12 */
	int newMonth = (month - 2)%12;
	if(month == 2)
	{
		newMonth = 12;
	}

	/* only integer part is needed */
	int floorlast = floor(last2year/4);
	int floorfirst = floor(first2year/4);

	/* calculation */
	int result = day + floor((13*newMonth - 1)/5) + last2year +
	             floorlast + floorfirst - 2*first2year;

	/* make sure it returns a positive value */
	return (result % 7 + 7) % 7;
}

/* switch between day and corresponding integer number */
int dayToInt(char* day) {

	if(strcasecmp(day, "Sunday") == 0)
	{
		return 0;
  } else if(strcasecmp(day, "Monday") == 0) {
	return 1;
  } else if(strcasecmp(day, "Tuesday") == 0) {
	return 2;
  } else if(strcasecmp(day, "Wednesday") == 0) {
	return 3;
  } else if(strcasecmp(day, "Thursday") == 0) {
	return 4;
  } else if(strcasecmp(day, "Friday") == 0) {
	return 5;
  } else if(strcasecmp(day, "Saturday") == 0) {
	return 6;
  } else {
	return -1;
  }
}

char* intToDay(int day) {
	switch(day) {
		case 0 : return "Sunday"; break;
		case 1 : return "Monday"; break;
	  case 2 : return "Tuesday";break;
		case 3 : return "Wednesday"; break;
    case 4 : return "Thursday"; break;
	  case 5 : return "Friday"; break;
		case 6 : return "Saturday";break;
		default : return " "; break;
	}
}

////////////////////////////// Date Game ///////////////////////////////////

/* Date Game */
void Dateprogram() {
	if(holiday > 1) goto date;

  printf("~~~~~~~~~~~~~~~~~~~~Welcome to Date game!~~~~~~~~~~~~~~~~~~~~\n");

	/* normal date game or holiday game randomlly */
	date:;
  int date_or_holiday = randomGen(4);
  /* get holiday game at the rate 25% */
  if (date_or_holiday == 1)
	{
	  Holidayrogram();
	  return;
  }

	/* generate a random date */
	int year  = randomGenYear();
  int month = randomGen(12);
	int day = randomGen(31);

	/* cases for months with different days */
  if(month == 2)
	{
		if(determineleapyear(year) == TRUE)
		{
			day = randomGen(29);
		} else {
			day = randomGen(28);
		}
  } else if(month == 4 || month == 6 || month == 9 || month == 11) {
		day = randomGen(30);
	}

	/* show game */
	int answer = calculateDay(day, month, year);
  printf("What day does the date %i/%i/%i have?\n", day, month, year);
	printf("Please write in form: Monday, Tuesday...e.t.c \n");
	char buffer[11];
	scanf("%s", buffer);
	isDay:;
	int result = dayToInt(buffer);

	/* check input */
	if(result == -1)
	{
		printf("Sorry to inform you that %s is not a day.\n", buffer);
		printf("Please type in a day.\n");
		scanf("%s", buffer); goto isDay;
	}

	if(result == answer)
	{
		printf("Congratulations!!! You are a genius.\n");
		printf("%s is indeed the correct day!\n", intToDay(answer));
	} else {
		printf("Unfortunately, this is the wrong answer.\n");
		printf("%s is the correct day\n", intToDay(answer));
	}

	play_again:;
	printf("Would you like to play again [y/n]:");
	char ans;
	scanf(" %c", &ans);

	if(tolower(ans) == 'y')
	{
		printf("                                         \n");
		goto date;
	} else if(tolower(ans) == 'n') {
	  printf("Goodbye! We hope you enjoyed the game!\n");
  } else {
	  printf("Sorry, you have typed in an invalid letter!\n");
	  printf("Please type in y or n.\n");
	  goto play_again;
  }
}

/* a list of holidays */
char* int_to_holiday(int holidayNum) {
	switch (holidayNum) {
		case 1: return "New Year's Day"; break;
		case 2: return "Valentine's Day"; break;
		case 3: return "Halloween"; break;
		case 4: return "Christmas Day"; break;
		case 5: return "Boxing Day"; break;
		case 6: return "New Year's Eve"; break;
		default:  return "";
		  break;
	}
}

/* get the date of holiday */
int get_month(int holidayNum) {
	switch (holidayNum) {
		case 1: return 1;  break;
		case 2: return 2;  break;
		case 3: return 10; break;
		case 4:
		case 5:
		case 6: return 12; break;
		default: return 0; break;
	}
}

int get_day(int holidayNum) {
	switch (holidayNum) {
		case 1: return 1;  break;
		case 2: return 14; break;
		case 3: return 31; break;
		case 4: return 25; break;
		case 5: return 26; break;
		case 6: return 31; break;
		default: return 0;
		  break;
	}
}

/* Date game in holiday form */
void Holidayrogram() {
	/* get a holiday from the list randomly */
	int holidayNum = randomGen(6);

	int year  = randomGenYear();
	/* get the date of that holiday */
	int month = get_month(holidayNum);
	int day   = get_day(holidayNum);

	char* holiday_name = int_to_holiday(holidayNum);

	int answer = calculateDay(day, month, year);

	/* show game */
	printf("What day does the %s in %i have?\n", holiday_name, year);
	printf("Please write in form: Monday, Tuesday...e.t.c \n");
	char buffer[11];
	scanf("%s", buffer);
	isDay:;
	int result = dayToInt(buffer);

	/* check input */
	if(result == -1)
	{
		printf("Sorry to inform you that %s is not a day.\n", buffer);
		printf("Please type in a day.\n");
		scanf("%s", buffer); goto isDay;
	}
	if(result == answer)
	{
		printf("Congratulations!!! You are a genius.\n");
		printf("%s is indeed the correct day!\n", intToDay(answer));
	} else {
		printf("Unfortunately, this is the wrong answer.\n");
		printf("%s is the correct day\n", intToDay(answer));
	}

	play_again:;
	printf("Would you like to play again [y/n]:");
	char ans;
	scanf(" %c", &ans);

	if(tolower(ans) == 'y')
	{
		printf("                                          \n");
		holiday++;
		Dateprogram();
	} else if(tolower(ans) == 'n') {
		printf("Goodbye! We hope you enjoyed the game!\n");
	} else {
		printf("Sorry, you have typed in an invalid letter!\n");
		printf("Please type in y or n.\n");
		goto play_again;
	}
}

////////////////////////////// History Game ///////////////////////////////////

/* record current pionts */
void progress() {
	printf("-----------------------------PROGRESS------------------------------\n");
	printf("                                                               \n");
	printf("                     You have -> %i <- points.         \n", score);
	printf("                                                               \n");
	printf("--------------------------------------------------------------------\n");
}

/* ask whether go to next question or not */
void nextques() {
	printf("                                                               \n");
	printf("Would you like to go to the next question? [y/n]\n");
  char c;
	scanf(" %c", &c);
	if(tolower(c) == 'y')
	{
		system("clear");
	} else if(tolower(c) == 'n') {
		printf("Thank you for playing the History game! We hope you enjoyed it!\n");
		exit(EXIT_SUCCESS);
	} else {
		printf("Sorry, you have typed in an invalid letter!\n");
		printf("Please type in y or n.\n");
		nextques();
	}
}

/* ask whether go to next level or not */
void nextlevel() {
	printf("                                                               \n");
	printf("Would you like to go to the next level? [y/n]\n");
  char c;
	scanf(" %c", &c);
	if(tolower(c) == 'y')
	{
		system("clear");
	} else if(tolower(c) == 'n') {
		printf("Thank you for playing the History game! We hope you enjoyed it!\n");
		exit(EXIT_SUCCESS);
	} else {
		printf("Sorry, you have typed in an invalid letter!\n");
		printf("Please type in y or n.\n");
		nextlevel();
	}
}

/* history game */
void Historyprogram() {

	/* show game */
	printf("~~~~~~~~~~~~~~~~~~~~Welcome to The History game!~~~~~~~~~~~~~~~~~~~~\n");
  printf("                                                               \n");
	printf("--------------------------------INFO---------------------------------\n");
	printf("                                                               \n");
	printf("      In this game, you will be asked about different historical \n");
	printf("                   events which took place.\n");
	printf("                                                               \n");
	printf("------------------------        ~~~        --------------------------\n");
	printf("                                                               \n");
	printf("           You will have to guess the dates \n");
	printf("                     of these events \n");
	printf("                                                               \n");
	printf("-------------------------------ENJOY---------------------------------\n");
	printf("                                                               \n");
	printf("Continue? [y/n]\n");
  /*Check if user wants to continue */
  repeatCont : ;
	char userIN;
	scanf(" %c", &userIN);
	char ch = tolower(userIN);

	if(ch == 'y') {
		system("clear");
		goto easyL;
	} else if(ch == 'n') {
		printf("Thank you for playing the History game! We hope you enjoyed it!\n");
		exit(EXIT_SUCCESS);
	} else {
		printf("Sorry, you have typed in an invalid letter!\n");
		printf("Please type in a, y or n.\n");
		goto repeatCont;
	}


	easyL: ;
	printf("                                                               \n");
  printf("------------------------- Level: EASY -------------------------------\n");
	printf("                                                               \n");

	easylevel();

	/* easy -> medium */
  system("clear");
	int newScore = score;  // save current score

	if (score >= 2) {
		printf("                                                               \n");
		printf("All the questions in easy level are done\n");
		nextlevel();
	} else {  // If score is not enough
		printf("                                                               \n");
		printf("All the questions in easy level are done\n");
		printf("Sorry you didn't have enough score to go to medium level.\n");
		printf("Would you like to go to start again? [y/n]\n");
		score = 0;

		E_to_M: ;
		/* get input */
		char input;
		scanf(" %c", &input);
		char c = tolower(input);

    if(c == 'y') {
			Historyprogram();
		} else if(c == 'n') {
			printf("Thank you for playing the History game! We hope you enjoyed it!\n");
			exit(EXIT_SUCCESS);
		} else {
			printf("Sorry, you have typed in an invalid letter!\n");
			printf("Please type in a, y or n.\n");
			goto E_to_M;
		}
	}


	printf("------------------------- Level: MEDIUM -----------------------------\n");
  printf("                                                               \n");

  mediumlevel();

	/* medium -> hard */
	system("clear");

	if ((score - newScore) >= 2) {
		printf("                                                               \n");
		printf("All the questions in medium level are done\n");
		nextlevel();
	} else {  // If score is not enough
		printf("                                                               \n");
		printf("All the questions in medium level are done\n");
		printf("Sorry you didn't have enough score to go to hard level.\n");
		printf("Would you like to go to start again? [y/n]\n");
		score = 0;

		M_to_H: ;
		/* get input */
		char input;
		scanf(" %c", &input);
		char c = tolower(input);

    if(c == 'y') {
			Historyprogram();
		} else if(c == 'n') {
			printf("                                                               \n");
			printf("Thank you for playing the History game! We hope you enjoyed it!\n");
			exit(EXIT_SUCCESS);
		} else {
			printf("                                                               \n");
			printf("Sorry, you have typed in an invalid letter!\n");
			printf("Please type in a, y or n.\n");
			goto M_to_H;
		}
	}


	printf("------------------------- Level: HARD -----------------------------\n");
  printf("                                                               \n");

	hardlevel();

  printf("                 You have a total of %i points!!            \n",score);


	if (score == 10) {  // check if all questions are correct
		printf("             WOW You have got the full marks !!\n");
	}
	printf("                                                               \n");
	printf("Thank you for playing the History game! We hope you enjoyed it!\n");
	exit(EXIT_SUCCESS);
}

////////////////////////////// Hangman Game ///////////////////////////////////

void Hangmanprogram() {

	printf("~~~~~~~~~~~~~~~~~~~~Welcome to History Hangman!~~~~~~~~~~~~~~~~~~~~\n");
  printf("                                                               \n");
	printf("--------------------------------INFO---------------------------------\n");
	printf("                                                               \n");
	printf("      In this game, you will be asked about names of  \n");
	printf("             historical events took place.\n");
	printf("                                                               \n");
	printf("------------------------        ~~~        --------------------------\n");
	printf("                                                               \n");
	printf("      Write a letter each go, until you get the whole word  \n");
	printf("                                                               \n");
	printf("-------------------------------ENJOY--------------------------------\n");

  printf("Continue? [y/n]\n");
  /* Check if user would like to continue */
	repeatCont : ;
	char userIN;
	scanf(" %c", &userIN);
	char ch = tolower(userIN);

	if(ch == 'y') {
		system("clear");
	} else if(ch == 'n') {
		printf("Thank you for playing the History game! We hope you enjoyed it!\n");
		exit(EXIT_SUCCESS);
	} else {
		printf("Sorry, you have typed in an invalid letter!\n");
		printf("Please type in a, y or n.\n");
		goto repeatCont;
	}

	hangman();
}


int main(void) {

	printf("~~~~~~~~~~~~~~~~~~~~Welcome to our game!~~~~~~~~~~~~~~~~~~~~\n");
	printf("---------------------------------------------------------------\n");
	printf("To play Date game            TYPE ----------------  DATE \n");
	printf("To play History game         TYPE ----------------  HISTORY \n");
	printf("To play History Hangman game TYPE ----------------  HANGMAN \n");
  printf("---------------------------------------------------------------\n");

	/* check input */
	char buffer[11];
	scanf("%s", buffer);
	checkbuffer:;

	if(strcasecmp(buffer, "DATE") == 0)
	{
		Dateprogram();
	} else if(strcasecmp(buffer, "HISTORY") == 0) {
		Historyprogram();
	}	else if(strcasecmp(buffer, "HANGMAN") == 0) {
		Hangmanprogram();
	} else {
		printf("Sorry, you have typed in an invalid game!\n");
		printf("Please type in a game.\n");
		scanf("%s", buffer); goto checkbuffer;
	}

  exit(EXIT_SUCCESS);
}
