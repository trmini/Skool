#include "c:\mydocu~1\cosc2315\address\Boolean.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#ifndef PEOPLE_CLASS_H
#define PEOPLE_CLASS_H

const int Max_Size=40;

struct Name
 {
	char LastName[Max_Size];
	char FirstName[Max_Size];
	char MidName[Max_Size];
	Name();
 };

Name::Name()
 {
	LastName[0]=NULL;
	FirstName[0]=NULL;
	MidName[0]=NULL;
 }
struct Date
 {
	int Month;
	int Day;
	int Year;

	Date();
 };

Date::Date():Month(0),Day(0),Year(0)
 {
 }

struct PhoneNumber
 {
	char AreaCode[10];
	char LocalNumber[10];

	PhoneNumber();
 };
PhoneNumber::PhoneNumber()
 {
	AreaCode[0]=NULL;
	LocalNumber[0]=NULL;
 }

struct Address
 {
	char Address1[Max_Size];
	char Address2[Max_Size];
	char City[30];
	char State[30];
	char ZipCode[10];

	Address();
 };
Address::Address()
 {
	Address1[0]=NULL;
	Address2[0]=NULL;
	City[0]=NULL;
	State[0]=NULL;
	ZipCode[0]=NULL;

 }

class People
 {
	private:
		Name AName;
		Date Birthday;
		PhoneNumber APhone;
		Address AAddress;
	public:
		People();
		People(const People &Org);
		~People();
		People &operator=(const People &Right);
		Boolean operator==(const People &Right) const;
		Boolean operator!=(const People &Right) const;
		Boolean operator<(const People &Right) const;
		Boolean operator<=(const People &Right) const;
		Boolean operator>(const People &Right) const;
		Boolean operator>=(const People &Right) const;

		void Get(Name &aName) const;
		void Get(Date &birthday) const;
		void Get(PhoneNumber &aPhone) const;
		void Get(Address &aAddress) const;
		void Get(Name &aName,Date &birthday, PhoneNumber &APhone,Address &aAddress) const;

		void Input(const Name &aName);
		void Input(const Date &birthday);
		void Input(const PhoneNumber &aPhone);
		void Input(const Address &aAddress);
		void Input(const Name &aName,const Date &birthday, const PhoneNumber &APhone,const Address &aAddress);

		void GetKey(char KeyField[]) const;
		char *Key();
 };

People::People()
 {
 }

People::People(const People &Right)
 {
	AName=Right.AName;
	Birthday=Right.Birthday;
	APhone=Right.APhone;
	AAddress=Right.AAddress;
 }

People::~People()
 {
 }

People &People::operator=(const People &Right)
 {
	if (&Right!=this)
	 {
		AName=Right.AName;
		Birthday=Right.Birthday;
		APhone=Right.APhone;
		AAddress=Right.AAddress;
	 }
	return *this;
 }

Boolean People::operator==(const People &Right) const
 {
	if(strcmp(AName.LastName,Right.AName.LastName)==0)
		return True;
	else
		return False;
 }

Boolean People::operator!=(const People &Right) const
 {
	if(strcmp(AName.LastName,Right.AName.LastName)==0)
		return False;
	else
		return True;
 }

Boolean People::operator<(const People &Right) const
 {
	if(strcmp(AName.LastName,Right.AName.LastName)<=0)
		return True;
	else
		return False;
 }

Boolean People::operator<=(const People &Right) const
 {
	if(strcmp(AName.LastName,Right.AName.LastName)<=0)
		return True;
	else
		return False;
 }

Boolean People::operator>(const People &Right) const
 {
	if(strcmp(AName.LastName,Right.AName.LastName)>0)
		return True;
	else
		return False;
 }

Boolean People::operator>=(const People &Right) const
 {
	if(strcmp(AName.LastName,Right.AName.LastName)>=0)
		return True;
	else
		return False;
 }

void People::Get(Name &aName) const
 {
	aName=AName;
	return;
 }

void People::Get(Date &birthday) const
 {
	birthday=Birthday;
	return;
 }

void People::Get(PhoneNumber &aPhone) const
 {
	aPhone=APhone;
	return;
 }

void People::Get(Address &aAddress) const
 {
	aAddress=AAddress;
	return;
 }

void People::Get(Name &aName,Date &birthday, PhoneNumber &aPhone,Address &aAddress) const
 {
	aName=AName;
	birthday=Birthday;
	aPhone=APhone;
	aAddress=AAddress;
	return;
 }

void People::Input(const Name &aName)
 {
	AName=aName;
	return;
 }

void People::Input(const Date &birthday)
 {
	Birthday=birthday;
	return;
 }

void People::Input(const PhoneNumber &aPhone)
 {
	APhone=aPhone;
	return;
 }

void People::Input(const Address &aAddress)
 {
	AAddress=aAddress;
	return;
 }

void People::Input(const Name &aName,const Date &birthday, const PhoneNumber &aPhone,const Address &aAddress)
 {
	AName=aName;
	Birthday=birthday;
	APhone=aPhone;
	AAddress=aAddress;
	return;
 }

void People::GetKey(char KeyField[]) const
 {
	strcpy(KeyField,AName.LastName);
	return;
 }

char* People::Key()
 {
	char *key = AName.LastName;
	return key;
 }
#endif